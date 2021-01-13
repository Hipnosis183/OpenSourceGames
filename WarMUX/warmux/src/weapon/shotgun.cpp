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
 * Shotgun. Shoot a bunch of buckshot at each fire
 *****************************************************************************/

#include "weapon/explosion.h"
#include "weapon/shotgun.h"
#include "weapon/weapon_cfg.h"

#include <sstream>
#include "map/map.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "object/objects_list.h"
#include "interface/game_msg.h"
#include "network/randomsync.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"

const uint   SHOTGUN_BUCKSHOT_SPEED  = 30;
const Double SHOTGUN_RANDOM_ANGLE    = 0.04;
const Double SHOTGUN_RANDOM_STRENGTH = 2.0;

class ShotgunBuckshot : public WeaponBullet
{
  public:
    ShotgunBuckshot(ExplosiveWeaponConfig& cfg,
                    WeaponLauncher * p_launcher);
    bool IsOverlapping(const PhysicalObj* obj) const;
  protected:
    void RandomizeShoot(Double &angle,Double &strength);
};


ShotgunBuckshot::ShotgunBuckshot(ExplosiveWeaponConfig& cfg,
                                 WeaponLauncher * p_launcher) :
  WeaponBullet("buckshot", cfg, p_launcher)
{
}

void ShotgunBuckshot::RandomizeShoot(Double &angle,Double &strength)
{
  angle += PI * RandomSync().GetDouble(-SHOTGUN_RANDOM_ANGLE,SHOTGUN_RANDOM_ANGLE);
  strength += RandomSync().GetDouble(-SHOTGUN_RANDOM_STRENGTH,SHOTGUN_RANDOM_STRENGTH);
}

bool ShotgunBuckshot::IsOverlapping(const PhysicalObj* obj) const
{
  if(GetName() == obj->GetName()) return true;
  return m_overlapping_object == obj;
}

//-----------------------------------------------------------------------------

Shotgun::Shotgun() : WeaponLauncher(WEAPON_SHOTGUN, "shotgun", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = RIFLE;

  announce_missed_shots = false;
  m_weapon_fire = new Sprite(GetResourceManager().LoadImage(weapons_res_profile,m_id+"_fire"));

  ReloadLauncher();
}

void Shotgun::UpdateTranslationStrings()
{
  m_name = _("Shotgun");
  m_help = _("Press space to shoot\nUse Up/Down to change initial angle");
}

// Return a buckshot instance for the shotgun
WeaponProjectile * Shotgun::GetProjectileInstance()
{
  return new ShotgunBuckshot(cfg(), this);
}

void Shotgun::ShootSound() const
{
  JukeBox::GetInstance()->Play("default", "weapon/shotgun");
}

void Shotgun::IncMissedShots()
{
  if(missed_shots + 1 == SHOTGUN_BULLETS)
    announce_missed_shots = true;
  WeaponLauncher::IncMissedShots();
}

bool Shotgun::p_Shoot ()
{
  missed_shots = 0;
  announce_missed_shots = false;
  if (IsOnCooldownFromShot())
    return false;

  for(int i = 0; i < SHOTGUN_BULLETS; i++) {
    projectile->Shoot(SHOTGUN_BUCKSHOT_SPEED);
    projectile = NULL;
    ReloadLauncher();
  }
  ShootSound();
  return true;
}

std::string Shotgun::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u shotgun!",
            "%s team has won %u shotguns!",
            items_count), TeamName, items_count);
}

