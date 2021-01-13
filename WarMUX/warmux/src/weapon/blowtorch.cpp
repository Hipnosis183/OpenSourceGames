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
 * Blowtorch - burns holes into walls
 *****************************************************************************/

#include "weapon/blowtorch.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "character/body.h"
#include "map/map.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

static const uint MIN_TIME_BETWEEN_DIG = 200;        // milliseconds

class BlowtorchConfig : public WeaponConfig
{
  public:
    BlowtorchConfig();
    virtual void LoadXml(const xmlNode* elem);

    uint range;
};

Blowtorch::Blowtorch() :
  Weapon(WEAPON_BLOWTORCH, "blowtorch", new BlowtorchConfig()),
  active(false)
{
  UpdateTranslationStrings();

  m_category = TOOL;
  m_time_between_each_shot = MIN_TIME_BETWEEN_DIG;
  m_weapon_fire = new Sprite(GetResourceManager().LoadImage(weapons_res_profile, "blowtorch_fire"));
  m_can_change_weapon = true;
}

void Blowtorch::UpdateTranslationStrings()
{
  m_name = _("Blowtorch");
  m_help = _("Keep the space key pressed\nAngle: Up/Down\nOne ammo per turn");
}

void Blowtorch::p_Deselect()
{
  active = false;
}

bool Blowtorch::p_Shoot()
{
  Point2i hole = ActiveCharacter().GetCenter();

  Double angle = ActiveCharacter().GetFiringAngle();
  uint h = cfg().range;
  Double dx = cos(angle) * h;
  Double dy = sin(angle) * h;

  Point2i pos = Point2i(hole.x+(int)dx, hole.y+(int)dy);
  int char_height = ActiveCharacter().GetHeight();
  int char_width  = ActiveCharacter().GetWidth();
  int size = ((int)sqrt(Double(char_height * char_height + char_width * char_width)))/2;
  GetWorld().Dig(pos, size);
  JukeBox::GetInstance()->Play("default", "weapon/blowtorch");

  return true;
}

void Blowtorch::StartShooting()
{
  if (active) {
     active = false;
     ActiveTeam().AccessNbUnits() = 0;
  } else {
    if (EnoughAmmo())
      active = true;
  }
}

void Blowtorch::StopShooting()
{
  // ignore
}

bool Blowtorch::ShouldAmmoUnitsBeDrawn() const
{
  return active;
}

bool Blowtorch::IsPreventingJumps()
{
  return active;
}

void Blowtorch::Refresh()
{
  if (active) {
    const LRMoveIntention * lr_move_intention = ActiveCharacter().GetLastLRMoveIntention();
    if (lr_move_intention && EnoughAmmoUnit()) {
      Weapon::RepeatShoot();
    }
  }
  if (!EnoughAmmoUnit()) {
    active = false;
    if (EnoughAmmo()) {
      ActiveTeam().ResetNbUnits();
    }
  }
}

//-------------------------------------------------------------------------------------

BlowtorchConfig::BlowtorchConfig()
{
  range = 2;
}

BlowtorchConfig& Blowtorch::cfg()
{
  return static_cast<BlowtorchConfig&>(*extra_params);
}

void BlowtorchConfig::LoadXml(const xmlNode* elem)
{
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
}

std::string Blowtorch::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u blowtorch! If you're under 18, ask your parents to use it.",
            "%s team has won %u blowtorchs! If you're under 18, ask your parents to use it.",
            items_count), TeamName, items_count);
}
