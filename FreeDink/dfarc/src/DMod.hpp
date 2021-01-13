/**
 * DMod metadata

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

#ifndef _DMOD_HPP
#define _DMOD_HPP

#include <wx/string.h>

class DMod
{
public:
  static bool IsADModDir(wxString path);
  DMod(wxString path);
  wxString GetFullPath();
  wxString GetBaseName();
  wxString GetTitle();
  wxString GetDescription();
  wxString GetPreviewPath();

private:
  wxString mFullPath;
  wxString mBaseName;
  wxString mTitle;
  wxString mDescription;
  wxString mPreviewPath;

  wxString GetDModDizContent();
  wxString GetDModDizTitle();
};

#endif
