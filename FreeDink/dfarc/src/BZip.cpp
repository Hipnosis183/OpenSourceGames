/**
 * BZip2 file creator and extractor, with progress bar support

 * Copyright (C) 2004  Andrew Reading
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2007, 2008, 2014  Sylvain Beucler

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

#include "BZip.hpp"

#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/progdlg.h>

#include <stdio.h>

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define fopen(file, mode) _wfopen(file, _T(mode))
#endif

BZip::BZip(const wxString &szFile) : DFile(szFile)
{
}


// Modified version of bzip2-1.0.6/bzlib.c and bzip2.c where we can
// get how many compressed data (not uncompressed data) was read
// (access to private field BZFILE->strm.total_in_lo32). This is, of
// course, only the high-level API, we still rely on an external
// libbz2 for the actual decompression.

/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.0.6 of 6 September 2010
   Copyright (C) 1996-2010 Julian Seward <jseward@bzip.org>
   Copyright (C) 2008, 2014  Sylvain Beucler

   Please read the WARNING, DISCLAIMER and PATENTS sections in the 
   README file.

--------------------------------------------------------------------------

This program, "bzip2", the associated library "libbzip2", and all
documentation, are copyright (C) 1996-2010 Julian R Seward.  All
rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. The origin of this software must not be misrepresented; you must 
   not claim that you wrote the original software.  If you use this 
   software in a product, an acknowledgment in the product 
   documentation would be appreciated but is not required.

3. Altered source versions must be plainly marked as such, and must
   not be misrepresented as being the original software.

4. The name of the author may not be used to endorse or promote 
   products derived from this software without specific prior written 
   permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Julian Seward, jseward@bzip.org
bzip2/libbzip2 version 1.0.6 of 6 September 2010

-------------------------------------------------------------------------- */

typedef char            Char;
typedef unsigned char   Bool;
typedef unsigned char   UChar;
typedef int             Int32;
typedef unsigned int    UInt32;
typedef short           Int16;
typedef unsigned short  UInt16;

#define True  ((Bool)1)
#define False ((Bool)0)


#define BZ_SETERR(eee)                    \
{                                         \
   if (bzerror != NULL) *bzerror = eee;   \
   if (bzf != NULL) bzf->lastErr = eee;   \
}

typedef 
   struct {
      FILE*     handle;
      Char      buf[BZ_MAX_UNUSED];
      Int32     bufN;
      Bool      writing;
      bz_stream strm;
      Int32     lastErr;
      Bool      initialisedOk;
   }
   bzFile;


/*---------------------------------------------*/
static Bool myfeof ( FILE* f )
{
   Int32 c = fgetc ( f );
   if (c == EOF) return True;
   ungetc ( c, f );
   return False;
}

/*---------------------------------------------------*/
BZFILE* BZip::bzReadOpen
                   ( int*  bzerror, 
                     FILE* f, 
                     int   verbosity,
                     int   small,
                     void* unused,
                     int   nUnused )
{
   bzFile* bzf = NULL;
   int     ret;

   BZ_SETERR(BZ_OK);

   if (f == NULL || 
       (small != 0 && small != 1) ||
       (verbosity < 0 || verbosity > 4) ||
       (unused == NULL && nUnused != 0) ||
       (unused != NULL && (nUnused < 0 || nUnused > BZ_MAX_UNUSED)))
      { BZ_SETERR(BZ_PARAM_ERROR); return NULL; };

   if (ferror(f))
      { BZ_SETERR(BZ_IO_ERROR); return NULL; };

   bzf = (bzFile*)malloc ( sizeof(bzFile) );  // DFArc: explicit cast for C++
   if (bzf == NULL) 
      { BZ_SETERR(BZ_MEM_ERROR); return NULL; };

   BZ_SETERR(BZ_OK);

   bzf->initialisedOk = False;
   bzf->handle        = f;
   bzf->bufN          = 0;
   bzf->writing       = False;
   bzf->strm.bzalloc  = NULL;
   bzf->strm.bzfree   = NULL;
   bzf->strm.opaque   = NULL;
   
   while (nUnused > 0) {
      bzf->buf[bzf->bufN] = *((UChar*)(unused)); bzf->bufN++;
      unused = ((void*)( 1 + ((UChar*)(unused))  ));
      nUnused--;
   }

   ret = BZ2_bzDecompressInit ( &(bzf->strm), verbosity, small );
   if (ret != BZ_OK)
      { BZ_SETERR(ret); free(bzf); return NULL; };

   bzf->strm.avail_in = bzf->bufN;
   bzf->strm.next_in  = bzf->buf;

   bzf->initialisedOk = True;
   return bzf;   
}


/*---------------------------------------------------*/
void BZip::bzReadClose ( int *bzerror, BZFILE *b )
{
   bzFile* bzf = (bzFile*)b;

   BZ_SETERR(BZ_OK);
   if (bzf == NULL)
      { BZ_SETERR(BZ_OK); return; };

   if (bzf->writing)
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return; };

   if (bzf->initialisedOk)
      (void)BZ2_bzDecompressEnd ( &(bzf->strm) );
   free ( bzf );
}


/*---------------------------------------------------*/
int BZip::bzRead
           ( int*    bzerror, 
             BZFILE* b, 
             void*   buf, 
             int     len )
{
   Int32   n, ret;
   bzFile* bzf = (bzFile*)b;

   BZ_SETERR(BZ_OK);

   if (bzf == NULL || buf == NULL || len < 0)
      { BZ_SETERR(BZ_PARAM_ERROR); return 0; };

   if (bzf->writing)
      { BZ_SETERR(BZ_SEQUENCE_ERROR); return 0; };

   if (len == 0)
      { BZ_SETERR(BZ_OK); return 0; };

   bzf->strm.avail_out = len;
   bzf->strm.next_out = (char*)buf;  // DFArc: explicit cast for C++

   while (True) {

      if (ferror(bzf->handle)) 
         { BZ_SETERR(BZ_IO_ERROR); return 0; };

      if (bzf->strm.avail_in == 0 && !myfeof(bzf->handle)) {
         n = fread ( bzf->buf, sizeof(UChar), 
                     BZ_MAX_UNUSED, bzf->handle );
         if (ferror(bzf->handle))
            { BZ_SETERR(BZ_IO_ERROR); return 0; };
         bzf->bufN = n;
         bzf->strm.avail_in = bzf->bufN;
         bzf->strm.next_in = bzf->buf;
      }

      ret = BZ2_bzDecompress ( &(bzf->strm) );

      if (ret != BZ_OK && ret != BZ_STREAM_END)
         { BZ_SETERR(ret); return 0; };

      if (ret == BZ_OK && myfeof(bzf->handle) && 
          bzf->strm.avail_in == 0 && bzf->strm.avail_out > 0)
         { BZ_SETERR(BZ_UNEXPECTED_EOF); return 0; };

      if (ret == BZ_STREAM_END)
         { BZ_SETERR(BZ_STREAM_END);
           return len - bzf->strm.avail_out; };
      if (bzf->strm.avail_out == 0)
         { BZ_SETERR(BZ_OK); return len; };
      
   }

   return 0; /*not reached*/
}



//
// wxString BZip::Extract(wxGauge *PBar = 0)
//
// Decompresses a BZip file.  Takes filename to decompress from mFilePath.
//
//     PBar - Parameter passed to a pre-made wxGauge displaying the progress of the decompression.
//
// Returns: Decompressed file location.
//
wxString BZip::Extract( wxProgressDialog* aProgressDialog )
{
  // First, grab a random filename.
  wxString lTempFileName = wxFileName::CreateTempFileName(_T("DFArc"));
  int lError(0);
  unsigned long lTotalBytes = 0;
  
  // Begin decompress stuff.
  FILE* lDmodFileIn = fopen(mFilePath.fn_str(), "rb");
    
    if ( lDmodFileIn == NULL )
      {
	wxLogError(_("Error: Could not open input file '%s' for bzip decompression."), mFilePath.c_str());
        return _T("");
      }

    // Get the file size
    fseek( lDmodFileIn, 0, SEEK_END );
    unsigned long lEnd = ftell( lDmodFileIn );
    fseek( lDmodFileIn, 0, SEEK_SET );
    lTotalBytes = lEnd - ftell( lDmodFileIn );
    lTotalBytes -= 10; // header

    BZFILE* lBZipFileIn = bzReadOpen( &lError, lDmodFileIn, 0, 0, 0, 0 );

    if( lError != BZ_OK )
      {
	wxLogError(_("Error: Invalid .dmod file selected!"));
	fclose(lDmodFileIn);
        return _T("");
      }

    // Open up the output file
    FILE* lTarFileOut = fopen(lTempFileName.fn_str(), "wb");
    
    if (lTarFileOut == NULL)
      {
	wxLogError(_("Error: Could not write to '%s'."), lTempFileName.c_str());
        fclose(lDmodFileIn);
        return _T("");
    }

    /* Efficient buffer size */
    // int ebufsiz = BUFSIZ;  // BUFSIZ=512 under woe, performance issues reported
    int ebufsiz = 8192;
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
    struct stat sb;
    if (fstat(fileno(lTarFileOut), &sb) < 0)
      perror("stat");
    else
      /* Preferred I/O block size */
      ebufsiz = sb.st_blksize;
#endif
    char lBuffer[ebufsiz];

    // Keep getting chunks of the file
    while (true)
      {
        int lCurrentBytesRead = bzRead(&lError, lBZipFileIn, lBuffer, ebufsiz);
        fwrite(lBuffer, lCurrentBytesRead, 1, lTarFileOut);

        if ( lError != BZ_OK )
        {
            // Success
            if ( lError == BZ_STREAM_END )
            {
                break;
            }
            else if ( lError == BZ_PARAM_ERROR )
            {
                wxLogError( _("Invalid .dmod file specified.") );
            }
            else if ( lError == BZ_SEQUENCE_ERROR )
            {
                wxLogError( _("Critical program function error: opened for write.") );
            }
            else if ( lError == BZ_IO_ERROR )
            {
                wxLogError( _("Could not read .dmod file.") );
            }
            else if ( lError == BZ_UNEXPECTED_EOF )
            {
                wxLogError( _("Incomplete .dmod file. Please download it again.") );
            }
            else if ( lError == BZ_DATA_ERROR )
            {
                wxLogError( _("The .dmod file is corrupted.  Please download it again.") );
            }
            else if ( lError == BZ_DATA_ERROR_MAGIC )
            {
                wxLogError( _("The file is not a valid .dmod file.") );
            }
            else if ( lError == BZ_MEM_ERROR )
            {
                wxLogError( _("Out of memory error.") );
            }
            else
            {
                wxLogError( _("An unhandled error occurred.") );
            }
            wxRemoveFile(lTempFileName);
            lTempFileName = _T("");
            break;
        }
        if (aProgressDialog != NULL)
        {
	  int total_read = ((bzFile*)lBZipFileIn)->strm.total_in_lo32;
	  int lPercent = 100 * (long long)total_read / lTotalBytes;
	  aProgressDialog->Update(lPercent);
        }
    }

    bzReadClose(&lError, lBZipFileIn);
    fclose(lDmodFileIn);
    fclose(lTarFileOut);

    return lTempFileName;
}
