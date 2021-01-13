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
 * Add a structure to the ground
 *****************************************************************************/

#include <WARMUX_types.h>
#include "weapon/construct.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/interface.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/network.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"

#ifdef DEBUG
#include "graphic/video.h"
#include "graphic/colors.h"
#include "include/app.h"
#endif

static const Double DELTA_ANGLE = PI / 6; // should be a multiple


Construct::Construct() : Weapon(WEAPON_CONSTRUCT, "construct",
                                new WeaponConfig(),
                                false)
{
  UpdateTranslationStrings();

  construct_spr = GetResourceManager().LoadSprite( weapons_res_profile, "construct_spr");
  // The 12 is the result of (TWO * PI) / DELTA_ANGLE
  construct_spr->EnableCaches(false, 12);
  m_name = _("Construct");
  m_category = TOOL;
  m_can_change_weapon = true;
  angle = 0;
  target_chosen = false;
}

void Construct::UpdateTranslationStrings()
{
  m_name = _("Construct");
  m_help = _("Click onto the place, where you\nwant to place the construction.\nUp/down changes direction.");
}

Construct::~Construct()
{
  delete construct_spr;
}

std::string Construct::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext("%s team has won %u construct weapon! Don't forget your helmet.",
                         "%s team has won %u construct weapons! Don't forget your helmet.",
                         items_count),
                TeamName, items_count);
}

bool Construct::p_Shoot()
{
  if (!target_chosen)
    return false;
  JukeBox::GetInstance()->Play("default", "weapon/construct");
  GetWorld().MergeSprite(dst - construct_spr->GetSizeMax()/2, construct_spr);

  target_chosen = false; // ensure next shoot cannot be done pressing key space
  return true;
}

void Construct::Draw()
{
  Weapon::Draw();

  if (EnoughAmmo()
      && EnoughAmmoUnit()
      && !Interface::GetInstance()->weapons_menu.IsDisplayed()
      && Interface::GetInstance()->IsDisplayed()
      && Network::GetInstance()->IsTurnMaster()) {

    dst = Mouse::GetInstance()->GetWorldPosition();
    construct_spr->SetRotation_rad(angle);
    construct_spr->Draw(dst - construct_spr->GetSize() / 2);

#ifdef DEBUG
    if (IsLOGGING("test_rectangle")) {
      Rectanglei test_rect(dst - construct_spr->GetSizeMax() / 2, construct_spr->GetSizeMax());
      test_rect.SetPosition(test_rect.GetPosition() - Camera::GetInstance()->GetPosition());
      GetMainWindow().RectangleColor(test_rect, primary_red_color, 1);
    }
#endif
  }
}

void Construct::ChooseTarget(Point2i mouse_pos)
{
  if (!EnoughAmmo())
    return;

  dst = mouse_pos;

  // Draw it so that GetSizeMax() returns the correct values.
  construct_spr->SetRotation_rad(angle);
  construct_spr->Draw(dst - construct_spr->GetSize() / 2);

  Point2i test_target = dst - construct_spr->GetSizeMax() / 2;
  Rectanglei rect(test_target, construct_spr->GetSizeMax());

  if (!GetWorld().ParanoiacRectIsInVacuum(rect))
    return;

  // Check collision with characters and other physical objects
  FOR_ALL_CHARACTERS(team, c) {
    if (c->GetTestRect().Intersect(rect))
      return;
  }

  FOR_ALL_OBJECTS(it) {
    PhysicalObj *obj = *it;
    if (obj->GetTestRect().Intersect(rect))
      return;
  }

  target_chosen = true;
  Shoot();
}

void Construct::Up() const
{
  Double new_angle = angle + DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

void Construct::Down() const
{
  Double new_angle = angle - DELTA_ANGLE;

  Action* a = new Action(Action::ACTION_WEAPON_CONSTRUCTION, new_angle);
  ActionHandler::GetInstance()->NewAction(a);
}

WeaponConfig& Construct::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }
