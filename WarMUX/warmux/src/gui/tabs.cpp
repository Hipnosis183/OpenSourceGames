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
 * Tabs
 *****************************************************************************/

#include <algorithm> // std::min/max

#include "include/app.h"
#include "graphic/colors.h"
#include "graphic/font.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/button.h"
#include "gui/tabs.h"
#include "tool/resource_manager.h"

#define TAB_MIN_WIDTH 200

static Point2i widget_size(150, 150);

class Tab
{
private:
  std::string id;
  std::string title;

public:
  Widget* box;

public:
  Tab(const std::string& _id, const std::string& _title, Widget* _box) :
    id(_id), title(_title), box(_box) {};

  ~Tab() {};

  const std::string& GetTitle() const { return title; };
  const std::string& GetId() const { return id; };
};

#define CIRCULAR_TABS   0
#define BORDER          2
#define MARGIN          5

MultiTabs::MultiTabs(const Point2i& size)
 : Widget(size)
 , max_visible_tabs(50)
 , current_tab(-1)
 , first_tab(0)
 , nb_visible_tabs(1)
 , tab_header_width(TAB_MIN_WIDTH)
{
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml",false);

  prev_tab_bt = new Button(res, "menu/really_big_minus", false);
  next_tab_bt = new Button(res, "menu/really_big_plus", false);
  tab_header_height = prev_tab_bt->GetSizeY() + 4;

  Widget::SetBorder(defaultOptionColorRect, BORDER);
  Widget::SetBackgroundColor(defaultOptionColorBox);
}

MultiTabs::~MultiTabs()
{
  for (std::vector<Tab>::iterator t=tabs.begin(); t != tabs.end(); t++)
    delete t->box;

  delete prev_tab_bt;
  delete next_tab_bt;

  tabs.clear();
}

void MultiTabs::SelectTab(uint current)
{
  ASSERT(!tabs.empty());

  if (current >= tabs.size())
    return;

  if (current_tab != current) {
    current_tab = current;

    // update first_tab to be sure that current tab will be visible
    if (current_tab < first_tab)
      first_tab = current_tab;
    else if (current_tab+1 > first_tab + nb_visible_tabs)
      first_tab = current_tab - nb_visible_tabs + 1;

    NeedRedrawing();
  }
}

void MultiTabs::PrevTab()
{
  if (tabs.empty())
    return;

  if (current_tab == 0) {
#if CIRCULAR_TABS
    SelectTab(tabs.size()-1);
#endif
    return;
  }

  SelectTab(current_tab-1);
}

void MultiTabs::NextTab()
{
  if (tabs.empty())
    return;

  if (current_tab+1 == tabs.size()) {
#if CIRCULAR_TABS
    SelectTab(0);
#endif
    return;
  }

  SelectTab(current_tab+1);
}

void MultiTabs::AddNewTab(const std::string& id, const std::string& title, Widget * w)
{
  Tab tab(id, title, w);
  tabs.push_back(tab);
  w->SetContainer(this);

  SelectTab(0);
}

void MultiTabs::DrawHeader(const Point2i &mousePosition) const
{
  if (tabs.empty())
    return;

  // Draw the buttons to change tab
  if (nb_visible_tabs < tabs.size()) {
#if !CIRCULAR_TABS
    if (first_tab != 0)
#endif
      prev_tab_bt->Draw(mousePosition);

#if !CIRCULAR_TABS
    if (first_tab + nb_visible_tabs != tabs.size())
#endif
      next_tab_bt->Draw(mousePosition);
  }

  for (uint i = first_tab; i < first_tab + nb_visible_tabs; i++) {
    // Draw the title
    uint pos_x = prev_tab_bt->GetPositionX() + prev_tab_bt->GetSizeX()
               + MARGIN + (i-first_tab)*tab_header_width;

    bool is_current = i == current_tab;
    Text tab_title(tabs[i].GetTitle(), is_current ? primary_red_color : dark_gray_color,
                   Font::FONT_MEDIUM, Font::FONT_BOLD, is_current);

    tab_title.DrawCenter(Point2i(pos_x + tab_header_width/2,
                                 position.y + tab_header_height/2));
  }

  if (nb_visible_tabs > 1) {
    uint end_y = position.y + tab_header_height - 2;
    uint current_tab_pos_x = prev_tab_bt->GetPositionX() + prev_tab_bt->GetSizeX()
                           + MARGIN + (current_tab-first_tab)*tab_header_width;

    GetMainWindow().VlineColor(current_tab_pos_x, position.y +1, end_y, GetBorderColor());
    GetMainWindow().VlineColor(current_tab_pos_x + tab_header_width, position.y +1, end_y, GetBorderColor());
    GetMainWindow().HlineColor(position.x, current_tab_pos_x, end_y, GetBorderColor());
    GetMainWindow().HlineColor(current_tab_pos_x + tab_header_width, position.x + size.x - 2, end_y, GetBorderColor());
  }
}

void MultiTabs::Draw(const Point2i &mousePosition)
{
  if (tabs.empty())
    return;

  // Draw the header
  DrawHeader(mousePosition);

  // Draw the current tab
  tabs[current_tab].box->Draw(mousePosition);
}

bool MultiTabs::Update(const Point2i &mousePosition,
                       const Point2i &lastMousePosition)
{
  Rectanglei header(position.x, position.y,
                    next_tab_bt->GetPositionX() + next_tab_bt->GetSizeX() - position.x,
                    prev_tab_bt->GetSizeY());

  bool updated = false;
  if (need_redrawing) {
    updated = Widget::Update(mousePosition, lastMousePosition);
  } else if ((header.Contains(mousePosition) && mousePosition != lastMousePosition) ||
             (header.Contains(lastMousePosition) && !header.Contains(mousePosition))) {
    RedrawBackground(header);

    DrawHeader(mousePosition);
    RedrawForeground();
    updated = true;
  }
  if (!tabs.empty())
    tabs[current_tab].box->Update(mousePosition, lastMousePosition);

  need_redrawing = false;
  return updated;
}

void MultiTabs::Pack()
{
  // Update buttons position
  prev_tab_bt->SetPosition(position.x + MARGIN, position.y + 2);
  next_tab_bt->SetPosition(position.x + size.x - MARGIN - next_tab_bt->GetSizeX(),
                           position.y + 2);

  // Update tabs position
  Point2i tab_pos(position.x + MARGIN, position.y + GetHeaderHeight());
  Point2i tab_size(size.x - 2*MARGIN, size.y - GetHeaderHeight() - MARGIN);

  for (std::vector<Tab>::iterator it=tabs.begin(); it!=tabs.end(); it++) {
    (*it).box->SetPosition(tab_pos);
    (*it).box->SetSize(tab_size);
    (*it).box->Pack();
  }

  // Compute how many tabs can be displayed
  nb_visible_tabs = std::min(uint(tabs.size()), uint(GetSizeX() / TAB_MIN_WIDTH));
  nb_visible_tabs = std::min(nb_visible_tabs, max_visible_tabs);

  if (nb_visible_tabs == 0)
    nb_visible_tabs = 1;

  // Compute tab size in the header
  tab_header_width = ((next_tab_bt->GetPositionX() -
                      (prev_tab_bt->GetPositionX() + prev_tab_bt->GetSizeX()) - 10))
                   / nb_visible_tabs;
}

void MultiTabs::NeedRedrawing()
{
  need_redrawing = true;

  if (!tabs.empty())
    tabs[current_tab].box->NeedRedrawing();
}

bool MultiTabs::SendKey(const SDL_keysym& key)
{
  if (tabs.empty())
    return false;

  if (SDL_GetModState()&(KMOD_CTRL|KMOD_META)) {
    switch (key.sym) {
    case SDLK_PAGEUP:
      PrevTab();
      return true;
    case SDLK_PAGEDOWN:
      NextTab();
      return true;
    default:
      break;
    }
  }

  return tabs[current_tab].box->SendKey(key);
}

Widget* MultiTabs::Click(const Point2i &mousePosition, uint button)
{
  if (!tabs.empty())
    return tabs[current_tab].box->Click(mousePosition, button);

  return NULL;
}

Widget* MultiTabs::ClickUp(const Point2i &mousePosition, uint button)
{
  if (tabs.size() != 1) {

    Rectanglei rect_header(position.x, position.y,
                           size.x, prev_tab_bt->GetSizeY());

    if (rect_header.Contains(mousePosition)) {
      if (button == SDL_BUTTON_WHEELDOWN) {
        PrevTab();

      } else if (button == SDL_BUTTON_WHEELUP) {
        NextTab();

      } else {
        // Either Left, Middle or Right buttons.
        if (prev_tab_bt->Contains(mousePosition)) {
          PrevTab();
        } else if (next_tab_bt->Contains(mousePosition)) {
          NextTab();
        } else if (nb_visible_tabs > 1) {
          uint clicked_tab = (mousePosition.x - prev_tab_bt->GetPositionX()
                              - prev_tab_bt->GetSizeX() - 5)
                           / tab_header_width + first_tab;
          SelectTab(clicked_tab);
        }
      }

      return this;
    }
  }

  if (!tabs.empty())
    return tabs[current_tab].box->ClickUp(mousePosition, button);

  return NULL;
}

const std::string& MultiTabs::GetCurrentTabId() const
{
  ASSERT(!tabs.empty());
  return tabs[current_tab].GetId();
}
