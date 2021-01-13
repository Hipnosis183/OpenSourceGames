/**
 * DFArc main frame

 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008, 2010  Sylvain Beucler

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

#ifndef _DFARCFRAME_HPP
#define _DFARCFRAME_HPP

#include "DFArcFrame_Base.h"
#include "DMod.hpp"
#include "ClientData.hpp"

#include <vector>
#include <wx/bitmap.h>

class Config;

// Main Frame Class, no arguments.
class DFArcFrame : public DFArcFrame_Base
{
public:
  DFArcFrame();
  
  void onShow(wxShowEvent& aEvent);
  void OnQuit(wxCommandEvent &Event);

  void Install(wxCommandEvent &Event);
  void onDownload(wxCommandEvent& aEvent);
  
  void onRefresh(wxCommandEvent& aEvent);
  void onBrowse(wxCommandEvent& aEvent);
  void uninstall(wxCommandEvent& aEvent);
  void showOptions(wxCommandEvent& aEvent);
  
  void showIntroductionText(wxCommandEvent& aEvent);
  void onWalkthroughs( wxCommandEvent& aEvent);
  void onForums(wxCommandEvent& aEvent);
  void OnAbout(wxCommandEvent &Event);

  void OnEvtListBox(wxCommandEvent &Event);

  void OnSetPlayOption(wxCommandEvent &Event);

  void OnPlay(wxCommandEvent& aEvent);
  void onEdit(wxCommandEvent& aEvent);
  void onPackage(wxCommandEvent &Event);

  wxBitmap mAllLogos;
  
private:
  void PrepareGUI();
  void showDeveloperButtons(bool visible);
  void initializeConfig();
  void refreshDmodList();
  void refreshDmodLogos();
  void RestoreListBoxFromConfig();
  void SelectDModFromListBox();

  void updateConfig();
  void updateDescription();
  void populateAvailableDModsList();

  bool launchURL(wxString url);

  enum program { GAME, EDITOR };
  wxString BuildCommand(enum program);

  Config *mConfig;
  std::vector<DMod> mAvailableDModsList;

  bool mNoDmods;
  int mSelectedDModIndex;
  wxBitmap mDefaultLogoBitmap;

  DECLARE_EVENT_TABLE()
};

#endif
