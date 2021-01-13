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
 * Generic menu
 *****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include "gui/widget_list.h"
#include <WARMUX_base.h>
#include "tool/resource_manager.h"
#include "gui/picture_widget.h"
#include "gui/horizontal_box.h"

// Forward declarations
class Button;
class Label;
class Box;
class Sprite;

typedef enum {
  vOkCancel,
  vOk,
  vCancel,
  vNo
} t_action;

class Menu : public Container
{
public:
  WidgetList widgets;
  const t_action actions;

  Menu(const std::string& bg, t_action actions = vOkCancel);
  Menu(void);
  virtual ~Menu();

  // Start the xml menu configuration.
  void LoadMenu(Profile * profile,
                const xmlNode * rootMenuNode);

  void Run(bool skip=false);
  virtual void RedrawBackground(const Rectanglei& rect) const;
  virtual void RedrawMenu();

  void DisplayError(const std::string &msg);

  // for receiving message from network
  virtual void ReceiveMsgCallback(const std::string& /*msg*/, const Color& /*color*/) {};

  // Push a stupid user event to make the menu exits for SDL_WaitEvent
  // To be called only by the ActionHandler
  static void WakeUpOnCallback();

private:
  Sprite *background;
  Widget *selected_widget;

  void LoadBackground(Profile * profile,
                      const xmlNode * rootMenuNode);

  // Recursive function wich load the widgets, and fill the containers widgets.
  void LoadWidget(Profile * profile,
                  const xmlNode * rootMenuNode,
                  WidgetList * container);

  // Detect and instanciate a widget.
  Widget * CreateWidget(Profile * profile,
                        const xmlNode * rootMenuNode,
                        std::string & widgetName);

  bool BasicOnClickUp(const Point2i &mousePosition);
  bool HandleGlobalEvent(const SDL_Event& evnt);

protected:
  Button *b_cancel;
  Button *b_ok;
  bool close_menu;
  Point2i last_mouse_position;
  /* Actions buttons  */
  HBox *actions_buttons;

  void play_ok_sound();
  void play_cancel_sound();
  void play_error_sound();

  virtual void mouse_ok();
  virtual void mouse_cancel();
  virtual void key_ok();
  virtual void key_cancel();
  virtual void key_up();
  virtual void key_down();
  virtual void key_left();
  virtual void key_right();
  virtual void key_tab();
  virtual bool signal_ok() { return true; }
  virtual bool signal_cancel() { return true; }

  virtual void signal_begin_run() {};

  virtual void DrawBackground();
  virtual void Draw(const Point2i& /*mousePosition*/) { };

  // we have released the button
  virtual void OnClickUp(const Point2i &mousePosition, int button)
  {
    widgets.ClickUp(mousePosition, button);
  }

  // we have clicked but still not released the button
  virtual void OnClick(const Point2i &mousePosition, int button)
  {
    // Do nothing if user has not released the button
    widgets.Click(mousePosition, button);
  }

  // for heavy modification of menu behavior
  virtual void HandleEvent(const SDL_Event& evnt);
  void HandleEvents();

  void SetActionButtonsXY(int x, int y);
  void Display(const Point2i& mousePosition);
};

#endif
