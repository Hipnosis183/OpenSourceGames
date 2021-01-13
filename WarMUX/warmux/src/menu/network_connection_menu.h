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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/

#ifndef NETWORK_CONNECTION_MENU_H
#define NETWORK_CONNECTION_MENU_H

#include <list>
#include "menu.h"
#include "network/network.h"
#include <WARMUX_index_server.h> // for GameServerInfo

// Forward declarations
class Button;
class TextBox;
class PasswordBox;
class MsgBox;
class CheckBox;
class MultiTabs;
class GameListBox;

class NetworkConnectionMenu : public Menu
{
public:
  typedef enum {
    NET_NOTHING,
    NET_HOST,
    NET_CONNECT
  } network_menu_action_t;

private:
  MultiTabs * tabs;

  TextBox* srv_port_number;
  TextBox* srv_game_name;
  PasswordBox* srv_game_pwd;
  CheckBox* srv_internet_server;

  Button* cl_refresh_net_games;
  Label* refresh_net_games_label;
  GameListBox* cl_net_games_lst;
  PasswordBox* cl_net_server_pwd;

  TextBox* cl_server_address;
  TextBox* cl_port_number;
  PasswordBox* cl_server_pwd;

  MsgBox *msg_box;

  uint m_last_click_on_games_lst;
  uint m_Double_click_interval;

  virtual void OnClickUp(const Point2i &mousePosition, int button);
  virtual void HandleEvent(const SDL_Event& evnt);

  void DisplayNetError(connection_state_t conn);

  virtual bool signal_ok();
  virtual bool signal_cancel();

  void __RefreshList();

  bool HostingServer(const std::string& port,
                     const std::string& game_name,
                     const std::string& passwd,
                     bool internet);
  bool ConnectToClient(const std::string& srv_address,
                       const std::string& port,
                       const std::string& passwd);

  static SDL_Thread* thread_refresh;
  void ThreadRefreshList();

public:
  NetworkConnectionMenu(network_menu_action_t action);
  ~NetworkConnectionMenu();
};

#endif
