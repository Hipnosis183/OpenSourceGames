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
 * Display a text during the game, waiting for input by the user
 *****************************************************************************/

#include "graphic/video.h"
#include <SDL_events.h>
#include "gui/question.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "include/app.h"
#include "interface/mouse.h"
#include "tool/resource_manager.h"

Question::Question(type _type)
{
  background = NULL;
  text = NULL;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  switch (_type) {
  case WARNING:
    icon = new Sprite(GetResourceManager().LoadImage(res,"menu/ico_warning"));
    break;
  case NO_TYPE:
    icon = NULL;
    break;
  }
}

Question::~Question()
{
  if (background != NULL)
    delete background;

  if (text != NULL)
    delete text;

  if (icon != NULL)
    delete icon;
}

int Question::TreatsKey (const SDL_Event &evnt){

  // Tests the key
  choice_iterator it=choices.begin(), end=choices.end();
  for (; it != end; ++it){
    if (evnt.key.keysym.sym == it -> key()) {
      return it -> val();
    }
  }

  // No key corresponding to the correct choice, so we use default choice
  if (default_choice.active) {
    return default_choice.value;
  }

  return -1;
}

void Question::Draw() const
{
  AppWarmux * app = AppWarmux::GetInstance();

  Point2i icon_size(0,0);
  Point2i icon_border(0,0);
  if (icon != NULL) {
    icon_size = icon->GetSize();
    icon_border = Point2i(10, 10);
  }

  Rectanglei rect;
  Point2i top_corner;

  if (background != NULL) {
    top_corner = app->video->window.GetSize() / 2 - background->GetSize() / 2;
    rect = Rectanglei(top_corner, background->GetSize());
    background->Blit(app->video->window,  top_corner);
  }
  else if (text->GetText() != "") {
    Point2i rect_size(text->GetWidth() + icon_size.GetX() + icon_border.GetX() + 10,
                      std::max(text->GetHeight(), icon_size.GetY() + icon_border.GetY()) + 10);

    top_corner = app->video->window.GetSize() / 2 - rect_size / 2;
    rect = Rectanglei(top_corner, rect_size);

    AppWarmux * appli = AppWarmux::GetInstance();

    appli->video->window.BoxColor(rect, defaultColorBox);
    appli->video->window.RectangleColor(rect, defaultColorRect);
  }

  if (icon != NULL) {
    Point2i icon_position = top_corner + Point2i(5, rect.GetSizeY()/2 - icon_size.GetY() /2);
    icon->Blit(app->video->window, icon_position);
  }

  if (text->GetText() != "") {
    Point2i text_position = rect.GetPosition() +
      Point2i(rect.GetSizeX() - text->GetWidth()/2 - 10, rect.GetSizeY()/2);
    text->DrawCenter(text_position);
  }
}

int Question::Ask(bool onKeyUp)
{
  SDL_Event evnt;

  int  answer = default_choice.value;
  bool end    = false;

  Draw();
  Mouse::pointer_t prev_pointer = Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
  do {
    while (SDL_PollEvent(&evnt)){
      if ((evnt.type == SDL_QUIT || evnt.type == SDL_MOUSEBUTTONUP) &&
          default_choice.active) {
        answer = default_choice.value;
        end = true;
        break;
      }

      // We might be put inactive while there
      AppWarmux::GetInstance()->CheckInactive(evnt);

      if ((onKeyUp && evnt.type == SDL_KEYUP) || evnt.type == SDL_KEYDOWN) {
        answer = TreatsKey(evnt);
        if (answer != -1) {
          end = true;
          break;
        }
      }
    } // SDL_PollEvent

    // To not use all CPU
    if (!end) {
      SDL_Delay(50);
    }

    AppWarmux::GetInstance()->video->Flip();
  } while (!end);

  AppWarmux::GetInstance()->RefreshDisplay();
  Mouse::GetInstance()->SetPointer(prev_pointer);

  return answer;
}

void Question::Set (const std::string &pmessage,
                    bool default_active, int default_value,const std::string& bg_sprite)
{
  text = new Text(pmessage, white_color, Font::FONT_BIG, Font::FONT_BOLD);

  default_choice.active = default_active;
  default_choice.value = default_value;

  if (background) {
    delete background;
    background = NULL;
  }

  if (bg_sprite != "") {
    Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
    background = new Sprite(LOAD_RES_IMAGE(bg_sprite));
    background->ScaleSize(GetMainWindow().GetSize());
  } else {
    text->SetMaxWidth(GetMainWindow().GetWidth()/2);
  }
}
