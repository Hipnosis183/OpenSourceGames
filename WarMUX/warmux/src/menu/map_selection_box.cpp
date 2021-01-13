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
 * Map selection box
 *****************************************************************************/

#include <algorithm>

#include "menu/map_selection_box.h"
#include "game/config.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/question.h"
#include "gui/horizontal_box.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "network/network.h"
#include "tool/resource_manager.h"

MapSelectionBox::MapSelectionBox(const Point2i &_size, bool show_border, bool _display_only)
  : VBox(_size.GetX(), show_border, false)
  , selected_map_index(0)
  , common(MapsList::GetInstance()->lst) // Created with an already initialized list
{
  display_only = _display_only;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml",false);

  // PreviousMap/NextMap buttons
  bt_map_plus = new Button(res, "menu/big_plus", false);
  bt_map_minus = new Button(res, "menu/big_minus", false);

  // random map
  random_map_preview = GetResourceManager().LoadImage(res, "menu/random_map");

  // compute margin width between previews
  uint map_preview_height = _size.GetY() -2*10 -40;

  // Previews
  Box* previews_box = new HBox(map_preview_height+10, false, false);
  previews_box->SetNoBorder();

   // compute margin width between previews
  uint map_preview_width = map_preview_height*4/3;
  uint total_width_previews = map_preview_width + map_preview_width*3;

  uint margin = 0;

  if (uint(size.x) > uint(total_width_previews + bt_map_plus->GetSizeX()
                          + bt_map_minus->GetSizeX() + border_size)) {
    margin = ( size.x - (total_width_previews + bt_map_plus->GetSizeX() +
                         bt_map_minus->GetSizeX() + border_size) ) / 6;
  }

  if (margin < 5) {
    margin = 5;
    uint total_size_wo_margin = size.x - 6*margin - bt_map_plus->GetSizeX()
                              - bt_map_minus->GetSizeX() - border_size;
    map_preview_width = (total_size_wo_margin)/4; // <= total = w + 4*(3/4)w
    map_preview_height = 3/4 * map_preview_width;
  }

  previews_box->SetMargin(margin);

  previews_box->AddWidget(bt_map_minus);

  map_preview_before2 = new PictureWidget(Point2i(map_preview_width *3/4,
                                                  map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before2);

  map_preview_before = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_before);

  // Selected map...
  map_preview_selected = new PictureWidget(Point2i(map_preview_width, map_preview_height));
  previews_box->AddWidget(map_preview_selected);

  map_preview_after = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after);

  map_preview_after2 = new PictureWidget(Point2i(map_preview_width *3/4, map_preview_height*3/4));
  previews_box->AddWidget(map_preview_after2);

  previews_box->AddWidget(bt_map_plus);

  AddWidget(previews_box);

  // Map information
  map_name_label = new Label("Map", W_UNDEF, Font::FONT_SMALL,
                             Font::FONT_BOLD, dark_gray_color,
                             Text::ALIGN_CENTER_TOP, false);
  AddWidget(map_name_label);

  map_author_label = new Label("Author", W_UNDEF, Font::FONT_SMALL,
                               Font::FONT_BOLD, dark_gray_color,
                               Text::ALIGN_CENTER_TOP, false);
  AddWidget(map_author_label);

  if (display_only) {
    bt_map_minus->SetVisible(false);
    bt_map_plus->SetVisible(false);
  }
}

void MapSelectionBox::ChangeMapDelta(int delta_index)
{
  ASSERT(!display_only);

  int tmp = selected_map_index + delta_index;

  // +1 is for random map!
  tmp = (tmp < 0 ? tmp + common.size() + 1 : tmp) % (common.size() + 1);

  ChangeMap(tmp);
}

void MapSelectionBox::ChangeMap(uint index)
{
  int tmp;
  if (index > common.size()+1) return;

  // Callback other network players
  if (Network::GetInstance()->IsGameMaster()) {

    selected_map_index = index;
    // We need to do it here to send the right map to still not connected clients
    // in distant_cpu::distant_cpu

    if (selected_map_index == common.size()) { // random map
      MapsList::GetInstance()->SelectMapByName("random");
    } else {
      MapsList::GetInstance()->SelectMapByName(common[index]->GetRawName());
    }

    Action a(Action::ACTION_GAME_SET_MAP);
    MapsList::GetInstance()->FillActionMenuSetMap(a);
    Network::GetInstance()->SendActionToAll(a);
  } else {
    selected_map_index = index;
  }

  // Set Map information
  UpdateMapInfo(map_preview_selected, index, true);

  // Set previews
  tmp = index - 1;
  tmp = (tmp < 0 ? tmp + common.size() + 1: tmp);
  UpdateMapInfo(map_preview_before, tmp, false);

  tmp = index - 2;
  tmp = (tmp < 0 ? tmp + common.size() + 1: tmp);
  UpdateMapInfo(map_preview_before2, tmp, false);

  UpdateMapInfo(map_preview_after,  (index + 1) % (common.size() +1), false);
  UpdateMapInfo(map_preview_after2, (index + 2) % (common.size() +1), false);
}

void MapSelectionBox::UpdateMapInfo(PictureWidget * widget, uint index, bool selected)
{
  if (index == common.size()) {
    UpdateRandomMapInfo(widget, selected);
    return;
  }

  InfoMapBasicAccessor* basic = NULL;

  basic = common[index]->LoadBasicInfo();
  if (!basic) {
    // Error already reported by LoadBasicInfo()
    MapsList *map_list = MapsList::GetInstance();

    // Crude
    MapsList::iterator it = map_list->lst.begin()
                          + map_list->FindMapById(common[index]->GetRawName());
    delete *it;
    map_list->lst.erase(it);

    // Inform network if need be - will call back up to this very function
    Network::GetInstance()->SendMapsList();
    return;
  }

  widget->SetSurface(basic->ReadPreview(), PictureWidget::FIT_SCALING);
  widget->Pack();

  if (display_only && !selected)
    widget->Disable();
  else
    widget->Enable();

  // If selected update general information
  if (selected) {
    map_name_label->SetText(basic->ReadFullMapName());
    map_author_label->SetText(basic->ReadAuthorInfo());
  }
}

void MapSelectionBox::UpdateRandomMapInfo(PictureWidget * widget, bool selected)
{
  widget->SetSurface(random_map_preview, PictureWidget::FIT_SCALING);
  widget->Pack();
  if ((display_only && !selected))
    widget->Disable();
  else
    widget->Enable();

  // If selected update general information
  if (selected) {
    map_name_label->SetText(_("Random map"));
    map_author_label->SetText(_("Choose randomly between the different maps"));
  }
}


Widget* MapSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (display_only)
    return NULL;

  if (!Contains(mousePosition))
    return NULL;

  bool is_click = Mouse::IS_CLICK_BUTTON(button);
  if (is_click && bt_map_minus->Contains(mousePosition)) {
    ChangeMapDelta(-3);
  } else if (is_click && map_preview_before2->Contains(mousePosition)) {
    ChangeMapDelta(-2);
  } else if ((is_click && map_preview_before->Contains(mousePosition))
             || button==SDL_BUTTON_WHEELUP) {
    ChangeMapDelta(-1);
  } else if ((is_click && map_preview_after->Contains(mousePosition))
             || button==SDL_BUTTON_WHEELDOWN) {
    ChangeMapDelta(+1);
  } else if (is_click && map_preview_after2->Contains(mousePosition)) {
    ChangeMapDelta(+2);
  } else if (is_click && bt_map_plus->Contains(mousePosition)) {
    ChangeMapDelta(+3);
  }

  return NULL;
}

Widget* MapSelectionBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void MapSelectionBox::ValidMapSelection()
{
  std::string map_name;

  if (selected_map_index == common.size()) {
    // Choose one and select it!
    map_name = "random";

    if (Network::GetInstance()->IsLocal()) {
      MapsList::GetInstance()->SelectMapByName(map_name);
    }
  } else {
    map_name = common[selected_map_index]->GetRawName();
    MapsList::GetInstance()->SelectMapByName(map_name);
  }

  /* The player chose a map, save it in the main config so that this will be
   * the defaut map at next load of the game */
  Config::GetInstance()->SetMapName(map_name);
}

void MapSelectionBox::ChangeMapCallback()
{
  const InfoMap* current = MapsList::GetInstance()->ActiveMap();
  for (uint i=0; i<common.size(); i++) {
    if (common[i] == current) {
      ChangeMap(i);
      break;
    }
  }
}

void MapSelectionBox::ChangeMapListCallback(const std::vector<uint>& index_list)
{
  std::vector<InfoMap*> local = MapsList::GetInstance()->lst;

  // Index list is made of indices of local maps: it's a subset
  common.resize(index_list.size());
  for (uint i=0; i<index_list.size(); i++) {
    common[i] = local[index_list[i]];
  }
}

void MapSelectionBox::AllowSelection()
{
  display_only = false;
  bt_map_minus->SetVisible(true);
  bt_map_plus->SetVisible(true);
  map_preview_before2->Enable();
  map_preview_before->Enable();
  map_preview_after->Enable();
  map_preview_after2->Enable();
  NeedRedrawing();
}

void MapSelectionBox::Pack()
{
  VBox::Pack();
  ChangeMapCallback();
}
