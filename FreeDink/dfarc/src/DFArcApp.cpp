/**
 * DFArc startup - wxWidgets application object

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

#include "Config.hpp"
#include "DFArcFrame.hpp"
#include "InstallVerifyFrame.hpp"

#include <wx/app.h>

class DFArcApp : public wxApp
{
  bool OnInit();
  int OnExit();
  virtual void OnUnhandledException();
private:
  // wxLocale object must live during all the program execution, or
  // translation will stop - so let's define it as an app member
  wxLocale app_locale;
};

DECLARE_APP(DFArcApp)


bool DFArcApp::OnInit(void)
{
  // Init i18n or internationalization
  // Under GNU/Linux, wx looks in standard /usr/share/locale/...
  // Under woe, wx looks in ./fr/dfarc.mo. Add ./po/fr/dfarc.mo:
  wxLocale::AddCatalogLookupPathPrefix(_T("po"));

  // Init locale, possibly overriding the system language
  Config *mConfig = Config::GetConfig(); // singleton
  const wxLanguageInfo* li = wxLocale::FindLanguageInfo(mConfig->mForceLocale);
  if (mConfig->mForceLocale == wxT("C"))
    ; // do not initialize i18n
  else if (mConfig->mForceLocale != wxEmptyString && li != NULL)
    app_locale.Init(li->Language);
  else
    app_locale.Init();

  app_locale.AddCatalog(wxT("dfarc"));

  int cont = true;

  if (argc == 2)
    {
      wxString arg = argv[1];
      if (arg.Cmp(_T("--help")) == 0)
	{
	  wxString progname = argv[0];
	  printf("Usage: %s [OPTIONS]... [file.dmod]\n", (const char*)progname.fn_str());
	  printf("\n");
	  printf("-h, --help            Display this help screen\n");
	  printf("-v, --version         Display the version\n");
	  printf("\n");
	  printf("If file.dmod is given, the DMod installation wizard will start.\n");
	  printf("\n");
	  /* printf ("Type 'info freedink' for more information\n"); */
	  printf("Report bugs to %s.\n", PACKAGE_BUGREPORT);
	  exit(EXIT_SUCCESS);  // don't 'return false', otherwise wxWidgets exit(255)
	}
      else if (arg.Cmp(_T("--version")) == 0)
	{
	  printf ("%s %s\n", PACKAGE_NAME, VERSION);
	  printf ("Copyright (C) 2006, 2007, 2008 by contributors\n");
	  printf ("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
	  printf ("This is free software: you are free to change and redistribute it.\n");
	  printf ("There is NO WARRANTY, to the extent permitted by law.\n");
	  exit(EXIT_SUCCESS);  // don't 'return false', otherwise wxWidgets exit(255)
	}
      else
	{
	  wxString lDmodFilePath = arg;
	  InstallVerifyFrame lTemp(lDmodFilePath);
	  cont = (lTemp.ShowModal() == wxID_OK);
	}
    }

  if (cont)
    {
      // Normal startup, or successful installation
      DFArcFrame* lDFArcMainForm = new DFArcFrame();
      lDFArcMainForm->Show();
      return true;
    }

  // Failed installation, not going to main screen, just quitting
  return false;
}

int DFArcApp::OnExit()
{
  // Singletons don't delete themselves automatically. Moreover, a
  // static SingletonDestroyer solution would happen after wxWindows
  // clean-up and crash. So we're deleting it here.
  delete Config::GetConfig(); // saves wxConfig on disk
  return wxApp::OnExit();
}

void DFArcApp::OnUnhandledException()
{
  // Rethrow the current exception so we can debug it
  // Note: it seems I can't get the original strack trace in gdb
  throw;
}

IMPLEMENT_APP(DFArcApp)
