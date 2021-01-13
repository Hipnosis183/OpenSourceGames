/**
 * Files utilities

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

#ifndef _IOUTILS_HPP
#define _IOUTILS_HPP

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>

class IOUtils
{
public:
  static void ciconvert(wxString& strPath);
  static wxString GetParentDir(wxString full_path);
  static void GetAllDModFiles(wxString strPath, wxArrayString &wxasFileList);

private:   
  //void FixSlashes(wxString &Path);
};
#endif
