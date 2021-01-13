/**
 * Tar file creation and extraction, with progress bar support

 * Copyright (C) 2004  Andrew Reading
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008, 2018  Sylvain Beucler

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

#ifndef _TAR_HPP
#define _TAR_HPP

#include <wx/string.h>
#include <wx/progdlg.h>

#include <vector>

#include "DFile.hpp"

struct fileinfo;

class TarHeader
{
public:
  char Name[100];      // 100
  char Mode[8];        // 108
  char Uid[8];         // 116
  char Gid[8];         // 124
  char Size[12];       // 136
  char Mtime[12];      // 148
  char Chksum[8];      // 156
  char Linkflag;       // 157
  char Linkname[100];  // 257
  char Magic[8];       // 265
  char Uname[32];      // 297
  char Gname[32];      // 329
  char Devmajor[8];    // 337
  char Devminor[8];    // 345
  char Padding[167];   // 512
};


class TarRecord
{
public:
  char Name[100];
  int iFilePosBegin;
  int iFileSize;
};


class Tar : public DFile
{
public:
  Tar(wxString &szFile, wxString &szCompressDir);
  Tar(wxString &szFile);
  
  bool Create(const wxString& aFolderName, double *compression_ratio, wxProgressDialog* aProgressDialog);
  int Extract(wxString destdir, wxProgressDialog* aProgressDialog);
  // tarmisc.cpp:
  int ReadHeaders( void );
  wxArrayString mPathList;
  wxString getInstalledDmodDirectory();
  wxString getmDmodDescription();

private:
  bool FillHeader(wxString &strFile, const wxString& lDestinationFile, char* header512, fileinfo* finfo);
  void DecToOct(int n, char *sz, int iMaxSize);
  const char* OctToDec(int n);
  const char* ToString(int n);
  bool PadWithZeros(char *s, int iszSize, int iAcheiveSize);
  bool VerifyChecksum(TarHeader *Header);
  bool CreateReal(const wxString& aFolderName, double *compression_ratio, wxProgressDialog* aProgressDialog);
  int ExtractData(std::istream* f_In, wxString destdir, wxProgressDialog* aProgressDialog);
  wxString GetFirstDir(wxString path);
  bool IsPathInsecure(wxString path);
  
  // True if the proper constructor was used.  The compression method
  // will not screw up if this flag is set to true.
  bool bCanCompress;
  
  // The directory to compress.  
  wxString strCompressDir;
  std::vector<TarRecord> TarRecords;
  wxString mInstalledDmodDirectory;
  wxString mmDmodDescription;
};

#endif
