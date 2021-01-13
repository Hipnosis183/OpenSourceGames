/**
 * BZip2 file creator and extractor, with progress bar support

 * Copyright (C) 2004  Andrew Reading
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008  Sylvain Beucler

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

#ifndef _BZIP_HPP
#define _BZIP_HPP

/* Put before wxWidgets headers, otherwise conflict with wx/string.h /
   windows.h / wxProgressDialog (in that order eventually in BZip.cpp)
   and crash under woe (arrrrgghhh!!!) */
#include <bzlib.h>

class wxProgressDialog;
#include "DFile.hpp"

class BZip : public DFile
{
public:
  BZip(const wxString &szFile);
  // Decompresses the file.  Returns a path to the decompressed file.
  wxString Extract( wxProgressDialog* aProgressDialog );

private:
  BZFILE* bzReadOpen(int* bzerror, FILE* f, int verbosity, int small, void* unused, int nUnused);
  void bzReadClose(int *bzerror, BZFILE *b);
  int bzRead(int* bzerror, BZFILE* b, void* buf, int len);
};

#endif
