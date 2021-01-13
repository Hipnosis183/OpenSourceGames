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

#include <wx/file.h>
#include <wx/filename.h>

#include "DMod.hpp"
#include "IOUtils.hpp"

/**
 * DMod auto-detection: either main.c or start.c must exist (otherwise
 * I don't see how a D-Mod can be started).
 */
bool DMod::IsADModDir(wxString full_path)
{
  wxString mainc_path = full_path + _T("/story/main.c");
  wxString startc_path = full_path + _T("/story/start.c");
  wxString maind_path = full_path + _T("/story/main.d");
  wxString startd_path = full_path + _T("/story/start.d");
  IOUtils::ciconvert(mainc_path);
  IOUtils::ciconvert(startc_path);
  IOUtils::ciconvert(maind_path);
  IOUtils::ciconvert(startd_path);
  return ::wxFileExists(mainc_path) || ::wxFileExists(startc_path)
    || ::wxFileExists(maind_path) || ::wxFileExists(startd_path);
}

DMod::DMod(wxString full_path)
{
  mFullPath = full_path;

  wxFileName tmp(full_path); // no trailing slash
  mBaseName = tmp.GetName();

  mDescription = GetDModDizContent();
  mTitle = GetDModDizTitle();

  wxString lGfxPreview(mFullPath + _T("/preview.bmp"));
  IOUtils::ciconvert(lGfxPreview);
  wxString lGfxTitle(mFullPath + _T("/graphics/title-01.bmp"));
  IOUtils::ciconvert(lGfxTitle);

  if (::wxFileExists(lGfxPreview))
    mPreviewPath = lGfxPreview;
  else if (::wxFileExists(lGfxTitle))
    mPreviewPath = lGfxTitle;
  else
    mPreviewPath = wxEmptyString;
}


wxString DMod::GetFullPath()
{
  return mFullPath;
}
wxString DMod::GetBaseName()
{
  return mBaseName;
}
wxString DMod::GetTitle()
{
  return mTitle;
}
wxString DMod::GetDescription()
{
  return mDescription;
}
wxString DMod::GetPreviewPath()
{
  return mPreviewPath;
}


wxString DMod::GetDModDizContent()
{
  wxString content;

  wxString lDModDizPath = mFullPath + _T("/dmod.diz");
  IOUtils::ciconvert(lDModDizPath);

  if (!lDModDizPath.IsEmpty() && ::wxFileExists(lDModDizPath))
    {
      wxFile f(lDModDizPath);
      if (f.Length() < 100000)
	{
	  char *buf = (char*)malloc(f.Length() + 1);
	  f.Read(buf, f.Length());
	  buf[f.Length()] = '\0';
	  // dmod.diz are typically uncoded in ISO-8859-1/latin-1
	  // TODO: find a way for new DMod authors to specify UTF-8
	  content = wxString(buf, wxConvISO8859_1);
	  free(buf);
	  f.Close();
	}
    }
  return content;
}

wxString DMod::GetDModDizTitle()
{
  wxString lTitle;
  char ch;
  int i = 0;
  while(((ch = mDescription.GetChar(i)) != '\n')
	&& (ch != '\r')
	&& (i < mDescription.Len()))
    i++;

  if (i > 0)
    lTitle = mDescription.Mid(0, i);
  else
    lTitle = mBaseName;

  return lTitle;
}
