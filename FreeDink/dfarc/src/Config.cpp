/**
 * Application configuration values

 * Copyright (C) 2008, 2009, 2014  Sylvain Beucler

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

#include "Config.hpp"
#include "IOUtils.hpp"
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/intl.h>
#include <wx/log.h>

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* registry */
#include <wx/textfile.h>
#endif

/**
 * Singleton getter
 */
static Config* theConfig = NULL;

Config* Config::GetConfig()
{
  if (theConfig == NULL)
    theConfig = new Config();
  return theConfig;
}

Config::Config():
  // Main frame
  mTrueColorValue(false),
  mWindowedValue(false),
  mSoundValue(true),
  mJoystickValue(true),
  mDebugValue(false),
  mV107Value(false),
  
  mShowIntroductionText(true),
  mWarnOnEdit(true),

  // Options
  mShowDeveloperButtons(true),
  mCloseDfarcOnPlay(false),
  mWriteIniValue(true),
  mOverrideDinkrefDir(false),
  mSpecifiedDinkrefDir(wxEmptyString),
  mDModDir(wxEmptyString),
  mForceLocale(wxEmptyString)
{
  /* Default value for DModDir */
  mDModDir = ::wxGetHomeDir() + wxFileName::GetPathSeparator() + wxT("dmods");
  mDinkExe = wxT("dink") wxT(EXEEXT);
  mEditorExe = wxT("dinkedit") wxT(EXEEXT);
  mSelectedDmod = GetDinkrefDir() + wxFileName::GetPathSeparator() + wxT("dink");

  mwxConfig = new wxConfig(_T("dfarc3"));
  mwxConfig->Read(_T("True Color"), &mTrueColorValue);
  mwxConfig->Read(_T("Windowed"), &mWindowedValue);
  mwxConfig->Read(_T("Sound"), &mSoundValue);
  mwxConfig->Read(_T("Joystick"), &mJoystickValue);
  mwxConfig->Read(_T("Debug"), &mDebugValue);
  mwxConfig->Read(_T("Windowed"), &mWindowedValue);
  mwxConfig->Read(_T("v1.07"), &mV107Value);
  mwxConfig->Read(_T("Last Selected D-Mod"), &mSelectedDmod);
  mwxConfig->Read(_T("Show Introduction"), &mShowIntroductionText);
  mwxConfig->Read(_T("Warn on Edit"), &mWarnOnEdit);
  
  // Options dialog
  mwxConfig->Read(_T("Close DFArc on Play"), &mCloseDfarcOnPlay);
  mwxConfig->Read(_T("Show Developer Buttons"), &mShowDeveloperButtons);
  mwxConfig->Read(_T("Write Ini"), &mWriteIniValue);
  mwxConfig->Read(_T("Override Dink Directory"), &mOverrideDinkrefDir);
  mwxConfig->Read(_T("Specified Dink Directory"), &mSpecifiedDinkrefDir);
  mwxConfig->Read(_T("Additional D-Mods Directory"), &mDModDir);
  mwxConfig->Read(_T("Dink Executable"), &mDinkExe);
  mwxConfig->Read(_T("Editor Executable"), &mEditorExe);
  mwxConfig->Read(_T("Language"), &mForceLocale);
}

Config::~Config()
{
  delete mwxConfig; // save changes on disk
}

void Config::Update()
{
  // Update config
  mwxConfig->Write(_T("Windowed"), mWindowedValue);
  mwxConfig->Write(_T("True Color"), mTrueColorValue);
  mwxConfig->Write(_T("Sound"), mSoundValue);
  mwxConfig->Write(_T("Joystick"), mJoystickValue);
  mwxConfig->Write(_T("Debug"), mDebugValue);
  mwxConfig->Write(_T("v1.07"), mV107Value);
  mwxConfig->Write(_T("Show Introduction"), mShowIntroductionText);
  mwxConfig->Write(_T("Warn on Edit"), mWarnOnEdit);
  
  // Options dialog
  mwxConfig->Write(_T("Write Ini"), mWriteIniValue);
  mwxConfig->Write(_T("Show Developer Buttons"), mShowDeveloperButtons);
  mwxConfig->Write(_T("Close DFArc on Play"), mCloseDfarcOnPlay);
  mwxConfig->Write(_T("Override Dink Directory"), mOverrideDinkrefDir);
  mwxConfig->Write(_T("Specified Dink Directory"), mSpecifiedDinkrefDir);
  mwxConfig->Write(_T("Additional D-Mods Directory"), mDModDir);
  mwxConfig->Write(_T("Dink Executable"), mDinkExe);
  mwxConfig->Write(_T("Editor Executable"), mEditorExe);
  mwxConfig->Write(_T("Language"), mForceLocale);
  
  mwxConfig->Write(_T("Last Selected D-Mod"), mSelectedDmod);
  return;
}



/*-------------------*/
/*- GetDinkrefDir() -*/
/*-------------------*/

static wxString FindDefaultDinkrefDir()
{
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
  const wxString fallback_woe_dinkdir = _T("C:/Program Files/FreeDink");

  /* Just check in the same directory */
  WCHAR exe_dir[MAX_PATH];
  if (GetModuleFileNameW(NULL, exe_dir, sizeof(exe_dir)) < 0)
    return fallback_woe_dinkdir;
  wxString str(exe_dir);
  return IOUtils::GetParentDir(str);

  /* The rest is too complex and error-prone when there's more than
     one Dink version installed, let's just skip */
if (false) {
  //
  // First see if it's in the registry.
  //
  unsigned long int iszPathSize = 512;
  unsigned long int iRegType = REG_SZ;
  unsigned long int liError;
  unsigned char *szPath = new unsigned char[512];
  HKEY hDS;
  
  memset(szPath, 0, 512);
  
  // Open the registry path.
  liError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\RTSOFT\\Dink Smallwood"),
			 0, KEY_READ, &hDS);
  
  // Grab the path.
  while(RegQueryValueEx(hDS, _T("Path"), 0, &iRegType, szPath,
			&iszPathSize) == ERROR_MORE_DATA)
    {
      // If the path is too small, make the buffer bigger.
      iszPathSize += 512;
      delete[] szPath;
      szPath = new unsigned char[iszPathSize];
      memset(szPath, 0, iszPathSize);
    }
  
  // Close the registry.
  RegCloseKey(hDS);
  
  if (liError == ERROR_SUCCESS && szPath[0] != '\0')
    {        
      wxString str = szPath;
      delete[] szPath;
      return str;
    }

  //
  // We don't have the registry key.  See if we can use the deprecated dinksmallwood.ini file.
  //
  wxString strSysRoot;
  wxString strBuf;

  //
  // Open dinksmallwood.ini
  //
  strSysRoot = wxGetOSDirectory() + _T("/dinksmallwood.ini");
  if (!::wxFileExists(strSysRoot))
    {
      wxLogVerbose(_("Error: dinksmallwood.ini not found. "
		       "Please run the main game and try running this program again."));
      return fallback_woe_dinkdir;
    }
  
  wxTextFile f_In;
  if (!f_In.Open(strSysRoot))
    {
      wxLogVerbose(_("Error opening dinksmallwood.ini"), strSysRoot);
      return fallback_woe_dinkdir;
    }
  
  // Parse
  return f_In.GetLine(1);
}

# else /* !WIN32 */

  // Return standard location
  wxString testdir;

  // pkgdatadir
  testdir = wxT(DEFAULT_DATA_DIR) wxT("/dink/dink/graphics");
  IOUtils::ciconvert(testdir);
  if (wxDir::Exists(testdir))
    return wxT(DEFAULT_DATA_DIR) wxT("/dink");

  // /usr/local/share/games
  testdir = wxT("/usr/local/share/games/dink/dink/graphics");
  IOUtils::ciconvert(testdir);
  if (wxDir::Exists(testdir))
    return wxT("/usr/local/share/games/dink");

  // /usr/local/share
  testdir = wxT("/usr/local/share/dink/dink/graphics");
  IOUtils::ciconvert(testdir);
  if (wxDir::Exists(testdir))
    return wxT("/usr/local/share/dink");

  // /usr/share/games
  testdir = wxT("/usr/local/share/dink/dink/graphics");
  IOUtils::ciconvert(testdir);
  if (wxDir::Exists(testdir))
    return wxT("/usr/local/share/dink");

  // /usr/share
  return wxT("/usr/share/dink");

#endif
}

wxString Config::GetDinkrefDir()
{
  if (mOverrideDinkrefDir == true)
    return mSpecifiedDinkrefDir;
  return FindDefaultDinkrefDir();
}
