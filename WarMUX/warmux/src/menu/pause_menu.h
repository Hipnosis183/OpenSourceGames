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
 * Pause menu
 *****************************************************************************/

#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include "menu/menu.h"

// Forward declarations
class ButtonPic;

class PauseMenu : public Menu
{
private:
  bool& exit_game;

  ButtonPic * bt_continue_play;
  ButtonPic * bt_options_menu;
  ButtonPic * bt_help_menu;
  ButtonPic * bt_main_menu;

  Menu* sub_menu;

  bool signal_ok();
  bool signal_cancel();

  void BackToMainMenu();
  void BackToGame();
  void RunOptionsMenu();
  void RunHelpMenu();

  void Draw(const Point2i &mousePosition);

  void OnClick(const Point2i &mousePosition, int button);
  void OnClickUp(const Point2i &mousePosition, int button);

 public:
  PauseMenu(bool& exit_game);
  ~PauseMenu();

  // Those are needed because PauseMenu might have those called through
  // a callback while displaying a submenu
  void RedrawMenu();
  void DisplayError(const std::string &msg);
};

#endif
