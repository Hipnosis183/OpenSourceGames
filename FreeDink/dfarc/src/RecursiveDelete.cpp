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

#include "RecursiveDelete.hpp"
#include <wx/dir.h>
#include <wx/wx.h>

RecursiveDelete::RecursiveDelete(bool aRemoveSaveGames) :
  mRemoveSaveGames(aRemoveSaveGames),
  mError(false)
{
}


wxDirTraverseResult RecursiveDelete::OnFile(const wxString& aFilename)
{
    bool lRemoveFile( true );

    // See if it is a save game file
    if (mRemoveSaveGames == false)
    {
      wxString lFilename = aFilename.Mid( aFilename.Find( '/', true ) + 1 );
      if (lFilename.Mid(0, 4).MakeUpper() == _T("SAVE")
	  && lFilename.Mid(lFilename.Len() - 3).MakeUpper() == _T("DAT"))
        {
	  lRemoveFile = false;
        }
    }
    
    // Remove the file
    if (lRemoveFile == true)
      {
        if (::wxRemoveFile(aFilename) == false)
        {
	  wxLogError(_("Could not remove %s"), aFilename.c_str());
	  mError = true;
        }
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult RecursiveDelete::OnDir( const wxString& aDirname )
{
  RecursiveDelete lRecursiveDelete( false );
  wxDir* lDir = new wxDir( aDirname );
  lDir->Traverse( lRecursiveDelete );
  delete lDir;
  
  if ( lRecursiveDelete.getError() == false )
    {
      if ( ::wxRmdir( aDirname ) == false )
	{
	  wxLogError(_("Could not remove %s"), aDirname.c_str());
	  mError = true;
	}
    }
  else if ( mError == false )
    {
      mError = true;
    }
  
  return wxDIR_IGNORE;
}

bool RecursiveDelete::getError( void )
{
  return mError;
}
