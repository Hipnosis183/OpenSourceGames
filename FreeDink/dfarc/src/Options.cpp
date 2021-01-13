/**
 * Options dialog

 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008, 2010, 2014  Sylvain Beucler

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

#include "Options.hpp"
#include "Config.hpp"
#include "ClientData.hpp"
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

BEGIN_EVENT_TABLE(Options, wxDialog)
EVT_BUTTON(wxID_OK, Options::onOkay)
EVT_BUTTON(ID_OVERRIDE_DINKREFDIR_BROWSE, Options::onBrowseOverrideDinkrefDir)
EVT_BUTTON(ID_DMODDIR_BROWSE, Options::onBrowseDModDir)
EVT_CHOICE(ID_LANGUAGE_LIST, Options::onSelectLanguageList)
END_EVENT_TABLE()

Options::Options(Config* aConfig) :
Options_Base(NULL, wxID_ANY, _T("")),
  mConfig(aConfig)
{
  prepareDialog();
  return;
}

void Options::prepareDialog()
{
  // Prepare likely choices for game and editor executables selection

  // Get a list of PATHs where executables are looked
  /* The following code is adapted from
     wxWidgets-2.8.8/src/common/filefn.cpp , not used directly because
     you can't access a wxPathList's list of paths (!), only query
     them, and the underlying list implementation changed from wxList
     to wxArrayString between 2.6 and 2.8 */
  wxArrayString paths_list;
#ifndef __WXWINCE__
  static const wxChar PATH_TOKS[] =
#if defined(__WINDOWS__) || defined(__OS2__)
    wxT(";"); // Don't separate with colon in DOS (used for drive)
#else
  wxT(":;");
#endif
  wxString val;
  if ( wxGetEnv(wxT("PATH"), &val) )
    {
      // split into an array of string the value of the env var
      paths_list = wxStringTokenize(val, PATH_TOKS);
    }
#endif // !__WXWINCE__
#if defined(__WINDOWS__) || defined(__OS2__)
  if (paths_list.Index(wxT(".")) == wxNOT_FOUND)
    paths_list.Add(wxT("."));
#endif

  for (int i = 0; i < paths_list.GetCount(); i++)
    {
      if (!wxDir::Exists(paths_list.Item(i)))
	continue;
      wxDir dir(paths_list.Item(i));
      if (dir.IsOpened())
	{
	  wxString filename;
	  bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
	  while (cont)
	    {
	      if (filename.MakeLower().Matches(wxT("*dink*" EXEEXT)))
		  if (wxFileName::IsFileExecutable(paths_list.Item(i) + wxFileName::GetPathSeparator() + filename))
		    {
		      mDinkExeBox->Append(filename);
		      mEditorExeBox->Append(filename);
		    }
	      cont = dir.GetNext(&filename);
	    }
	}
    }

  /* Bare ISO-639-1 codes won't work with GNU/Linux */
  /*
  wxString iso_639_1 = wxT("aa ab ad ae af ak am an ar as av ay az ba be bg bh bi bm bn bo br bs ca ce ch co cr cs cu cv cy da de dv dz ee el en eo es et eu fa ff fi fj fo fr fy ga gd gl gn gu gv ha he hi ho hr ht hu hy hz ia id ie ig ii ik io is it iu ja jv ka kg ki kj kk kl km kn ko kr ks ku kv kw ky la lb lg li ln lo lt lu lv mg mh mi mk ml mn mo mr ms mt my na nb nd ne ng nl nn no nr nv ny oc oj om or os pa pi pl ps pt qu rm rn ro ru rw sa sc sd se sg si sk sl sm sn so sq sr ss st su sv sw ta te tg th ti tk tl tn to tr ts tt tw ty ug uk ur uz ve vi vo wa wo xh yi yo za zh zu");
  wxArrayString language_list = wxStringTokenize(iso_639_1, _T(" "));
  for (int i = 0; i < language_list.GetCount(); i++)
    {
      mForceLocaleBox->Append(language_list.Item(i));
    }
  */
  /* Let's suggest a set of supported locales instead - it can be
     overridden anyway*/
  /* TODO: generate from po/LINGUAS */
  int idx = 0;
  wxString supported_dfarc_languages = wxT("ca da de es eo fr fi hr hu it mk nl pl pt_BR sr tr vi");
  wxArrayString language_list = wxStringTokenize(supported_dfarc_languages, wxT(" "));
  mLanguageList->Append(_("System language"));
  mLanguageList->SetClientObject(idx, new ClientDataString(wxEmptyString));
  if (mConfig->mForceLocale == wxEmptyString)
    mLanguageList->Select(idx);
  idx++;
  mLanguageList->Append(_("Don't translate"));
  mLanguageList->SetClientObject(idx, new ClientDataString(wxT("C")));
  if (mConfig->mForceLocale == wxT("C"))
    mLanguageList->Select(idx);
  idx++;
  for (int i = 0; i < language_list.GetCount(); i++)
    {
      wxString label = language_list.Item(i);
      const wxLanguageInfo* li = wxLocale::FindLanguageInfo(language_list.Item(i));
      if (li != NULL)
	label += wxT(" - ") + wxString(wxGetTranslation(li->Description));
      mLanguageList->Append(label);
      wxString locale_name = li->CanonicalName;
      mLanguageList->SetClientObject(idx, new ClientDataString(locale_name));
      if (locale_name == mConfig->mForceLocale)
	mLanguageList->Select(idx);
      idx++;
    }
  mLanguageList->Append(_("Custom"));
  if (mLanguageList->GetSelection() == wxNOT_FOUND)
    mLanguageList->Select(idx);

  mWriteIniCheck           -> SetValue(mConfig->mWriteIniValue);
  mShowDeveloperCheck      -> SetValue(mConfig->mShowDeveloperButtons);
  mCloseCheck              -> SetValue(mConfig->mCloseDfarcOnPlay);
  mOverrideDinkrefDirCheck -> SetValue(mConfig->mOverrideDinkrefDir);
  mOverrideDinkrefDirBox   -> SetValue(mConfig->mSpecifiedDinkrefDir);
  mDModDirBox              -> SetValue(mConfig->mDModDir);
  mDinkExeBox              -> SetValue(mConfig->mDinkExe);
  mEditorExeBox            -> SetValue(mConfig->mEditorExe);
  mPreferredFileBrowserBox -> SetValue(mConfig->mPreferredFileBrowserExe);
  mForceLocaleBox          -> SetValue(mConfig->mForceLocale);

  this->SetMinSize(this->GetBestSize());
  this->SetSize(this->GetMinSize());
}

void Options::onOkay(wxCommandEvent& aEvent)
{
  mConfig->mWriteIniValue           = mWriteIniCheck->GetValue();
  mConfig->mShowDeveloperButtons    = mShowDeveloperCheck->GetValue();
  mConfig->mCloseDfarcOnPlay        = mCloseCheck->GetValue();
  mConfig->mOverrideDinkrefDir      = mOverrideDinkrefDirCheck->GetValue();
  mConfig->mSpecifiedDinkrefDir     = mOverrideDinkrefDirBox->GetValue();
  mConfig->mDModDir                 = mDModDirBox->GetValue();
  mConfig->mDinkExe                 = mDinkExeBox->GetValue();
  mConfig->mEditorExe               = mEditorExeBox->GetValue();
  mConfig->mPreferredFileBrowserExe = mPreferredFileBrowserBox->GetValue();
  mConfig->mForceLocale             = mForceLocaleBox->GetValue();

  mConfig->Update();
  EndModal(1);
  return;
}

void Options::onBrowseOverrideDinkrefDir(wxCommandEvent& aEvent)
{
  const wxString& dir = wxDirSelector(_("Choose the Dink Smallwood install directory"),
				      mOverrideDinkrefDirBox->GetValue());
  if (!dir.IsEmpty())
    mOverrideDinkrefDirBox->SetValue(dir);
}

void Options::onBrowseDModDir(wxCommandEvent& aEvent)
{
  const wxString& dir = wxDirSelector(_("Choose a folder containing D-Mods"),
				      mDModDirBox->GetValue());
  if (!dir.IsEmpty())
    mDModDirBox->SetValue(dir);
}

void Options::onSelectLanguageList(wxCommandEvent& aEvent)
{
  ClientDataString* cds = (ClientDataString*)aEvent.GetClientObject();
  if (cds != NULL)
    mForceLocaleBox->SetValue(((ClientDataString*)aEvent.GetClientObject())->s);
}
