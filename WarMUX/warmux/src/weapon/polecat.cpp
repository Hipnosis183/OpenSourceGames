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
 * Polecat : send a polecat to the enemy. Close character get sick with the mefitic odor.
 *****************************************************************************/

#include "character/character.h"
#include "game/config.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "weapon/explosion.h"
#include "weapon/polecat.h"
#include "weapon/weapon_cfg.h"

const uint TIME_BETWEEN_FART = 500;
const uint TIME_BETWEEN_REBOUND = 400;

class Polecat : public WeaponProjectile
{
 private:
  int m_sens;
  int save_x, save_y;
  uint last_fart_time;
  uint last_rebound_time;
  Double angle;
  void Fart();
 protected:
  void SignalOutOfMap();
 public:
  Polecat(ExplosiveWeaponConfig& cfg,
          WeaponLauncher * p_launcher);
  void Shoot(Double strength);
  void Refresh();

  virtual void Explosion();
};


Polecat::Polecat(ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile("polecat", cfg, p_launcher)
{
  explode_with_collision = false;
  explode_with_timeout = true;
  last_fart_time = 0;
  last_rebound_time = 0;
  // Make sure the flip cache is set
  image->EnableCaches(true, 0);
}

void Polecat::Shoot(Double strength)
{
  WeaponProjectile::Shoot(strength);

  save_x=GetX();
  save_y=GetY();

  Double angle = ActiveCharacter().GetFiringAngle();

  if (angle<HALF_PI && angle>-HALF_PI)
    m_sens = 1;
  else
    m_sens = -1;
}

void Polecat::Fart()
{
  // particles must be exactly the same accross the network
  Double norme = Double(RandomSync().GetInt(0, 500))/100;
  Double angle = Double(RandomSync().GetInt(0, 3000))/100;
  ParticleEngine::AddNow(GetPosition(), 3, particle_POLECAT_FART, true, angle, norme);
  last_fart_time = GameTime::GetInstance()->Read();
  JukeBox::GetInstance()->Play("default", "weapon/polecat_fart");
}

void Polecat::Explosion()
{
  if (!last_fart_time) {
    Fart();
    return;
  }

  WeaponProjectile::Explosion();
}

void Polecat::Refresh()
{
  if (m_energy == 0) {
    Explosion();
    return;
  }
  int tmp = GetMSSinceTimeoutStart();
  if (cfg.timeout && tmp > 1000 * (GetTotalTimeout())) {
    if (!last_fart_time) {
      std::string txt = Format(_("%s has done something for the environment, he has not ordered the polecat to fart."),
             ActiveCharacter().GetName().c_str());
      Weapon::Message(txt);
    }
    SignalTimeout();
  }

  Double norm, angle;
  if (last_fart_time && last_fart_time + TIME_BETWEEN_FART < GameTime::GetInstance()->Read()) {
    Fart();
  }

  //When we hit the ground, jump !
  if(!IsMoving() && !FootsInVacuum()) {
    // Limiting number of rebound to avoid desync
    if(last_rebound_time + TIME_BETWEEN_REBOUND > GameTime::GetInstance()->Read()) {
      image->SetRotation_rad(0.0);
      return;
    }
    last_rebound_time = GameTime::GetInstance()->Read();
    MSG_DEBUG("weapon.polecat", "Jump ! (time = %d)", last_rebound_time);
    //If the GNU is stuck in ground -> change direction
    int x = GetX();
    int y = GetY();
    if(x == save_x && y == save_y)
      m_sens = -m_sens;
    save_x = x;
    save_y = y;

    //Do the jump
    norm = RandomSync().GetDouble(1.0, 2.0);
    PutOutOfGround();
    SetSpeedXY(Point2d(m_sens * norm , -norm * THREE));
  }
  //Due to a bug in the physic engine
  //sometimes, angle==infinite (according to gdb) ??
  GetSpeed(norm, angle);

  angle = RestrictAngle(angle) * ONE_HALF;
  bool flipped = m_sens == -1;
  if (flipped) {
    if (angle > 0)
      angle -= HALF_PI;
    else
      angle += HALF_PI;
  }

  image->SetRotation_rad(angle);
  image->SetFlipped(flipped);
  image->Scale(ONE, ONE);
  image->Update();
}

void Polecat::SignalOutOfMap()
{
  Weapon::Message(_("The Polecat left the battlefield before exploding!"));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

PolecatLauncher::PolecatLauncher() :
  WeaponLauncher(WEAPON_POLECAT, "polecatlauncher", new ExplosiveWeaponConfig()),
  current_polecat(NULL),
  polecat_death_time(0)
{
  UpdateTranslationStrings();

  m_category = SPECIAL;
  ReloadLauncher();

  // unit will be used when the polecat disappears
  use_unit_on_first_shoot = false;
}

void PolecatLauncher::UpdateTranslationStrings()
{
  m_name = _("Polecat Launcher");
  m_help = _("Set timer 1-6 using +/- or 1-6 keys\nPress space to shoot\nOne shoot per turn");
}

bool PolecatLauncher::p_Shoot()
{
  if (current_polecat || polecat_death_time)
    return false;

  current_polecat = static_cast<Polecat *>(projectile);
  polecat_death_time = 0;
  bool r = WeaponLauncher::p_Shoot();
  return r;
}

void PolecatLauncher::Refresh()
{
  WeaponLauncher::Refresh();
  if (current_polecat)
    return;

  if (polecat_death_time && polecat_death_time + 2000 < GameTime::GetInstance()->Read()) {

    UseAmmoUnit();
    polecat_death_time = 0;
  }
}

bool PolecatLauncher::IsOnCooldownFromShot() const
{
  return (current_polecat || polecat_death_time);
}

bool PolecatLauncher::IsReady() const
{
  return !IsOnCooldownFromShot() && WeaponLauncher::IsReady();
}

void PolecatLauncher::StopShooting()
{
  if (current_polecat) {
    current_polecat->Explosion();
    return;
  }
  WeaponLauncher::StopShooting();
}

bool PolecatLauncher::IsPreventingLRMovement()
{
  return (current_polecat || polecat_death_time);
}

bool PolecatLauncher::IsPreventingJumps()
{
  return (current_polecat || polecat_death_time);
}

bool PolecatLauncher::IsPreventingWeaponAngleChanges()
{
  return (current_polecat || polecat_death_time);
}

void PolecatLauncher::SignalEndOfProjectile()
{
  if (!current_polecat)
    return;

  current_polecat = NULL;
  polecat_death_time = GameTime::GetInstance()->Read();
}

WeaponProjectile * PolecatLauncher::GetProjectileInstance()
{
  return new Polecat(cfg(), this);
}

std::string PolecatLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u polecat! You have your gas mask, right?",
            "%s team has won %u polecats! You have your gas mask, right?",
            items_count), TeamName, items_count);
}


