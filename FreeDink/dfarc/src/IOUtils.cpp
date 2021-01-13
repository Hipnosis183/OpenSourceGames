/**
 * Files utilities

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

#include "IOUtils.hpp"

#include <wx/filename.h>
#include <wx/dir.h>

/**
 * Convert a case-insensitive filename to case-sensitive
 */
void IOUtils::ciconvert(wxString& path)
{
  // Nothing to simulate if the filesystem is already case-insensitive
  if (!wxFileName::IsCaseSensitive())
    return;

  wxString path_tmp_copy(path);
  path_tmp_copy.Append(wxT("/")); // simulate a directory for GetDirs()
  wxFileName filename(path_tmp_copy);
  wxArrayString path_elements = filename.GetDirs(); // split path

  wxString curpath;
  if (path.StartsWith(wxT("/")))
    curpath = wxT("/");
  else
    curpath = wxT("./");
  for (int i = 0; i < path_elements.Count(); i++)
    {
      wxDir cur_dir(curpath);
      wxString found = wxEmptyString;
      wxString filename;
      bool cont = cur_dir.GetFirst(&filename);
      while (cont)
	{
	  if (path_elements.Item(i).IsSameAs(filename, false)) // case-insensitive
	    {
	      found = filename;
	      break;
	    }
	  cont = cur_dir.GetNext(&filename);
	}

      if (found.IsEmpty())
	// can't find current element, non-existent path
	return;
      
      curpath = curpath + wxT("/") + found;
    }
  path = curpath;
}

wxString IOUtils::GetParentDir(wxString path)
{
  wxString result;
  path.Append(wxT("/")); // simulate a directory for RemoveLastDir()
  wxFileName filename(path);
  filename.RemoveLastDir();
  return filename.GetPath();
}

/* ------------------- */
/* --- GetAllFiles --- */
/* ------------------- */

class wxDirTraverserDMod : public wxDirTraverser
{
public:
  wxDirTraverserDMod(wxString& strPath, wxArrayString& files)
    : m_files(files), 
      m_strPath(strPath)
  {}
  
  virtual wxDirTraverseResult OnFile(const wxString& filename)
  {
    // Filter out savegames, as well as cache and backup files
    wxString cmp = wxFileName::FileName(filename).GetFullName().Lower();
    if (!cmp.Matches(_T("save*.dat"))
	&& !cmp.Matches(_T("thumbs.db"))
	&& !cmp.Matches(_T("*~"))
	&& !cmp.Matches(_T("debug.txt")))
      {
	wxFileName wxfn(filename);
	wxfn.MakeRelativeTo(m_strPath);
	m_files.Add(wxfn.GetFullPath(wxPATH_UNIX));
      }
    return wxDIR_CONTINUE;
  }
  
  virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
  {
    return wxDIR_CONTINUE;
  }
  
private:
  wxArrayString& m_files;
  wxString m_strPath;
};


/**
 * Get a list filenames in a DMod (relative), excluding common
 * temporary files and savegames.
 */
void IOUtils::GetAllDModFiles(wxString strPath, wxArrayString &wxasFileList)
{
  // get the names of all files in the array
  wxDirTraverserDMod traverser(strPath, wxasFileList);
  wxDir dir(strPath);
  dir.Traverse(traverser);
}
