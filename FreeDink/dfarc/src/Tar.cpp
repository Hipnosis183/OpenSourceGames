/**
 * Tar file creation and extraction, with progress bar support

 * Copyright (C) 2004  Andrew Reading
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008, 2014, 2018  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Tar.hpp"

#include <wx/intl.h>
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

#include <math.h>
#include <fstream>
#include <sstream>
#include <sys/types.h>

#include <bzlib.h>

#include "IOUtils.hpp"

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
  #define fopen(file, mode) _wfopen(file, _T(mode))
  // can't do the same with 'stat' as it's already a macro in mingw32

  // See doc/unicode.txt
  #include <ext/stdio_filebuf.h>
  std::istream* istream_open_unicode(const wchar_t* filename) {
    int fd = _wopen(filename, _O_RDONLY|O_BINARY);
    __gnu_cxx::stdio_filebuf<char>* filebuf =
      new __gnu_cxx::stdio_filebuf<char>(fd, std::ios::in|std::ios::binary);
    return new std::istream(filebuf);
  }
  std::ostream* ostream_open_unicode(const wchar_t* filename) {
    int fd = _wopen(filename, _O_CREAT|_O_WRONLY|_O_BINARY);
    __gnu_cxx::stdio_filebuf<char>* filebuf =
      new __gnu_cxx::stdio_filebuf<char>(fd, std::ios::out|std::ios::binary);
    return new std::ostream(filebuf);
  }
  void istream_close_unicode(std::istream* s) {
    delete s->rdbuf();
    delete s;
  }
  void ostream_close_unicode(std::ostream* s) {
    delete s->rdbuf();
    delete s;
  }
#else
  std::istream* istream_open_unicode(const char* filename) {
    return new std::ifstream(filename, std::ios::in|std::ios::binary);
  }
  std::ostream* ostream_open_unicode(const char* filename) {
    return new std::ofstream(filename, std::ios::out|std::ios::binary);
  }
  void istream_close_unicode(std::istream* s) {
    delete s;
  }
  void ostream_close_unicode(std::ostream* s) {
    delete s;
  }
#endif

struct fileinfo
{
  wxString fullpath;
  int mode;
  int blksize;
  int size;
  int mtime;
};

Tar::Tar(wxString &szFile, wxString &szCompressDir) : 
  DFile(szFile)
{
  strCompressDir = szCompressDir;
  bCanCompress = true;
}

Tar::Tar(wxString& szFile) : DFile(szFile)
{
  bCanCompress = false;
}

//
// TAR CREATE FUNCTIONS
//
bool Tar::Create(const wxString& dmod_folder, double *compression_ratio, wxProgressDialog* aProgressDialog)
{
  if (!bCanCompress)
    return wxEmptyString;

  // Remember current directory
  wxString strCwd = ::wxGetCwd();

  // Actual archiving
  ::wxSetWorkingDirectory(strCompressDir);
  bool result = CreateReal(dmod_folder, compression_ratio, aProgressDialog);

  // Back to where we were
  ::wxSetWorkingDirectory(strCwd);

  return result;
}

bool Tar::CreateReal(const wxString& dmod_folder, double *compression_ratio, wxProgressDialog* aProgressDialog)
{
  bool aborted = false;
  // Populate file list.
  wxArrayString wxasFileList;
  aProgressDialog->Update(0, _("Listing files..."));
  IOUtils::GetAllDModFiles(strCompressDir, wxasFileList);
  
  // We'll make the paths relative now
  int iNumEntries = wxasFileList.GetCount();

  // stat() all files, and get the total input size (for progress bar)
  fileinfo *fileinfos = new fileinfo[iNumEntries];
  int total_file_data = 0;
  for (unsigned int i = 0; i < wxasFileList.GetCount(); ++i)
    {
      fileinfos[i].fullpath = strCompressDir + _T("/") + wxasFileList.Item(i);
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
      struct _stat sb;
      if (_wstat(fileinfos[i].fullpath.fn_str(), &sb) < 0)
#else
      struct stat sb;
      if (stat(fileinfos[i].fullpath.fn_str(), &sb) < 0)
#endif
	{
	  perror("stat");
	  fileinfos[i].mode = 0; // invalid, to skip
	  continue;
	}
      fileinfos[i].mode = sb.st_mode;
      fileinfos[i].size = sb.st_size;
      fileinfos[i].mtime = sb.st_mtime;

      /* Preferred I/O block size */
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
      fileinfos[i].blksize = sb.st_blksize;
#else
      // fileinfos[i].blksize = BUFSIZ;  // BUFSIZ=512 under woe, performance issues reported
      fileinfos[i].blksize = 8192;
#endif

      total_file_data += sb.st_size;
    }


  aProgressDialog->Update(0, _("Initializing..."));
  FILE* out = fopen(mFilePath.fn_str(), "wb");
  if (!out)
    {
      wxLogError(_("Error: Could not open tar file '%s' for bzip compression."), mFilePath.c_str());
      return false;
    }
  
  /* libbz2 init */
  BZFILE* bz_out = 0;
  int iErr = 0;
  int compress_factor = 9;
  int debug_verbosity = 0;
  // workFactor is in [1,250], defaults to 30 (this is also what
  // 'bzip2' uses). 90 is what is used in DFArc1.5 and seems pretty
  // much arbitrary. The documentation is in BZ2_bzCompressInit - but
  // after reading it I can't say whether using 90 actually helps with
  // compression ratio or times. So let's reset to default (0 => 30).
  int workFactor = 0;
  bz_out = BZ2_bzWriteOpen(&iErr, out,
			   compress_factor,
			   debug_verbosity,
			   workFactor);
  
  if (iErr != BZ_OK)
    {
      wxLogError(_("Error: Could not initialize compression method!"
		     "  "
		     "Will not generate a correct .dmod file."
		     "  "
		     "Quitting."));
      fclose(out);
      return false;
    }
  

  int total_file_data_written = 0;
  for (unsigned int i = 0; i < wxasFileList.GetCount(); ++i)
    {
      // Update the progress bar.
      if (aProgressDialog != NULL)
        {
	  // Update the meter.
	  //int lPercent = 98 * i / iNumEntries;
	  int lPercent = 98 * (long long)total_file_data_written / total_file_data;
	  aProgressDialog->Update(lPercent, wxasFileList.Item(i));
        }
      
      // Write tar data.
      char header[512];
      if (!FillHeader(wxasFileList.Item(i), dmod_folder, header, &fileinfos[i]))
	continue;
      // Now, write our record.
      BZ2_bzWrite(&iErr, bz_out, header, 512);
      
      // Open our input file.
      FILE *in = fopen(fileinfos[i].fullpath.fn_str(), "rb");
      if (in == NULL)
        {
	  wxLogFatalError(_("Error: File '%s' not found!  Cannot archive file."),
			    fileinfos[i].fullpath.c_str());
	  throw;
        }
  

      // Write the file.
      int ebufsiz = fileinfos[i].blksize; // efficient buffer size
      unsigned char* szBuf = (unsigned char*)malloc(ebufsiz);
      int cur_file_nb_written = 0;

      while(!feof(in))
        {
	  int nb_read = fread(szBuf, 1, ebufsiz, in);
	  BZ2_bzWrite(&iErr, bz_out, szBuf, nb_read);
	  cur_file_nb_written += nb_read;
	  total_file_data_written += nb_read;
	  if (aProgressDialog != NULL)
	    {
	      int lPercent = 98 * (long long)total_file_data_written / total_file_data;
	      bool cont = aProgressDialog->Update(lPercent);
	      if (!cont)
		{
		  aborted = true;
		  fclose(in);
		  free(szBuf);
		  goto clean_up;
		}
	    }
        }
      fclose(in);

      // Round to the nearest 512-byte mark.
      // For empty file: no data (i.e. _not_ an empty extra 512 block)
      int iAmountToRound = (512 - (cur_file_nb_written % (512))) % 512;
      memset(szBuf, 0, iAmountToRound);
      BZ2_bzWrite(&iErr, bz_out, szBuf, iAmountToRound);
      
      // BE FREEEEEEE!!!
      free(szBuf);
    }	
  

  // Write the end-of-archive entry.
  char eoa[2*512];
  memset(eoa, 0, 2*512);
  BZ2_bzWrite(&iErr, bz_out, eoa, 2*512);
  
 clean_up:
  // Close the output file.
  aProgressDialog->Update(98, _("Closing..."));
  int force_flush = 0; /* no force flush */
  unsigned int nbytes_in = 0;
  unsigned int nbytes_out = 0;
  BZ2_bzWriteClose(&iErr, bz_out, force_flush, &nbytes_in, &nbytes_out);
  *compression_ratio = (double) nbytes_in / nbytes_out;
  fclose(out);
  delete[] fileinfos;
  
  aProgressDialog->Update(100); // Closes window
  return !aborted;
}


/** 
 * Fill in a tar header in a memory buffer
 *
 * It is not portable to do the same job in a C struct, because
 * there's no guarantee about how the C compiler will align the struct
 * fields in memory, which would break the checksum.
 *
 * Return whether to use this file (true) or skip it (false)
 */
bool Tar::FillHeader(wxString &mFilePath, const wxString& dmod_folder, char *header512, fileinfo *finfo)
{
  // Get file information
  // Only store regular files
  if (!S_ISREG(finfo->mode))
    return false;

  // Start collecting data.
  char* ptr = header512;

  // filename
  strncpy(ptr, (dmod_folder.Lower() + _T("/") + mFilePath.Lower()).mb_str(wxConvUTF8), 100);
  ptr += 100;
  
  // No need to save actual permissions and owner - just tidy them
  // mode
  strncpy(ptr, "0100644", 8);
  ptr += 8;
  // uid
  strncpy(ptr, "0000000", 8);
  ptr += 8;
  // gid
  strncpy(ptr, "0000000", 8);
  ptr += 8;
 
  // file size (octal on 11 bytes + '\0')
  sprintf(ptr, "%011o", finfo->size);
  ptr += 12;
  
  // time of modification (octal on 11 bytes + '\0')
  sprintf(ptr, "%011o", finfo->mtime);
  ptr += 12;

  // Before the actual checksum, consider that the checksum field is
  // filled with spaces
  memset(ptr, ' ', 8);
  ptr += 8;

  // Type of file.
  // linkflag
  *ptr = '0'; // Normal file
  ptr++;
  // linkname
  strncpy(ptr, "", 100);
  ptr += 100;

  // version
  strncpy(ptr, "ustar  ", 8);
  ptr += 8;

  // uname
  strncpy(ptr, "root", 32);
  ptr += 32;
  // gname
  strncpy(ptr, "root", 32);
  ptr += 32;

  // Skip devminor and devmajor.
  strncpy(ptr, "", 8);
  ptr += 8;
  strncpy(ptr, "", 8);
  ptr += 8;

  // Fill the padding.
  strncpy(ptr, "", 167);


  // Find the checksum.
  unsigned char* ptru = NULL; // use the (unsigned) ASCII value of characters
  int iChksum = 0;
  for (ptru = (unsigned char*) header512; ptru < (unsigned char*)(header512 + 512); ptru++)
    iChksum += *ptru;
  
  // (octal on 7 bytes + '\0')
  ptr = header512 + 148;
  sprintf(ptr, "%07o", iChksum);

  return true;
}


/**
 * Read all tar headers from a flux, so it can work if reading file
 * with BZip2 directly (only the 'read' operation is available).
 */
int Tar::ReadHeaders( void )
{
  FILE *in;
  TarHeader lHeader;
  TarRecord lRecord;
  unsigned int iBegData = 0;
  char buf_header[512];
  
  in = fopen(mFilePath.fn_str(), "rb");
  
  if(in == NULL)
    {
      wxLogFatalError(_("Error: File '%s' not found!  Cannot read data."), mFilePath.c_str());
      return 1;
    }
  
  wxString lDmodDizPath;
  mmDmodDescription = _T("");
  mInstalledDmodDirectory = _T("");

  int total_read = 0;
  while (true)
    {
      memset(&lHeader, 0, sizeof(TarHeader));
      memset(&lRecord, 0, sizeof(TarRecord));
      
      // Read the data. Don't load the whole header to a struct, you
      // never know what the memory alignment will be, especially in
      // these 32->64bits days
      fread((char*)&lHeader.Name, 100, 1, in);
      fread((char*)&lHeader.Mode, 8, 1, in);
      fread((char*)&lHeader.Uid, 8, 1, in);
      fread((char*)&lHeader.Gid, 8, 1, in);
      fread((char*)&lHeader.Size, 12, 1, in);
      fread((char*)&lHeader.Mtime, 12, 1, in);
      fread((char*)&lHeader.Chksum, 8, 1, in);
      fread((char*)&lHeader.Linkflag, 1, 1, in);
      fread((char*)&lHeader.Linkname, 100, 1, in);
      fread((char*)&lHeader.Magic, 8, 1, in);
      fread((char*)&lHeader.Uname, 32, 1, in);
      fread((char*)&lHeader.Gname, 32, 1, in);
      fread((char*)&lHeader.Devmajor, 8, 1, in);
      fread((char*)&lHeader.Devminor, 8, 1, in);
      fread((char*)&lHeader.Padding, 167, 1, in);
      total_read += 512;

      if(!VerifyChecksum(&lHeader))
        {
	  // Nope.  Exit.
	  wxLogFatalError(_("Error: This .dmod file has an invalid checksum!  Cannot read file."));
	  return 1;
        }
        
      strncpy(lRecord.Name, lHeader.Name, 100);
      
      // Bug fix for phantom files that aren't really files...
      // This is due to a bug in DFArc1.5 with empty files
      if (strcmp(lHeader.Name, "\xFF") == 0)
	continue;
      
      // Size
      sscanf((const char*)&lHeader.Size, "%o", &lRecord.iFileSize);
      // Start
      lRecord.iFilePosBegin = total_read;
      
      // Tar files (except for a design bug in DFArc1.5) have a
      // multitude of NULL characters at the end to round up to the
      // 10K mark.  If the header is blank, we don't have any more
      // data.
      if(strcmp(lHeader.Name, "") == 0)
        {
	  break;
        }
      //wxLogError("%s %d", lHeader.Name, lRecord.iFileSize);
      
      wxString lPath(lRecord.Name, wxConvUTF8);
      if (mInstalledDmodDirectory.Length() == 0)
        {
	  // Security: ensure the D-Mod directory is non-empty
	  wxString firstDir = GetFirstDir(lPath);
	  if (firstDir.IsSameAs("", true) || firstDir.IsSameAs("..", true) || firstDir.IsSameAs("dink", true))
            {
	      wxLogError(_("Error: invalid D-Mod directory.  Stopping."));
              return 1;
            }
          mInstalledDmodDirectory = firstDir;

	  lDmodDizPath = mInstalledDmodDirectory + _T("dmod.diz");
	  lDmodDizPath.LowerCase();
        }
      if (lPath.Lower() == lDmodDizPath && lRecord.iFileSize < 100*1024)
	{
	  // Read dmod.diz content, but only if < 100kB
	  char *buf = (char*)malloc(lRecord.iFileSize + 1);
	  fread(buf, 1, lRecord.iFileSize, in);
	  buf[lRecord.iFileSize] = '\0';
	  // dmod.diz are typically uncoded in ISO-8859-1/latin-1
	  mmDmodDescription = wxString(buf, wxConvISO8859_1);
	  free(buf);
	}
      else
	{
	  int remaining = lRecord.iFileSize;
	  char buf[BUFSIZ];
	  while (remaining > 0)
	    {
	      if (feof(in))
		break; // TODO: error, unexpected end of file
	      int nb_read = fread(buf, 1, (remaining > BUFSIZ) ? BUFSIZ : remaining, in);
	      remaining -= nb_read;
	    }
	}
      total_read += lRecord.iFileSize;
      TarRecords.push_back(lRecord);
      
      // Move to the beginning of the next header
      int padding_size = (512 - (total_read % 512)) % 512;
      fread(buf_header, 1, padding_size, in);
      total_read += padding_size;
    }
  
  fclose(in);
  
  return 0;
}

int Tar::Extract(wxString destdir, wxProgressDialog* aProgressDialog)
{
    wxString strBuf;
    int lError = 0;

    // Open the file here so it doesn't error after changing.
    std::istream* f_In = istream_open_unicode(mFilePath.fn_str());

    if(!f_In->good())
    {
      lError = 1;
      wxLogFatalError(_("Error: File '%s' not found!  Cannot extract data."), mFilePath.c_str());
      throw;
    }
    
    // Attempt to create destination if not present (e.g. custom
    // DModDir that is not created yet)
    if (!::wxDirExists(destdir))
      wxFileName::Mkdir(destdir, 0777, wxPATH_MKDIR_FULL); // 0777 minus umask
    if (!::wxDirExists(destdir))
      {
	wxLogFatalError(_("Error: Cannot create directory '%s'.  Cannot extract data."), destdir.c_str());
	throw;
      }

    // Put the data in the directories.
    if (ExtractData(f_In, destdir, aProgressDialog) != 0)
    {
        lError = 1;
    }
    istream_close_unicode(f_In);

    return lError;
}

int Tar::ExtractData(std::istream* aTarStreamIn, wxString destdir, wxProgressDialog* aProgressDialog)
{
    int lError = 0;

    unsigned long lTotalBytes = 0;
    unsigned long lTotalBytesRead = 0;

    // Get the file size
    aTarStreamIn->seekg(0, std::ios::end);
    unsigned long lEnd = static_cast<unsigned long>(aTarStreamIn->tellg());
    aTarStreamIn->seekg(0, std::ios::beg);
    lTotalBytes = lEnd - static_cast<unsigned long>(aTarStreamIn->tellg());

    // Extract the files.
    int ebufsiz = 8192;
    char buffer[ebufsiz];
    for (unsigned int lTarRecordIndex = 0; lTarRecordIndex < TarRecords.size(); ++lTarRecordIndex)
    {
		TarRecord lCurrentTarRecord = TarRecords.at(lTarRecordIndex);
		wxString lCurrentFilePath = wxString(lCurrentTarRecord.Name, wxConvUTF8);
		if (lCurrentFilePath.IsEmpty())
		  {
		    /* Attempt convertion from latin-1 if not valid UTF-8 */
		    lCurrentFilePath = wxString(lCurrentTarRecord.Name, wxConvISO8859_1);
		  }
        // Security: check if archive tries to jump out of destination directory
        if (IsPathInsecure(lCurrentFilePath))
        {
            wxLogError(_("Error: Insecure filename: '%s'.  Stopping."), lCurrentFilePath);
            lError = 1;
            break;
        }
        // Security: ensure full, non-relative path, under destdir/
        lCurrentFilePath = destdir + wxFileName::GetPathSeparator() + lCurrentFilePath;
        wxString lCurrentDirectory = lCurrentFilePath.substr(0, lCurrentFilePath.find_last_of("/\\"));

        // Odd bad file problem...
	if (lCurrentFilePath.compare(_T("\xFF")) == 0) // "ÿ"
        {
            // Only warn them if it doesn't come at the end.
            if (lTarRecordIndex != TarRecords.size() - 1)
            {
                lError = 1;
                wxLogError(_("Got bad file %d/%d.  Skipping."), lTarRecordIndex, TarRecords.size() );
            }
        }
        else
	{
	    // Create the directory if it doesn't exist
	    if (::wxDirExists(lCurrentDirectory) == false)
	    {
		// Use 0777 - the umask will remove group and/or
		// other write access as necessary. Don't forget
		// to prefix with a '0' for octal mode.
		wxFileName::Mkdir(lCurrentDirectory, 0777, wxPATH_MKDIR_FULL);
	    }
	    
            // Only write the file if it is a file... some files are stored as directories
            char lLastCharacter = lCurrentFilePath[lCurrentFilePath.length() - 1];
            if (lLastCharacter != '\\' && lLastCharacter != '/')
            {
                // Open the file.
                std::ostream* lStreamOut = ostream_open_unicode(lCurrentFilePath);
		if (!lStreamOut->good())
                {
		    wxLogError(_("Error: Improperly archived file '%s'.  Skipping."), lCurrentFilePath);
                    lError = 1;
                    continue;
                }
                
		aTarStreamIn->seekg(lCurrentTarRecord.iFilePosBegin, std::ios::beg);
		long remaining = lCurrentTarRecord.iFileSize;
		while (remaining > 0)
		  {
		    int bufsiz = remaining;
		    if (bufsiz > ebufsiz) bufsiz = ebufsiz;
		    aTarStreamIn->read(buffer, bufsiz);
		    lStreamOut->write(buffer, bufsiz);
		    remaining -= bufsiz;
		  }
                lTotalBytesRead += lCurrentTarRecord.iFileSize;
                
                // Close up.
                lStreamOut->flush();
		ostream_close_unicode(lStreamOut);
            }
            
            // Update progress
            if( aProgressDialog != NULL )
            {
                double lPercent( 100.0 * lTotalBytesRead / lTotalBytes ) ;
                if ( lPercent >= 100 )
                {
                    lPercent = 99.0;
                }
                aProgressDialog->Update(static_cast<int>(lPercent), lCurrentFilePath);
                ::wxYield();
            }
        }
    }
    aProgressDialog->Update(100, _("Done."));
    return lError;
}

wxString Tar::getInstalledDmodDirectory( void )
{
    return mInstalledDmodDirectory;
}

wxString Tar::getmDmodDescription( void )
{
    return mmDmodDescription;
}


////////////////////
// Tar Operations //
////////////////////

//
// class CTar, bool VerifyChecksum(TarHeader *Header)
//
// PURPOSE: Ensures the internal Header checksum is valid.
// RETURNS: True if valid.
//          False otherwise.
bool Tar::VerifyChecksum(TarHeader *Header)
{
    char *CheckRecord = 0;
    int iChksum = 0;

    // Checksum is counted as if it was blank.  Let's back it up and clear it.
    memset(Header->Chksum, 0, sizeof(Header->Chksum));

    // Prepare the incrementor.
    CheckRecord = reinterpret_cast<char*>(Header);

    // Find the checksum.
    for(; (CheckRecord - reinterpret_cast<char*>(Header)) < 512; ++CheckRecord)
    {
        iChksum += static_cast<int>(*CheckRecord);
    }

    // Add 256 (2^8)
    iChksum += 256;

    char sz[8];
    sprintf(sz, "%07o", iChksum);

    /*if(atoi(sz) == atoi(strChksum.c_str()))
    {
        return true;
    }*/

    //return false;
    return true;
}


wxString Tar::GetFirstDir(wxString path) {
    wxString firstDir = "";
    wxString previousDir = "";
    // tokenizer never returns empty strings + distinguish dir// and a/$
    if (path.EndsWith("/") || path.EndsWith("\\"))
        path += "dummy";
    wxStringTokenizer tokenizer(path, "/\\", wxTOKEN_STRTOK);
    while (tokenizer.HasMoreTokens()) {
        wxString curDir = tokenizer.GetNextToken();
        if (curDir == '.')
	    continue;
        if (previousDir != "") {
	  firstDir = previousDir;
	  break;
	}
        previousDir = curDir;
    }
    return firstDir;
}

// Security: check if archive tries to jump out of destination directory
bool Tar::IsPathInsecure(wxString path) {
    // Avoid leading slashes (even if we preprend destdir)
    if (path[0] == '/' || path[0] == '\\')
        return true;
    // Avoid ':' since wxFileName::Mkdir silently normalizes
    // e.g. C:\test1\C:\test2 to C:\test2
    if (path.Contains(":"))
        return true;
    // Ensure all files reside in the same subdirectory
    if (GetFirstDir(path) != mInstalledDmodDirectory)
        return true;
    // Ensure there's no '..' path element
    wxStringTokenizer tokenizer(path, "/\\");
    while (tokenizer.HasMoreTokens()) {
        wxString token = tokenizer.GetNextToken();
        if (token == "..")
            return true;
    }
    return false;
}
