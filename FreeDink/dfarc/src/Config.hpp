/**
 * Application configuration values

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

#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <wx/config.h> /* set appropriate wxConfig subclass */

class Config
{
public:
  static Config* GetConfig();
  Config();
  ~Config();
  void Update();
  wxString GetDinkrefDir();

  bool mTrueColorValue;
  bool mWindowedValue;
  bool mSoundValue;
  bool mJoystickValue;
  bool mDebugValue;
  bool mV107Value;

  bool mShowIntroductionText;
  bool mWarnOnEdit;
  wxString mSelectedDmod;
  
  bool mCloseDfarcOnPlay;
  bool mShowDeveloperButtons;
  bool mWriteIniValue;
  bool mOverrideDinkrefDir;
  wxString mSpecifiedDinkrefDir;
  wxString mDModDir;
  wxString mDinkExe;
  wxString mEditorExe;
  wxString mPreferredFileBrowserExe;
  wxString mForceLocale;

private:
  wxConfig* mwxConfig;
};

#endif
