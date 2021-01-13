/**
 * D-Mod packaging wizard

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

#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>

#include "Package.hpp"

#include "BZip.hpp"
#include "Tar.hpp"
#include "IOUtils.hpp"

BEGIN_EVENT_TABLE(Package, wxDialog)
EVT_BUTTON(wxID_OK, Package::onOkay)
END_EVENT_TABLE()

Package::Package(const DMod aDMod) :
Package_Base(NULL, wxID_ANY, _T("")),
  mDMod(aDMod)
{
  prepareDialog();
  return;
}

void Package::prepareDialog()
{
  wxString introText = wxString::Format(
    lIntroText->GetLabel().c_str(),
    mDMod.GetTitle().c_str(), mDMod.GetFullPath().c_str());
  lIntroText->SetLabel(introText);

  mIdentifier->SetValue(mDMod.GetBaseName());
  mIdentifier->SetValidator(wxTextValidator(wxFILTER_ALPHANUMERIC));
  mIdentifier->SetMaxLength(8);

  this->SetMinSize(this->GetBestSize());
  this->SetSize(this->GetMinSize());


  // Dmod.diz
  // TODO: this was disabled in DFArc2 - can it be reused somehow?
  /*
    wxStaticText* lDmodDizTitleText = new wxStaticText( lPanel, -1, "Dmod.diz title" );
    mDmodDizTitle = new wxTextCtrl( lPanel, -1, lTitle );
    wxBoxSizer* lDmodDizTitleBox = new wxBoxSizer( wxHORIZONTAL );
    lDmodDizTitleBox->Add( lDmodDizTitleText, lLeftFlags );
    lDmodDizTitleBox->Add( mDmodDizTitle, lRightFlags );

    wxStaticText* lDmodDizInfo1Text = new wxStaticText( lPanel, -1, "Dmod.diz author/copyright information 1" );
    mDmodDizInfo1 = new wxTextCtrl( lPanel, -1, lInfo1 );
    wxBoxSizer* lDmodDizInfo1Box = new wxBoxSizer( wxHORIZONTAL );
    lDmodDizInfo1Box->Add( lDmodDizInfo1Text, lLeftFlags );
    lDmodDizInfo1Box->Add( mDmodDizInfo1, lRightFlags );

    wxStaticText* lDmodDizInfo2Text = new wxStaticText( lPanel, -1, "Dmod.diz author/copyright information 2" );
    mDmodDizInfo2 = new wxTextCtrl( lPanel, -1, lInfo2 );
    wxBoxSizer* lDmodDizInfo2Box = new wxBoxSizer( wxHORIZONTAL );
    lDmodDizInfo2Box->Add( lDmodDizInfo2Text, lLeftFlags );
    lDmodDizInfo2Box->Add( mDmodDizInfo2, lRightFlags );

    wxStaticText* lDmodDizDescriptionText = new wxStaticText( lPanel, -1, "Dmod.diz description" );
    mDmodDizDescription = new wxTextCtrl( lPanel, -1, lDescription, wxDefaultPosition,
        wxSize( -1, 100 ), wxTE_MULTILINE | wxTE_LEFT | wxTE_BESTWRAP );
    wxBoxSizer* lDmodDizDescriptionBox = new wxBoxSizer( wxHORIZONTAL );
    lDmodDizDescriptionBox->Add( lDmodDizDescriptionText, lLeftFlags );
    lDmodDizDescriptionBox->Add( mDmodDizDescription, lRightFlags );

    wxStaticBoxSizer* lDmodDizBox = new wxStaticBoxSizer( wxVERTICAL, lPanel, "Dmod.diz" );
    lDmodDizBox->Add( lDmodDizTitleBox, wxSizerFlags( 0 ).Left().Expand() );
    lDmodDizBox->Add( lDmodDizInfo1Box, wxSizerFlags( 0 ).Left().Expand() );
    lDmodDizBox->Add( lDmodDizInfo2Box, wxSizerFlags( 0 ).Left().Expand() );
    lDmodDizBox->Add( lDmodDizDescriptionBox, wxSizerFlags( 0 ).Left().Expand() );*/

    // Cleanup
    /*mCleanupSaveDat = new wxCheckBox( lPanel, -1, "Include save game files?" );
    mCleanupThumbsDb = new wxCheckBox( lPanel, -1, "Include thumbs.db files?" );
    wxStaticBoxSizer* lCleanupBox = new wxStaticBoxSizer( wxHORIZONTAL, lPanel, "Cleanup" );
    lCleanupBox->Add( mCleanupSaveDat, lLeftFlags );
    lCleanupBox->Add( mCleanupThumbsDb, lLeftFlags );*/

    // Patch
    /*mPatch = new wxCheckBox( lPanel, -1, "Create patch?" );

    wxStaticText* lPatchOriginalFileText = new wxStaticText( lPanel, -1, "Original .dmod file" );
    mPatchOriginalFile = new wxTextCtrl( lPanel, -1 );
    mPatchBrowse = new wxButton( lPanel, Package::ID_BROWSE, "Browse" );
    wxBoxSizer* lPatchOriginalBox = new wxBoxSizer( wxHORIZONTAL );
    lPatchOriginalBox->Add( lPatchOriginalFileText, wxSizerFlags( 25 ).Left().Expand().Border( wxALL, 2 ) );
    lPatchOriginalBox->Add( mPatchOriginalFile, wxSizerFlags( 55 ).Left().Expand().Border( wxALL, 2 ) );
    lPatchOriginalBox->Add( mPatchBrowse, wxSizerFlags( 20 ).Right().Expand().Border( wxALL, 2 ) );

    wxStaticBoxSizer* lPatchBox = new wxStaticBoxSizer( wxVERTICAL, lPanel, "Patch" );
    lPatchBox->Add( mPatch, lLeftFlags );
    lPatchBox->Add( lPatchOriginalBox, wxSizerFlags( 0 ).Left().Expand() );*/
}

void Package::onOkay( wxCommandEvent& aEvent )
{
  if (mIdentifier->GetValue().Len() == 0)
    {
      wxLogError(_("You must provide an identifier filename."));
      return;
    }
  
  int flags = wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT;
  // Using elapsed time instead of remaining time, because the latter
  // is not much reliable (when BZ2 flushes its buffer, which can't be
  // predicted, the progress is stopped for a long time, and this
  // confuses the time/progress estimation)
  flags |= wxPD_ELAPSED_TIME;
  wxProgressDialog* lPackageProcess =
    new wxProgressDialog(_("Packaging"), _("The D-Mod is being packaged."),
			 100, this, flags);
  
  // Rename the old D-Mod
  wxString lDmodFilePath = IOUtils::GetParentDir(mDMod.GetFullPath()) + wxFileName::GetPathSeparator()
    + mIdentifier->GetValue() + _T(".dmod");
  wxString lOldDmodFilePath = lDmodFilePath;

  int lCount = 1;
  while (::wxFileExists(lOldDmodFilePath))
    {
      lOldDmodFilePath = IOUtils::GetParentDir(mDMod.GetFullPath()) + wxFileName::GetPathSeparator()
	+ wxString::Format(_T("%s%d.dmod"), mIdentifier->GetValue().c_str(), lCount);
      lCount++;
    }
  if (lOldDmodFilePath != lDmodFilePath)
    {
      ::wxRenameFile(lDmodFilePath, lOldDmodFilePath);
    }
  
  wxString dirToCompress = mDMod.GetFullPath();
  Tar lTar(lDmodFilePath, dirToCompress);
  double compression_ratio = 0;
  if (lTar.Create(mIdentifier->GetValue(), &compression_ratio, lPackageProcess))
    {
      // Success dialog
      wxMessageBox(wxString::Format(_("%s was successfully packaged (compression ratio %2.1f : 1)."),
				    mIdentifier->GetValue().c_str(), compression_ratio),
		   _("Success"), wxOK | wxICON_INFORMATION, this);
    }
  else
    {
      ::wxRemoveFile(lDmodFilePath);
      wxMessageBox(_("Packaging aborted - removing partial .dmod file."),
		   _("Abort"), wxICON_EXCLAMATION, this);
    }
  EndModal(wxID_OK);
}
