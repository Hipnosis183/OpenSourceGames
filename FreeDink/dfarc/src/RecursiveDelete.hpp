/**
 * File utilities to uninstall a D-Mod

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

#ifndef _RECURSIVEDELETE_HPP
#define _RECURSIVEDELETE_HPP

#include <wx/dir.h>

class RecursiveDelete : public wxDirTraverser
{
public:
  RecursiveDelete(bool aRemoveSaveGames);

  virtual wxDirTraverseResult OnFile(const wxString& aFilename);
  virtual wxDirTraverseResult OnDir(const wxString& aDirname);
  bool getError( void );
  
protected:
private:
  bool mRemoveSaveGames;
  bool mError;
};

#endif
