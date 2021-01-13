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
 * Virtual class to handle man/machine interaction
 *****************************************************************************/

#ifndef MAN_MACHINE_INTERFACE_H
#define MAN_MACHINE_INTERFACE_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include <vector>
#include <WARMUX_base.h>
//-----------------------------------------------------------------------------

// Forward declarations
#ifndef _SDL_events_h
union SDL_Event;
#endif

class ManMachineInterface
{
public:
  typedef enum
  {
    KEY_QUIT,
    KEY_WEAPONS1,
    KEY_WEAPONS2,
    KEY_WEAPONS3,
    KEY_WEAPONS4,
    KEY_WEAPONS5,
    KEY_WEAPONS6,
    KEY_WEAPONS7,
    KEY_WEAPONS8,
    KEY_PAUSE,
    KEY_FULLSCREEN,
    KEY_TOGGLE_INTERFACE,
    KEY_CENTER,
    KEY_TOGGLE_WEAPONS_MENUS,
    KEY_CHAT,
    KEY_MOVE_LEFT,
    KEY_MOVE_LEFT_SLOWLY,
    KEY_MOVE_CAMERA_LEFT,
    KEY_MOVE_RIGHT,
    KEY_MOVE_RIGHT_SLOWLY,
    KEY_MOVE_CAMERA_RIGHT,
    KEY_UP,
    KEY_UP_SLOWLY,
    KEY_MOVE_CAMERA_UP,
    KEY_DOWN,
    KEY_DOWN_SLOWLY,
    KEY_MOVE_CAMERA_DOWN,
    KEY_JUMP,
    KEY_HIGH_JUMP,
    KEY_BACK_JUMP,
    KEY_SHOOT,
    KEY_CHANGE_WEAPON,
    KEY_WEAPON_1,
    KEY_WEAPON_2,
    KEY_WEAPON_3,
    KEY_WEAPON_4,
    KEY_WEAPON_5,
    KEY_WEAPON_6,
    KEY_WEAPON_7,
    KEY_WEAPON_8,
    KEY_WEAPON_9,
    KEY_WEAPON_LESS,
    KEY_WEAPON_MORE,
    KEY_NEXT_CHARACTER,
    KEY_MENU_OPTIONS_FROM_GAME,
    KEY_MINIMAP_FROM_GAME,
    KEY_HELP,
    KEY_DECREASE_MINIMAP,
    KEY_INCREASE_MINIMAP,
    KEY_DECREASE_VOLUME,
    KEY_INCREASE_VOLUME,
    KEY_SCREENSHOT,
    KEY_NONE
  } Key_t;

protected:
  // This is a widget in charge of displaying and setting the config
  friend class ControlItem;
  friend class ControlConfig;

  typedef enum
  {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_REFRESH,
    X_AXIS_MOTION,
    Y_AXIS_MOTION
  } Key_Event_t;

  virtual void SetDefaultConfig() { };
  std::map<int, std::vector<Key_t> > layout;
  std::list<uint32_t> registred_event;
  bool PressedKeys[256]; // stupid default value

  void RegisterEvent(uint32_t event_type) { registred_event.push_back(event_type); };
  bool IsRegistredEvent(uint32_t event_type);
  void HandleKeyPressed(const Key_t &action_key);
  void HandleKeyReleased(const Key_t &action_key);

  void SetKeyAction(int key, Key_t at) { layout[key].push_back(at); };
  void ClearKeyAction(Key_t at);
  void ClearKeyBindings() { layout.clear(); }

  int GetKeyFromKeyName(const std::string &name) const;
  std::string GetKeyNameFromKey(int key) const;

  Key_t GetActionFromActionName(const std::string &name) const;
  std::string GetActionNameFromAction(Key_t) const;
  std::string GetHumanReadableActionName(Key_t) const;

  ManMachineInterface() { SetDefaultConfig(); };
  virtual ~ManMachineInterface() { };

public:
  virtual bool HandleKeyEvent(const SDL_Event& evnt) = 0;
  virtual void Reset();

  // Get the key associated to an action.
  int GetKeyAssociatedToAction(Key_t at) const;
};

//-----------------------------------------------------------------------------
#endif /* MAN_MACHINE_INTERFACE_H */
