/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Game menu from which one may start a new game, modify options, obtain some
 * infomations or leave the game.
 *****************************************************************************/

#include "menu/main_menu.h"
#include "gui/grid_box.h"
#include "gui/big/button_pic.h"
#include "game/config.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "sound/jukebox.h"
#include "tool/resource_manager.h"
#include "tool/stats.h"

#ifndef WIN32
#include <dirent.h>
#endif

// Position du texte de la version
const int VERSION_DY = -40;

MainMenu::~MainMenu()
{
  delete version_text;
  delete website_text;
  StatStop("Main:Menu");
}

MainMenu::MainMenu() :
    Menu("main_menu/bg_main", vNo)
{
  Point2i size(120,110);
  Box* box = new GridBox(2, 4, 6, true);

  play = new ButtonPic(_("Play"), "menu/ico_play", size);
  box->AddWidget(play);

  network = new ButtonPic(_("Network Game"), "menu/ico_network_menu", size);
  box->AddWidget(network);

  options = new ButtonPic(_("Options"), "menu/ico_options_menu", size);
  box->AddWidget(options);

  bench = new ButtonPic(_("Benchmark"), "menu/ico_benchmark_menu", size);
  box->AddWidget(bench);

  help = new ButtonPic(_("Help"), "menu/ico_help", size);
  box->AddWidget(help);

  credits = new ButtonPic(_("Credits"), "menu/ico_credits", size);
  box->AddWidget(credits);

  replay = new ButtonPic(_("Replay"), "menu/ico_replay", size);
  box->AddWidget(replay);

  quit =  new ButtonPic(_("Quit"), "menu/ico_quit", size);
  box->AddWidget(quit);

  widgets.AddWidget(box);

  // We must "pack" all the widgets before centering the box to compute its size
  box->Pack();

  uint center_x = GetMainWindow().GetWidth()/2;
  uint center_y = GetMainWindow().GetHeight()/2;
  box->SetPosition(center_x - box->GetSizeX()/2, center_y - box->GetSizeY()/2);

  widgets.Pack();

  std::string s = _("Version") + std::string(" ")+Constants::WARMUX_VERSION;
  version_text = new Text(s, orange_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true);

  std::string s2(Constants::WEB_SITE);
  website_text = new Text(s2, orange_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true);

  if (!JukeBox::GetInstance()->IsPlayingMusic()) {
    JukeBox::GetInstance()->PlayMusic("menu");
  }

  StatStart("Main:Menu");
}

void MainMenu::Init(void)
{
  Profile * xmlProfile = GetResourceManager().LoadXMLProfile("menu.xml", false);
  XmlReader * xmlFile = xmlProfile->GetXMLDocument();

  const xmlNode * mainMenuNode = xmlFile->GetFirstNamedChild(xmlFile->GetRoot(), "MainMenu");
  if (NULL == mainMenuNode) {
    Error("MainMenu: can't load 'MainMenu' xml node from menu.xml");
    exit(EXIT_FAILURE);
  }

  LoadMenu(xmlProfile, mainMenuNode);
}

void MainMenu::SelectAction(const Widget * widget)
{
  if (widget == play) {
    choice = PLAY;
    close_menu = true;
  } else if (widget == network) {
    choice = NETWORK;
    close_menu = true;
  } else if (widget == options) {
    choice = OPTIONS;
    close_menu = true;
  } else if (widget == bench) {
    choice = BENCHMARK;
    close_menu = true;
  } else if (widget == help) {
    choice = HELP;
    close_menu = true;
  } else if (widget == credits) {
    choice = CREDITS;
    close_menu = true;
  } else if (widget == replay) {
    choice = REPLAY;
    close_menu = true;
  } else if (widget == quit) {
    choice = QUIT;
    close_menu = true;
  }

  // New implementation (XML custom menus)
  /*
  if (NULL == widget) {
    return;
  }
  std::string action = widget->GetActionName();
  if ("GoToLocalGameMenu" == action) {
    choice = PLAY;
    close_menu = true;
  } else if ("GoToNetworkGameMenu" == action) {
    choice = NETWORK;
    close_menu = true;
  } else if ("GoToOptionsMenu" == action) {
    choice = OPTIONS;
    close_menu = true;
  } else if ("GoToHelpMenu" == action) {
    choice = HELP;
    close_menu = true;
  } else if ("GoToCreditsMenu" == action) {
    choice = CREDITS;
    close_menu = true;
  } else if ("Quit" == action) {
    choice = QUIT;
    close_menu = true;
  }
  */
}

void MainMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* b = widgets.ClickUp(mousePosition,button);
  if (b) {
    SelectAction(b);
    JukeBox::GetInstance()->Play("default", "menu/clic");
  }
}

void MainMenu::OnClick(const Point2i &/*mousePosition*/, int /*button*/)
{
  // nothing to do while button is still not released
}

MainMenu::menu_item MainMenu::Run ()
{
  choice = NONE;

  Menu::Run();

  ASSERT( choice != NONE );
  return choice;
}

bool MainMenu::signal_cancel()
{
  choice = QUIT;
  return true;
}

bool MainMenu::signal_ok()
{
  Widget * w = widgets.GetCurrentKeyboardSelectedWidget();
  if (w) {
    SelectAction(w);
  } else {
    choice = PLAY;
  }
  return true;
}

void MainMenu::DrawBackground()
{
  Surface& window = GetMainWindow();

  Menu::DrawBackground();

  version_text->DrawCenter(Point2i(window.GetWidth()/2,
                                   window.GetHeight() + VERSION_DY));
  website_text->DrawCenter(Point2i(window.GetWidth()/2,
                                   window.GetHeight() + VERSION_DY/2));
}

void MainMenu::RedrawBackground(const Rectanglei& rect) const
{
  Surface& window = GetMainWindow();

  Menu::RedrawBackground(rect);

  Point2i version_pos(window.GetWidth()/2,
		      window.GetHeight() + VERSION_DY);
  Point2i website_pos(window.GetWidth()/2,
		      window.GetHeight() + VERSION_DY/2);

  if (rect.Contains(version_pos)) {
    version_text->DrawCenter(version_pos);
  }

  if (rect.Contains(website_pos)) {
    website_text->DrawCenter(website_pos);
  }
}
