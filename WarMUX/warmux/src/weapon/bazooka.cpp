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
 * Bazooka, a simple rocket launcher
 *****************************************************************************/

#include "weapon/bazooka.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"
#include "game/config.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"

class BazookaRocket : public WeaponProjectile
{
  ParticleEngine smoke_engine;
  SoundSample flying_sound;
public:
  BazookaRocket(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher);
  void Refresh();
  void Explosion();
  void Shoot(Double strength);
protected:
  void SignalOutOfMap();
  void SignalDrowning();
};

BazookaRocket::BazookaRocket(ExplosiveWeaponConfig& cfg,
                             WeaponLauncher * p_launcher) :
  WeaponProjectile ("rocket", cfg,p_launcher), smoke_engine(20)
{
  explode_colliding_character = true;
}

void BazookaRocket::Refresh()
{
  WeaponProjectile::Refresh();
  if (!IsDrowned()) {
    image->SetRotation_rad(GetSpeedAngle());
    smoke_engine.AddPeriodic(GetPosition() + (GetSize()>>1),
                             particle_DARK_SMOKE, false, -1, 2.0);
  }
  else
  {
    image->SetRotation_rad(HALF_PI);
  }
}

void BazookaRocket::Shoot(Double strength)
{
  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  flying_sound.Play("default","weapon/rocket_flying", -1);

  WeaponProjectile::Shoot(strength);
}

void BazookaRocket::SignalOutOfMap()
{
  Weapon::Message(_("The rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();

  flying_sound.Stop();
}

void BazookaRocket::SignalDrowning()
{
  smoke_engine.Stop();
  WeaponProjectile::SignalDrowning();

  flying_sound.Stop();
}

void BazookaRocket::Explosion()
{
  WeaponProjectile::Explosion();

  flying_sound.Stop();
}

//-----------------------------------------------------------------------------

Bazooka::Bazooka() :
  WeaponLauncher(WEAPON_BAZOOKA, "bazooka", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = HEAVY;
  ReloadLauncher();
}

WeaponProjectile * Bazooka::GetProjectileInstance()
{
  return new BazookaRocket(cfg(), this);
}

void Bazooka::UpdateTranslationStrings()
{
  m_name = _("Bazooka");
  m_help = _("Initial fire angle: Up/Down\nFire: Press space until desired strength is reached\nOne ammo per turn");
}

std::string Bazooka::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u bazooka!",
            "%s team has won %u bazookas!",
            items_count), TeamName, items_count);
}

