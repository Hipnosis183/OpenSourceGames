/**
 * D-Mod installation wizard

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

#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <wx/progdlg.h>

#include "InstallVerifyFrame.hpp"
#include "Config.hpp"
#include "BZip.hpp"
#include "Tar.hpp"

BEGIN_EVENT_TABLE(InstallVerifyFrame, wxDialog)
EVT_BUTTON(wxID_OK, InstallVerifyFrame::onInstall)
EVT_BUTTON(wxID_CANCEL, InstallVerifyFrame::onCancel)
END_EVENT_TABLE()

InstallVerifyFrame::InstallVerifyFrame(const wxString& lDmodFilePath)
: InstallVerifyFrame_Base(NULL, wxID_ANY, _T(""))
{
  mConfig = Config::GetConfig();

  prepareDialog();
  
  int flags = wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_REMAINING_TIME;
  wxProgressDialog lPrepareProgress(_("Preparing"),
    _("The D-Mod archive is being decompressed in a temporary file."), 100, this, flags);
  
  // Extract
  BZip lBZip(lDmodFilePath);
  mTarFilePath = lBZip.Extract(&lPrepareProgress);
  
  if (mTarFilePath.Len() != 0)
    {
      // Prepare the tar file for reading
      Tar lTar(mTarFilePath);
      if (lTar.ReadHeaders() == 1) {
        this->EndModal(wxID_CANCEL);
        return;
      }
     
      // Get and display the dmod description
      wxString lDmodDescription = lTar.getmDmodDescription();
      if ( lDmodDescription.Len() == 0 )
        {
	  lDmodDescription = wxString::Format(_("No Description Available.\n"
						"\n"
						"The D-Mod will be installed in subdirectory '%s'."),
					      lTar.getInstalledDmodDirectory().c_str());
        }
      else
        {
	  int lBreakChar = lDmodDescription.Find( '\r' );
	  if ( lBreakChar <= 0 )
            {
	      lBreakChar = lDmodDescription.Find( '\n' );
            }
	  mDmodName = lDmodDescription.SubString( 0, lBreakChar - 1 );
	  this->SetTitle(_("DFArc - Install D-Mod - ") + mDmodName);
        }
      mDmodDescription->SetValue(lDmodDescription);
      
      // Re-enable the install button
      mInstallButton->Enable(true);
    }
}

InstallVerifyFrame::~InstallVerifyFrame()
{
  if (mTarFilePath.Len() != 0)
    ::wxRemoveFile(mTarFilePath);
}

void InstallVerifyFrame::prepareDialog(void)
{
  if (mConfig->mDModDir.IsEmpty())
    // force install in Dink dir
    mDestdirBox->Disable();
  else
    // default install in D-Mods dir
    mDestdirBox->SetSelection(1);

  // Disable installation button until the tar is properly decompressed
  mInstallButton->Enable(false);

  this->SetMinSize(this->GetBestSize());
  this->SetSize(this->GetMinSize());
}

void InstallVerifyFrame::onCancel(wxCommandEvent &Event)
{
  this->EndModal(wxID_CANCEL);
}

void InstallVerifyFrame::onInstall(wxCommandEvent &Event)
{
  int flags = wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_REMAINING_TIME;
  wxProgressDialog lInstallProgress(_("DFArc - Installing"), _T(""),
    100, this, flags);

  // Install in the main dink dir, or in the specified DMod dir if any
  wxString destdir = mConfig->GetDinkrefDir();
  if (mDestdirBox->GetSelection() == 1)
    destdir = mConfig->mDModDir;

  Tar lTar(mTarFilePath);
  if (lTar.ReadHeaders() == 1) {
    this->EndModal(wxID_CANCEL);
    return;
  }

  if (wxDirExists(destdir + wxFileName::GetPathSeparator() + lTar.getInstalledDmodDirectory())) {
    wxString question;
    question.Printf(_("Directory '%s' already exists. Continue?"), lTar.getInstalledDmodDirectory());
    int lResult = wxMessageBox(question, _("DFArc - Installing"),
			       wxYES_NO | wxICON_WARNING, this);
    if (lResult == wxNO)
      return;
  }

  int lError = lTar.Extract(destdir, &lInstallProgress);
  if (lError == 0)
    {
      if (mDmodName.Len() <= 0)
        {
	  mDmodName = _("The D-Mod you selected");
        }
      ::wxMessageBox(mDmodName + _(" was successfully installed."),
		   _("Success"), wxOK | wxICON_INFORMATION, this);
      // Preselect this new D-Mod
      wxString dmod_dir = lTar.getInstalledDmodDirectory();
      if (dmod_dir.Last() == _T('/'))
	dmod_dir = dmod_dir.RemoveLast();
      mConfig->mSelectedDmod = destdir + wxFileName::GetPathSeparator() + dmod_dir;
      this->EndModal(wxID_OK);
    }
  else
    {
      wxLogError(_("An error occurred while extracting the .dmod file."));
    }
}
