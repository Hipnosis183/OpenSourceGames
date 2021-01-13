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
 * Teleportation : move a charecter anywhere on the map
 *****************************************************************************/

#include "character/character.h"
#include "character/body.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "include/action_handler.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/network.h"
#include "particles/teleport_member.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "weapon/teleportation.h"

Teleportation::Teleportation() :
  Weapon(WEAPON_TELEPORTATION, "teleportation", new WeaponConfig()),
  done(false)
{
  UpdateTranslationStrings();

  m_category = MOVE;
  target_chosen = false;
  // teleportation_anim_duration is declare in particles/teleport_member.h
  m_time_between_each_shot = TELEPORTATION_ANIM_DURATION + 100;
}

void Teleportation::UpdateTranslationStrings()
{
  m_name = _("Teleportation");
  m_help = _("Click onto map to teleport yourself\nto a place of your choice.");
}

bool Teleportation::p_Shoot ()
{
  Character& achar = ActiveCharacter();
  // Check we are not going outside of the world !
  if (achar.IsOutsideWorldXY(dst))
    return false;

  Rectanglei rect = achar.GetTestRect();
  rect.SetPosition(dst);

  // Go back to default cursor
  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  //  Game::GetInstance()->interaction_enabled = false;

  JukeBox::GetInstance()->Play("default", "weapon/teleport_start");

  achar.Hide();
  achar.body->MakeTeleportParticles(achar.GetPosition(), dst);
  Camera::GetInstance()->SetAutoCrop(false);
  return true;
}

void Teleportation::Refresh()
{
  if (!target_chosen)
    return;
  if (done)
    return;
  if (GameTime::GetInstance()->Read() - m_last_fire_time > TELEPORTATION_ANIM_DURATION) {
    Character& achar = ActiveCharacter();
    achar.SetXY(dst);
    achar.SetSpeed(ZERO, ZERO);
    achar.Show();
    JukeBox::GetInstance()->Play("default", "weapon/teleport_end");
    Camera::GetInstance()->SetAutoCrop(true);
    done = true;
  }
}

bool Teleportation::ShouldBeVisible()
{
  return !IsOnCooldownFromShot();
}

void Teleportation::p_Select()
{
  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_AIM);

  Weapon::p_Select();
  target_chosen = false;
  done = false;
}

void Teleportation::ChooseTarget(Point2i mouse_pos)
{
  if (target_chosen)
    return;

  Character& achar = ActiveCharacter();
  dst = mouse_pos - achar.GetSize()/2;
  if (!GetWorld().ParanoiacRectIsInVacuum(Rectanglei(dst, achar.GetSize())) ||
      !achar.IsInVacuumXY(dst))
    return;
  target_chosen = true;
  Shoot();
}

std::string Teleportation::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext("%s team has won %u teleportation!",
                         "%s team has won %u teleportations!",
                         items_count), TeamName, items_count);
}

WeaponConfig& Teleportation::cfg()
{
  return static_cast<WeaponConfig&>(*extra_params);
}
