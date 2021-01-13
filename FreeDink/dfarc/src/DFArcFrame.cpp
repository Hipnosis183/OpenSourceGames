/**
 * DFArc main frame

 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008, 2010, 2014, 2017  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "DFArcFrame.hpp"

#include <stdio.h>

#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/image.h>

#include <wx/msgdlg.h>
#include <wx/filedlg.h>

#include <wx/listbox.h>
#include <wx/clntdata.h>

#include <wx/process.h>

#include "IOUtils.hpp"
#include "InstallVerifyFrame.hpp"
#include "Options.hpp"

#include "Config.hpp"
#include "RecursiveDelete.hpp"
#include "Package.hpp"
#include "DMod.hpp"
#include "icon_xpm.hpp"

#define LOGO_WIDTH 160
#define LOGO_HEIGHT 120


class MonitorDinkExit : public wxProcess
{
public:
  MonitorDinkExit(Config *mConfig) : wxProcess(), mConfig(mConfig) {}
  virtual void OnTerminate(int pid, int status);
private:
  Config *mConfig;
};

void MonitorDinkExit::OnTerminate(int pid, int status)
{
  if (status == -1)
    ::wxMessageBox(wxString::Format(_("Dink Smallwood ('%s') was not found on your computer."
				      " Please configure the Dink program name in the Options menu."),
				    mConfig->mDinkExe.c_str(), wxICON_ERROR, this),
		   _("Error"));
  else if (status != 0)
    ::wxMessageBox(wxString::Format(_("Dink Smallwood failed! Error code %d."),
  				    status, wxICON_EXCLAMATION, this),
  		   _("Error"));
}


class MonitorEditorExit : public wxProcess
{
public:
  MonitorEditorExit(Config *mConfig) : wxProcess(), mConfig(mConfig) {}
  virtual void OnTerminate(int pid, int status);
private:
  Config *mConfig;
};

void MonitorEditorExit::OnTerminate(int pid, int status)
{
  if (status == -1)
    ::wxMessageBox(wxString::Format(_("The editor ('%s') was not found on your computer."
				      " Please configure the editor program name in the Options menu."),
				    mConfig->mEditorExe.c_str()),
		   _("Error"));
  else if (status != 0)
    ::wxMessageBox(_("Error while running the editor"), _("Error"));
}


/**
 * Custom paint method to display the animation efficiently.
 * Cf. doc/animation.txt for details.
 */
#define TIMER_ID 1  // arbitrary, maybe there's a better way
#define FPS 50
class DFAnimationPanel : public wxPanel
{
private:
  double pos;
  double dpos;
  DFArcFrame* frame;
public:
  DFAnimationPanel(DFArcFrame* frame, wxWindow* parent) :
    wxPanel(parent, wxID_ANY),
    frame(frame),
    pos(.0), dpos(.0)
  { }

  void setIndex(int idx)
  {
    dpos = (idx+1) * LOGO_HEIGHT;
  }

  void OnPaint(wxPaintEvent &)
  {
    /* Note: this->IsDoubleBuffered() == 1 under Gtk.  Under woe it
       reports 0, but when I clear the surface I don't any flickering,
       so it must be double-buffered somewhere too. */
    wxPaintDC dst(this);
    // Clear surface - unneeded actually
    //dst.SetBrush(*wxWHITE_BRUSH);
    //dst.DrawRectangle(0, 0, LOGO_WIDTH, LOGO_HEIGHT);
    // Draw logo
    wxMemoryDC src(frame->mAllLogos);
    dst.Blit(0, 0, LOGO_WIDTH, LOGO_HEIGHT, &src, 0, rint(pos));
  }

  /**
   * Update the animation parameters regularly.
   */
  void OnIdle(wxIdleEvent &)
  {
    if (fabs(pos - dpos) < .5)
      // destination reached, taking rounding into account.
      return;

    // Simple easing
    pos += (dpos - pos) / 7;

    Refresh(/*eraseBackground=*/false);

    wxMilliSleep(1000.0/FPS); // ms
  }
  DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(DFAnimationPanel, wxPanel)
  EVT_PAINT(DFAnimationPanel::OnPaint)
  EVT_IDLE(DFAnimationPanel::OnIdle)
END_EVENT_TABLE()


// FRAME EVENT TABLE
BEGIN_EVENT_TABLE(DFArcFrame, wxFrame)
EVT_SHOW(DFArcFrame::onShow)

EVT_MENU(ID_FileInstall, DFArcFrame::Install)
EVT_MENU(ID_Download, DFArcFrame::onDownload)
EVT_MENU(wxID_EXIT, DFArcFrame::OnQuit)

EVT_MENU(ID_Refresh, DFArcFrame::onRefresh)
EVT_MENU(ID_Browse, DFArcFrame::onBrowse)
EVT_MENU(ID_Uninstall, DFArcFrame::uninstall)
EVT_MENU(ID_Options, DFArcFrame::showOptions)

EVT_MENU(ID_IntroductionText, DFArcFrame::showIntroductionText)
EVT_MENU(ID_Walkthroughs, DFArcFrame::onWalkthroughs)
EVT_MENU(ID_Forums, DFArcFrame::onForums)
EVT_MENU(wxID_ABOUT, DFArcFrame::OnAbout)

EVT_BUTTON(ID_Play, DFArcFrame::OnPlay)
EVT_BUTTON(ID_Edit, DFArcFrame::onEdit)
EVT_BUTTON(ID_Package, DFArcFrame::onPackage)

EVT_LISTBOX(ID_DmodTitleList, DFArcFrame::OnEvtListBox)

EVT_CHECKBOX(ID_Truecolor, DFArcFrame::OnSetPlayOption)
EVT_CHECKBOX(ID_Windowed, DFArcFrame::OnSetPlayOption)
EVT_CHECKBOX(ID_Sound, DFArcFrame::OnSetPlayOption)
EVT_CHECKBOX(ID_Joystick, DFArcFrame::OnSetPlayOption)
EVT_CHECKBOX(ID_Debug, DFArcFrame::OnSetPlayOption)
EVT_CHECKBOX(ID_V107, DFArcFrame::OnSetPlayOption)
END_EVENT_TABLE()

DFArcFrame::DFArcFrame() :
DFArcFrame_Base(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
  mNoDmods(false),
  mSelectedDModIndex(wxNOT_FOUND)
{
  mConfig = Config::GetConfig();

  PrepareGUI();
  refreshDmodList();
  mConfig->Update();
  return;
}

void DFArcFrame::OnQuit(wxCommandEvent& aCommandEvent)
{
    this->Close();
}

void DFArcFrame::OnAbout(wxCommandEvent& aCommandEvent)
{
  // Use a wxString for concatenation with the date.
  wxString version = wxT(PACKAGE_VERSION);
  wxString wxs = wxString::Format(_(
    "DFArc version %s\n"
    "Copyright (C) 2004  Andrew Reading (merlin)\n"
    "Copyright (C) 2005, 2006  Dan Walma (redink1)\n"
    "Copyright (C) 2008-%04d  Sylvain Beucler (Beuc)\n"
    "Powered by bzip2 (bzip.org) and wxWidgets (wxwidgets.org)"),
				  version.c_str(), 2018);
  wxMessageBox(wxs, _("About DFArc v3"), wxOK | wxICON_INFORMATION, this);
}

// Display or hide "Edit" and "Package" buttons
void DFArcFrame::showDeveloperButtons(bool visible)
{
  wxSizer *mButtonSizer = mEditButton->GetContainingSizer();
  wxSizer *mImageSizer = mAnimationPanel->GetContainingSizer();
  mImageSizer->Show(mButtonSizer, visible);
  // Re-layout, starting with expanding mDModListBox
  mDModListBox->GetContainingSizer()->Layout();
}

static wxBitmap CreateTextLogo(wxString text)
{
  wxBitmap lLogo = wxBitmap(LOGO_WIDTH, LOGO_HEIGHT);
  wxMemoryDC dc;
  dc.SelectObject(lLogo);
  dc.SetBackground(*wxBLACK_BRUSH);
  dc.SetTextForeground(*wxWHITE);
  dc.Clear();
  // Center text
  // (There seem to be a bug in GetTextExtent in wx2.8.10, the
  // calculated width is smaller than the drawned width)
  wxSize size = dc.GetTextExtent(text);
  dc.DrawText(text,
	      (LOGO_WIDTH - size.GetWidth()) / 2,
	      (LOGO_HEIGHT - size.GetHeight()) / 2);
  dc.SelectObject(wxNullBitmap);
  return lLogo;
}

// GUI for main screen (with no args).
void DFArcFrame::PrepareGUI()
{   
  // Replace wxGlade's plain wxPanel with our custom class -
  // cf. DFArcFrame_Base.cpp
  wxSizer *sizer = mAnimationPanel->GetContainingSizer();
  sizer->Detach(mAnimationPanel);
  delete mAnimationPanel;
  mAnimationPanel = new DFAnimationPanel(this, panel_1);
  mAnimationPanel->SetMinSize(wxSize(160, 120));
  sizer->Insert(1, mAnimationPanel, 0, wxLEFT|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 10);

  // Features not supported by wxGlade:
  SetIcon(wxIcon(dink_xpm));
  mSplitter->SetSashGravity(0.5); // proportional outer resize
  // Avoid 'unsplit' (where one of the pane is hidden)
  mSplitter->SetMinimumPaneSize(20);
  // Manual contraints
  mDModListBox->SetMinSize(wxSize(150, 165));
  mDmodDescription->SetMinSize(wxSize(-1, 220));
  this->SetMinSize(this->GetBestSize());
  mSplitter->SetSashPosition(0); // reset to middle pos (wx3 bug?)
  this->SetSize(this->GetMinSize());
  // user can override size restrictions:
  mDmodDescription->SetMinSize(wxSize(-1, -1));

  // Optionally show developper buttons
  showDeveloperButtons(mConfig->mShowDeveloperButtons);
  // Update the checkboxes
  mTrueColor->SetValue(mConfig->mTrueColorValue);
  mWindowed->SetValue(mConfig->mWindowedValue);
  mSound->SetValue(mConfig->mSoundValue);
  mJoystick->SetValue(mConfig->mJoystickValue);
  mDebug->SetValue(mConfig->mDebugValue);
  mV107->SetValue(mConfig->mV107Value);

  // Default logo (currently all black with a question mark)
  /* TRANSLATORS: please make this SHORT, possibly rephrasing as "<
     Choose!". This is included in the 160x120px logo box in the main
     window and it doesn't word-wrap. */
  mDefaultLogoBitmap = CreateTextLogo(_("< Pick a D-Mod"));
  ((DFAnimationPanel*)mAnimationPanel)->setIndex(-1);
}

void DFArcFrame::updateDescription()
{
  wxString lDescription;

  // Set the current D-Mod directory to the currently selected item's client string
  DMod cur_dmod = mAvailableDModsList.at(mSelectedDModIndex);
  mConfig->mSelectedDmod = cur_dmod.GetFullPath();
  mConfig->Update();
  mDmodDescription->SetValue(cur_dmod.GetDescription());
  mStatusBar->SetStatusText(mConfig->mSelectedDmod);
}

void DFArcFrame::SelectDModFromListBox()
{
  int lb_index = mDModListBox->GetSelection();
  if (lb_index != wxNOT_FOUND)
    {
      Integer *I = (Integer*)mDModListBox->GetClientObject(lb_index);
      mSelectedDModIndex = I->i;
      mConfig->mSelectedDmod = mAvailableDModsList.at(mSelectedDModIndex).GetFullPath();
      mConfig->Update();
      updateDescription();
      mPlayButton->Enable();
      mEditButton->Enable();
      mPackageButton->Enable();

      // Fill-in translations list
      wxString cur_locale_name = mConfig->mForceLocale;
      if (mConfig->mForceLocale == wxEmptyString) {
	const wxLanguageInfo* li = wxLocale::GetLanguageInfo(wxLocale::GetSystemLanguage());
	if (li != NULL)
	  cur_locale_name = li->CanonicalName;
      }

      mGameLocaleList->Clear();
      wxArrayString mo_files;
      wxString mo_dir = mConfig->mSelectedDmod + wxFileName::GetPathSeparator() + wxT("l10n");
      if (wxDir::Exists(mo_dir)) {
	wxDir::GetAllFiles(mo_dir, &mo_files, wxT("*.mo"));
	mo_files.Sort();
	for (int i = 0; i < mo_files.Count(); i++)
	  {
	    // dmod/l10n/fr/LC_MESSAGES/dmod.mo
	    wxFileName mo(mo_files.Item(i));
	    mo.RemoveLastDir();
	    wxArrayString dirs = mo.GetDirs();
	    wxString dir = dirs.Item(dirs.Count()-1);
	    wxString label = dir;
	    wxString locale_name = dir;
	    const wxLanguageInfo* li = wxLocale::FindLanguageInfo(locale_name);
	    if (li != NULL) {
	      label += wxT(" - ") + wxString(wxGetTranslation(li->Description));
	      locale_name = li->CanonicalName;
	    }
	    mGameLocaleList->Append(label);
	    mGameLocaleList->SetClientObject(i, new ClientDataString(locale_name));
	    if (cur_locale_name == locale_name)
	      mGameLocaleList->Select(i);
	  }
      }
      if (mGameLocaleList->GetCount() == 0) {
	mGameLocaleList->Append(_("No translations"));
	mGameLocaleList->Select(0);
	mGameLocaleList->Disable();
      } else {
	// Not displaying the default language explicitely, because it
	// makes the user think that a translation is always available.
	// mGameLocaleList->Insert(wxString(_("Default language")) + wxT(" (") + cur_locale_name + wxT(")"), 0);
	mGameLocaleList->Insert(_("Don't translate"), 0);
	mGameLocaleList->SetClientObject(0, new ClientDataString(wxT("C")));
	mGameLocaleList->Enable();
	if (mGameLocaleList->GetSelection() == wxNOT_FOUND)
	  mGameLocaleList->Select(0);
      }
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
      // work-around wx3 bug -> default height=6 despite BestSize=21
      mGameLocaleList->SetSize(wxSize(-1, mGameLocaleList->GetBestSize().y+1));
#endif
    }
  ((DFAnimationPanel*)mAnimationPanel)->setIndex(lb_index);
}

void DFArcFrame::OnEvtListBox(wxCommandEvent &Event)
{
  SelectDModFromListBox();
  return;
}

// Function for Install menu entry
void DFArcFrame::Install(wxCommandEvent& aCommandEvent)
{
  wxString description = _("D-Mod files (*.dmod)");
  wxFileDialog FileDlg(0, _("Select a .dmod file"), _T(""), _T(""), description + _T("|*.dmod"),
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
     
  if (FileDlg.ShowModal() == wxID_OK)
    {
      InstallVerifyFrame lTemp(FileDlg.GetPath());
      lTemp.ShowModal();
      // Update the D-Mod list
      refreshDmodList();
    }
}

void DFArcFrame::onPackage(wxCommandEvent &Event)
{
  if (mSelectedDModIndex != wxNOT_FOUND) {
    DMod cur_dmod = mAvailableDModsList.at(mSelectedDModIndex);
    Package* lPackage = new Package(cur_dmod);
    lPackage->ShowModal();
    lPackage->Destroy();
  }
}

void DFArcFrame::OnSetPlayOption(wxCommandEvent &Event)
{
  // Set variables
  mConfig->mTrueColorValue = mTrueColor->IsChecked();
  mConfig->mWindowedValue = mWindowed->IsChecked();
  mConfig->mSoundValue = mSound->IsChecked();
  mConfig->mJoystickValue = mJoystick->IsChecked();
  mConfig->mDebugValue = mDebug->IsChecked();
  mConfig->mV107Value = mV107->IsChecked();
  mConfig->Update();
  return;
}

wxString DFArcFrame::BuildCommand(enum program progname)
{
  wxString lCommand;
  wxString executable;
  if (progname == GAME)
    executable = mConfig->mDinkExe;
  else
    executable = mConfig->mEditorExe;

  // Attempt to use the binary in the Dink directory (important for
  // disambiguation under woe, where the PATH includes the current
  // DFArc _binary_ directory (not cwd()) by default).
  wxString test_dinkref = mConfig->GetDinkrefDir() + wxFileName::GetPathSeparator() + executable;
  if (::wxFileExists(test_dinkref))
    lCommand = test_dinkref;
  else
    lCommand = executable;

  if (mConfig->mDebugValue == true)
    lCommand += _T(" -debug ");
  if (mConfig->mSoundValue == false)
    lCommand += _T(" -nosound ");
  if (mConfig->mTrueColorValue == true)
    lCommand += _T(" -truecolor ");
  if (mConfig->mWindowedValue == true)
    lCommand += _T(" -window ");
  if (mConfig->mWriteIniValue == false)
    lCommand += _T(" -noini ");
  if (mConfig->mJoystickValue == false)
    lCommand += _T(" -nojoy ");
  if (mConfig->mV107Value == true)
    lCommand += _T(" --v1.07 ");
  
  /* Specify the directory, as short as possible*/
  wxString dinkref = mConfig->GetDinkrefDir();
  wxFileName dmod_dir(mConfig->mSelectedDmod);
  dmod_dir.MakeRelativeTo(dinkref);
  if (dmod_dir.GetFullPath().StartsWith(_T("..")))
    dmod_dir.MakeAbsolute(dinkref);
  if (dmod_dir.GetFullPath().IsSameAs(_T("dink")))
    dmod_dir = wxEmptyString;
  if (!dmod_dir.GetFullName().IsEmpty())
    {
      wxString fullpath = dmod_dir.GetFullPath();
      if (fullpath.Find(wxT(' ')) != wxNOT_FOUND)
	// Only do that if necessary, quoting doesn't work with the
	// original engine for some reason
	fullpath = _T("\"") + dmod_dir.GetFullPath() + _T("\"");
      lCommand += _T(" -game ") + fullpath;
    }

  /* Always specify the dinkref dir, because we don't know what
     FreeDink's default refdir is. (/usr/local?  /usr?...) */
  /* Note: this is ignored by Seth and Dan's versions */
  lCommand += _T(" --refdir \"") + mConfig->GetDinkrefDir() + _T("\"");

  /* If at a point we need better escaping, it is suggested to replace
     spaces with '\ ' under GNU/Linux and '" "' under woe -
     http://trac.wxwidgets.org/ticket/4115#comment:22 . I wish the
     wxWidgets dev properly fixed it with a wxExecute(wxArrayString)
     version... */

  return lCommand;
}

void DFArcFrame::OnPlay(wxCommandEvent &Event)
{
  wxString lCommand = BuildCommand(GAME);


  // Configure locale
  wxString locale_name = mConfig->mForceLocale;
  int idx = mGameLocaleList->GetSelection();
  if (idx != wxNOT_FOUND && mGameLocaleList->HasClientObjectData())
    {
      wxClientData* wcd = mGameLocaleList->GetClientObject(idx);
      if (wcd != NULL)
	locale_name = ((ClientDataString*)wcd)->s;
    }

  wxEnvVariableHashMap env;
  wxGetEnvMap(&env);

  if (locale_name != wxEmptyString)
    {
      // locale_name is in the form 'xx_YY', e.g. 'da_DK'

      // (gettext.info.gz)The LANGUAGE variable: LANGUAGE > LC_ALL > LC_* > LANG
      env.erase("LANGUAGE");
      env.erase("LC_ALL");

      // Locales need to be precisely set, including the encoding,
      // even if in the case of FreeDink, we don't need it.
      // http://lists.gnu.org/archive/html/bug-gnu-utils/2010-10/msg00018.html
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
      // Under woe, things are pretty sloppy, no worry - except that
      // you won't get any warning if you lack fonts or whatever is
      // needed for your language.
      env["LC_ALL"] = locale_name;
#else
      // Look through $(locale -a) and check if there's a matching
      // locale, otherwise explain to the user that he needs to
      // install the locale.  Not sure if there's a similar method
      // under woe to get the installed locales.
      wxArrayString installed_locales;
      long code = ::wxExecute("locale -a", installed_locales, wxEXEC_SYNC);
      if (code == 0) // good exit code
	{
	  bool found = false;
	  if (!found)
	    {
	      for (int i = 0; i < installed_locales.Count(); i++)
		{
		  if (installed_locales.Item(i).StartsWith(locale_name))
		    {
		      found = true;
		      locale_name = installed_locales.Item(i);
		      env["LC_ALL"] = locale_name;
		      break;
		    }
		}
	    }
	  if (!found)
	    {
	      // Try with the language name  / without the country name
	      wxString language_name = locale_name.Mid(0,2);
	      for (int i = 0; i < installed_locales.Count(); i++)
		{
		  if (installed_locales.Item(i).StartsWith(language_name))
		    {
		      found = true;
		      locale_name = installed_locales.Item(i);
		      env["LC_ALL"] = locale_name;
		      break;
		    }
		}
	    }
	  if (!found)
	    {
	      // Poor-man's / fallback i18n:
	      // - LC_CTYPE and LC_MESSAGES to en_US.UTF-8 (if available)
	      // - LANGUAGE to the locale
	      found = (installed_locales.Index("en_US.UTF-8", false) != wxNOT_FOUND);
	      if (!found)
		found = (installed_locales.Index("en_US.utf8", false) != wxNOT_FOUND);
	      if (found)
		{
		  env["LC_CTYPE"] = "en_US.UTF-8";
		  env["LC_MESSAGES"] = "en_US.UTF-8";
		  env["LANGUAGE"] = locale_name;
		}
	    }
	  if (!found)
	    ::wxMessageBox(wxString::Format(_("The '%s' locale is not installed on your computer"
					      " (locales tells the computer how to manage a language)."
					      " You need to install it - check your system documentation."),
					    locale_name.c_str()), _("Warning"), wxICON_EXCLAMATION, this);
	  // try anyway; this also prevent fallback-ing to the system language
	  env["LC_ALL"] = locale_name;
	}
#endif
    }
  

  // Start the child process.
  long code = 0;
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
  wxLogNull logNo;  // remove redundant "Execution of command 'xxx' failed" popup
#endif
  wxExecuteEnv exec_env;
  exec_env.env = env;
  MonitorDinkExit* process = new MonitorDinkExit(mConfig);
  if ((code = ::wxExecute(lCommand, wxEXEC_ASYNC, process, &exec_env)) == 0)
    {
      // On woe, returns immediately if cannot run the process (not using fork&exec)
      ::wxMessageBox(wxString::Format(_("Dink Smallwood ('%s') was not found on your computer."
					" Please configure the Dink program name in the Options menu."),
				      mConfig->mDinkExe.c_str(), wxICON_ERROR, this),
		     _("Error"));
      delete process;
    }
  else if (mConfig->mCloseDfarcOnPlay)
    {
      // since we're async, DFArc will have exited before we know that e.g. Dink couldn't be found
      this->Close(true);
    }
}

void DFArcFrame::onEdit( wxCommandEvent& aEvent )
{
  if (mConfig->mWarnOnEdit == true)
    {
      if (::wxMessageBox(_("Dinkedit saves all changes automatically."
			   " Altering maps can ruin the game."
			   " Are you sure you want to continue?"),
			 _("Warning"),
			 wxOK | wxCANCEL | wxICON_INFORMATION, this) == wxCANCEL)
        {
	  return;
        }
      else
        {
	  // Don't display the warning again
	  mConfig->mWarnOnEdit = false;
	  mConfig->Update();
        }
    }


  wxString lCommand = BuildCommand(EDITOR);
  
  // Start the child process.
  long code = 0;

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
  wxLogNull logNo;  // remove redundant "Execution of command 'xxx' failed" popup
#endif
  MonitorEditorExit* process = new MonitorEditorExit(mConfig);
  if ((code = ::wxExecute(lCommand, wxEXEC_ASYNC, process)) == 0)
    ::wxMessageBox(wxString::Format(_("The editor ('%s') was not found on your computer."
				      " Please configure the editor program name in the Options menu."),
				    mConfig->mEditorExe.c_str()),
		   _("Error"));
}

void DFArcFrame::onRefresh( wxCommandEvent& aEvent )
{
  refreshDmodList();
  return;
}

void DFArcFrame::onDownload( wxCommandEvent& aEvent )
{
  ::wxLaunchDefaultBrowser(_T("http://www.dinknetwork.com/"));
}

void DFArcFrame::onWalkthroughs( wxCommandEvent& aEvent )
{
  ::wxLaunchDefaultBrowser(_T("http://solutions.dinknetwork.com/"));
}

void DFArcFrame::onForums( wxCommandEvent& aEvent )
{
  ::wxLaunchDefaultBrowser(_T("http://www.dinknetwork.com/forum.cgi"));
}

void DFArcFrame::onBrowse( wxCommandEvent& aEvent )
{
  wxString cur_dmod_dir = mConfig->mSelectedDmod;
  if (mConfig->mPreferredFileBrowserExe.IsEmpty())
    {
      // Try default browser(s)
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
      // Use '\' to force opening the directory, not dink.exe...
      ::ShellExecute(0, _T("open"), (cur_dmod_dir + wxT("\\")).c_str(), NULL, NULL, SW_SHOW);
#else
      /* Crudely escape spaces, since wxWidgets 2.8 doesn't have a
	 proper way to separate command line arguments */
      cur_dmod_dir.Replace(wxT(" "), wxT("\\ "), true);
      if (::wxExecute(_T("xdg-open ") + cur_dmod_dir) < 0) // FreeDesktop
	if (::wxExecute(_T("nautilus ") + cur_dmod_dir) < 0) // Gnome
	  if (::wxExecute(_T("konqueror ") + cur_dmod_dir) < 0) // KDE
	    if (::wxExecute(_T("thunar ") + cur_dmod_dir) < 0) // Xfce
	      ::wxMessageBox(_("Could not find a file manager"
			       " (tried 'xdg-open', 'nautilus', 'konqueror' and 'thunar')"),
			     _("Error"), wxICON_ERROR);
#endif
    }
  else
    {
      if (::wxExecute(mConfig->mPreferredFileBrowserExe + _T(" ") + cur_dmod_dir) < 0)
	::wxMessageBox(wxString::Format(_("Cannot start '%s', please check your"
					  " configuration in the Options window."),
					mConfig->mPreferredFileBrowserExe.c_str()),
		       _("Error"), wxICON_ERROR);
    }
  return;
}

void DFArcFrame::showIntroductionText(wxCommandEvent& aEvent)
{
  ::wxMessageBox(_("Welcome to DFArc, the Dink Smallwood front end!\n"
"\n"
"You can choose to play the original game (Dink Smallwood) or"
" Dink-Modules (D-Mods) which contain new adventures.\n"
"\n"
"After completing the main game, give some D-Mods a try.\n"
"There are hundreds of them, just click File-Download D-Mods."),
		 _("Introduction"),
		 wxOK | wxICON_INFORMATION, this);
  return;
}

void DFArcFrame::onShow(wxShowEvent& aEvent)
{
  if (mConfig->mShowIntroductionText == true)
    {
      wxCommandEvent se;
      showIntroductionText(se);
      mConfig->mShowIntroductionText = false;
      mConfig->Update();
    }
  return;
}

void DFArcFrame::refreshDmodList()
{
  mDModListBox->Clear();
  populateAvailableDModsList();
  
  if (mAvailableDModsList.empty() == false)
    {
      mNoDmods = false;
      for (unsigned int i = 0; i < mAvailableDModsList.size(); i++)
	{
	  // Using 'wxClientData*' as int for simplicity.
	  Integer *I = new Integer(i);
	  mDModListBox->Append(mAvailableDModsList.at(i).GetTitle(), I);
	}
    }
  else
    {
      mNoDmods = true;
    }
  refreshDmodLogos();
  RestoreListBoxFromConfig();
}

/**
 * Create a big image with all the D-Mods logos, used for the
 * transition effect that happens when selecting a different D-Mod.
 */
void DFArcFrame::refreshDmodLogos()
{
  mAllLogos = wxBitmap(LOGO_WIDTH, (mDModListBox->GetCount()+1) * LOGO_HEIGHT);
  wxMemoryDC lAllDC(mAllLogos);
  lAllDC.DrawBitmap(mDefaultLogoBitmap, 0,0, false);

  for (unsigned int i = 0; i < mDModListBox->GetCount(); i++)
    {
      Integer *I = (Integer*)mDModListBox->GetClientObject(i);
      int cur_dmod_index = I->i;
      DMod cur_dmod = mAvailableDModsList.at(cur_dmod_index);

      wxBitmap lLogoBitmap;
      
      wxString lDmodPreview(cur_dmod.GetFullPath() + _T("/preview.bmp"));
      IOUtils::ciconvert(lDmodPreview);
      wxString lDmodTitle(cur_dmod.GetFullPath() + _T("/graphics/title-01.bmp"));
      IOUtils::ciconvert(lDmodTitle);
      if (::wxFileExists(lDmodPreview) == true)
	{
	  lLogoBitmap = wxBitmap(lDmodPreview, wxBITMAP_TYPE_BMP);
	}
      else if (::wxFileExists(lDmodTitle) == true)
	{
	  wxImage lImage(lDmodTitle);
	  lImage.Rescale(LOGO_WIDTH, LOGO_HEIGHT);
	  lLogoBitmap = wxBitmap(lImage);
	}
      else
	{
	  lLogoBitmap = CreateTextLogo(cur_dmod.GetBaseName());
	}
      lLogoBitmap.SetWidth(LOGO_WIDTH);
      lLogoBitmap.SetHeight(LOGO_HEIGHT);

      lAllDC.DrawBitmap(lLogoBitmap, 0,(i+1)*LOGO_HEIGHT, false);
      //mLogoButton->SetBitmap(lLogoBitmap);
      //wxMemoryDC src(lLogoBitmap);
      //wxBitmap dst_bitmap = mLogoButton->GetBitmap();
      //wxMemoryDC dst(dst_bitmap);
      //dst.Blit(0,0, LOGO_WIDTH, LOGO_HEIGHT, &src, 0,0);
      //dst.DrawBitmap(lLogoBitmap, 0,0, false);
      //dst.DrawRectangle(0,0, LOGO_WIDTH,LOGO_HEIGHT);
      //mLogoButton->SetBitmap(dst_bitmap);
    }
}

/**
 * Select DMod from configuration
 */
void DFArcFrame::RestoreListBoxFromConfig()
{
  int lb_index_to_select = wxNOT_FOUND;
  // Update the selection
  if (!mConfig->mSelectedDmod.IsEmpty())
    {
      for (unsigned int i = 0; i < mDModListBox->GetCount(); i++)
	{
	  Integer *I = (Integer*)mDModListBox->GetClientObject(i);
	  int cur_dmod_index = I->i;
	  DMod cur_dmod = mAvailableDModsList.at(cur_dmod_index);

	  if (cur_dmod.GetFullPath().IsSameAs(mConfig->mSelectedDmod))
	    {
	      lb_index_to_select = i;
	      break;
	    }
	}
      mDModListBox->SetSelection(lb_index_to_select);
      SelectDModFromListBox();
    }
  else
    {
      mPlayButton->Disable();
      mEditButton->Disable();
      mPackageButton->Disable();
      ((DFAnimationPanel*)mAnimationPanel)->setIndex(-1);
    }
}

void DFArcFrame::showOptions(wxCommandEvent& aEvent)
{
  Options* lOptions = new Options(mConfig);
  if (lOptions->ShowModal() == 1)
    {
      // Apply configuration changes
      showDeveloperButtons(mConfig->mShowDeveloperButtons);
      if (mConfig->mOverrideDinkrefDir)
	{
	  if (::wxDirExists(mConfig->mSpecifiedDinkrefDir))
	    {
	      // Get the full path to the directory, in case users type in a non-full path.
	      if (::wxSetWorkingDirectory(mConfig->mSpecifiedDinkrefDir))
		{
		  mConfig->mSpecifiedDinkrefDir = ::wxGetCwd();
		}
	      else
		{
		  // If there's no directory, let's not override
		  ::wxMessageBox(_("Cannot use the overridden Dink Smallwood directory"
				   " - ignoring it. (permission problem?)"),
				 _("Configuration error"));
		  mConfig->mOverrideDinkrefDir = false;
		}
	    }
	  else
	    {
	      // If there's no directory, let's not override
	      ::wxMessageBox(_("The Dink Smallwood directory you entered does not exist - ignoring it."),
			     _("Configuration error"));
	      mConfig->mOverrideDinkrefDir = false;
	    }
	}
      refreshDmodList();
    }
  lOptions->Destroy();
  return;
}

void DFArcFrame::uninstall( wxCommandEvent& aEvent )
{
  if (mConfig->mSelectedDmod == _T("dink"))
    {
      wxMessageBox(_("You must select the uninstall option from the start menu to uninstall the main game."),
		   _("Uninstall - Error"),
		   wxOK | wxICON_INFORMATION, this);
    }
  else
    {
      int lResult = wxMessageBox(_("Do you want to remove all save game files?"),
				 _("Uninstall - Save Game Files"),
				 wxYES_NO | wxCANCEL | wxICON_QUESTION, this);
      if (lResult != wxCANCEL)
        {
	  bool lRemoveSaveGames(false);
	  if (lResult == wxYES)
            {
	      lRemoveSaveGames = true;
            }
	  
	  RecursiveDelete lRecursiveDelete(lRemoveSaveGames);
	  wxDir* lDir = new wxDir(mConfig->mSelectedDmod);
	  lDir->Traverse(lRecursiveDelete);
	  delete lDir;
	  
	  if (lRecursiveDelete.getError() == false)
            {
	      bool lSuccess(true);
	      if (lRemoveSaveGames == true)
                {
		  if (::wxRmdir(mConfig->mSelectedDmod) == false)
                    {
		      wxLogError(_("Unable to remove D-Mod directory. All other files were removed."));
		      lSuccess = false;
                    }
                }
	      if (lSuccess == true)
                {
		  wxMessageBox(_("D-Mod successfully uninstalled"),
			       _("Uninstall - Success"),
			       wxOK | wxICON_INFORMATION, this);
                }
            }
	  mConfig->mSelectedDmod = wxEmptyString;
	  refreshDmodList();
        }
    }
  return;
}





/**
 * This finds all of the DMods.
 */
void DFArcFrame::populateAvailableDModsList()
{
  mAvailableDModsList.clear();

  wxString* folders = new wxString[2];
  folders[0] = mConfig->GetDinkrefDir();
  folders[1] = mConfig->mDModDir;

  for (int i = 0; i < 2; i++)
    {
      if (folders[i].IsEmpty() || !::wxDirExists(folders[i]))
	continue;
      wxDir lDirectory(folders[i]);
      if (lDirectory.IsOpened() == false)
	continue;

      // Grab dir names.
      wxString lCurrentFolder;
      bool cont = lDirectory.GetFirst(&lCurrentFolder, wxEmptyString, wxDIR_DIRS);
      while (cont)
	{
	  wxString full_path = folders[i] + wxFileName::GetPathSeparator() + lCurrentFolder;
	  if (DMod::IsADModDir(full_path))
	      mAvailableDModsList.push_back(DMod(full_path));
	  cont = lDirectory.GetNext(&lCurrentFolder);
	}
    }

  delete[] folders;
  return;
}
