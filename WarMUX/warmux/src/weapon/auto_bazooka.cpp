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
 * auto bazooka : launch a homing missile
 *****************************************************************************/

#include "character/character.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/game_msg.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/wind.h"
#include "network/network.h"
#include "object/objects_list.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/auto_bazooka.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"


class AutomaticBazookaConfig : public ExplosiveWeaponConfig
{
public:
  Double uncontrolled_turn_speed;
  Double max_controlled_turn_speed;
  Double fuel_time;
  Double rocket_force;
  AutomaticBazookaConfig();
  void LoadXml(const xmlNode* elem);
};

class RPG : public WeaponProjectile
{
  // Avoid dynamic_cast, hoping no race condition between WeaponProjectile::cfg and it
  AutomaticBazookaConfig& acfg;
  ParticleEngine smoke_engine;
  SoundSample flying_sound;
protected:
  Double m_initial_strength;
  Double angle_local;
  Point2i m_targetPoint;
  bool m_targeted;
  Double m_force;
  uint m_lastrefresh;
public:
  RPG(AutomaticBazookaConfig& cfg,
      WeaponLauncher * p_launcher);
  void Refresh();
  void Shoot(Double strength);
  void Explosion();
  void SetTarget (int x,int y);

protected:
  void SignalOutOfMap();
  void SignalDrowning();
};

RPG::RPG(AutomaticBazookaConfig& cfg, WeaponLauncher * p_launcher)
 : WeaponProjectile("rocket", cfg, p_launcher)
 , acfg(cfg)
 , smoke_engine(20)
 , m_lastrefresh(0)
{
  m_targeted = false;
  explode_colliding_character = true;
}

void RPG::Shoot(Double strength)
{
  m_initial_strength = strength;

  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  flying_sound.Play("default","weapon/automatic_rocket_flying", -1);

  WeaponProjectile::Shoot(strength);
  angle_local=ActiveCharacter().GetFiringAngle();
}

void RPG::Refresh()
{
  uint time = GameTime::GetInstance()->Read();
  Double flying_time = GetMSSinceTimeoutStart();
  uint timestep = time - m_lastrefresh;
  m_lastrefresh = time;
  if (!m_targeted) {
    // rocket is turning around herself
    angle_local += acfg.uncontrolled_turn_speed * timestep * (Double)0.001;
    if (angle_local > PI) angle_local = -PI;

    // TPS_AV_ATTIRANCE msec later being launched, the rocket is homing to the target
    if (flying_time > 1000 * GetTotalTimeout() * (m_initial_strength/ActiveTeam().AccessWeapon().max_strength)) {
      m_targeted = true;
      SetSpeed(0,0);
      angle_local = GetPosition().ComputeAngle(m_targetPoint);
      m_force = acfg.rocket_force;
      SetExternForce(m_force, angle_local);
      SetGravityFactor(0);
      SetWindFactor(0);
    }
  } else {
    SetExternForce(m_force, angle_local+HALF_PI); // reverse the force applyed on the last Refresh()

    if (flying_time - GetTotalTimeout() < acfg.fuel_time*1000) {
      smoke_engine.AddPeriodic(GetPosition() + (GetSize()>>1),
                               particle_DARK_SMOKE, false, -1, 2.0);
      Double wish_angle = GetPosition().ComputeAngle(m_targetPoint);
      Double max_rotation = abs(acfg.max_controlled_turn_speed * timestep * (Double)0.001);
      Double diff = fmod(wish_angle-angle_local, PI*TWO);
      if (diff < -PI) diff += TWO_PI;
      if (diff > PI) diff -= TWO_PI;
      //diff should now be between -PI and PI...
      if (diff > max_rotation) {
        angle_local += max_rotation;
      } else if (diff < -max_rotation) {
        angle_local -= max_rotation;
      } else {
        angle_local = wish_angle;
      }
      m_force = acfg.rocket_force * ((acfg.fuel_time* ((Double)1300) - flying_time + GetTotalTimeout())/acfg.fuel_time/(Double)1300);
      SetGravityFactor((flying_time - GetTotalTimeout())/acfg.fuel_time/(Double)1000); // slowly increase gravity
      SetWindFactor((flying_time - GetTotalTimeout())/acfg.fuel_time/(Double)1000); // slowly increase wind
    } else {
      SetGravityFactor(1);
      m_force = 0; //if there's no fuel left just let it crash into the ground somewhere
      if (!IsDrowned()) {
        angle_local += acfg.uncontrolled_turn_speed * timestep / (Double)1000;
        if (angle_local > PI) angle_local = - PI;
      } else {
        angle_local = HALF_PI;
      }
    }

    SetExternForce(m_force, angle_local);

  }
  image->SetRotation_rad(angle_local);
}

void RPG::SignalDrowning()
{
  smoke_engine.Stop();
  WeaponProjectile::SignalDrowning();

  flying_sound.Stop();
}

void RPG::SignalOutOfMap()
{
  Weapon::Message(_("The automatic rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();

  flying_sound.Stop();
}

// Set the coordinate of the target
void RPG::SetTarget(int x, int y)
{
  m_targetPoint.x = x;
  m_targetPoint.y = y;
}

void RPG::Explosion()
{
  WeaponProjectile::Explosion();

  flying_sound.Stop();
}

//-----------------------------------------------------------------------------

struct target_t
{
  Point2i pos;
  bool selected;
  Surface image;
};

AutomaticBazooka::AutomaticBazooka() :
  WeaponLauncher(WEAPON_AUTOMATIC_BAZOOKA, "automatic_bazooka", new AutomaticBazookaConfig())
{
  UpdateTranslationStrings();

  m_category = HEAVY;
  mouse_character_selection = false;
  m_allow_change_timeout = true;
  m_target = new target_t;
  m_target->selected = false;
  m_target->image = GetResourceManager().LoadImage(weapons_res_profile, "baz_cible");
  ReloadLauncher();
}

void AutomaticBazooka::UpdateTranslationStrings()
{
  m_name = _("Automatic Bazooka");
  m_help = _("Usage: left click on the target\nInitial fire angle: Up/Down\n"
             "Firing: keep the space key pressed until the desired strength\nOne ammo per turn");
}

AutomaticBazooka::~AutomaticBazooka()
{
  if (m_target)
    delete m_target;
}

WeaponProjectile * AutomaticBazooka::GetProjectileInstance()
{
  return new RPG(cfg(), this);
}

void AutomaticBazooka::Draw()
{
  WeaponLauncher::Draw();
  DrawTarget();
}

void AutomaticBazooka::Refresh()
{
  DrawTarget();
  WeaponLauncher::Refresh();
}

void AutomaticBazooka::p_Select()
{
  WeaponLauncher::p_Select();
  m_target->selected = false;

  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_AIM);
}

void AutomaticBazooka::p_Deselect()
{
  WeaponLauncher::p_Deselect();
  if (m_target->selected) {
    // need to clear the old target
    GetWorld().ToRedrawOnMap(Rectanglei(m_target->pos - (m_target->image.GetSize()>>1),
                                        m_target->image.GetSize()));
  }
}

void AutomaticBazooka::ChooseTarget(Point2i mouse_pos)
{
  if (m_target->selected) {
    // need to clear the old target
    GetWorld().ToRedrawOnMap(Rectanglei(m_target->pos - (m_target->image.GetSize()>>1),
                                        m_target->image.GetSize()));
  }

  m_target->pos = mouse_pos;
  m_target->selected = true;

  if (!ActiveTeam().IsLocalHuman())
    Camera::GetInstance()->SetXYabs(mouse_pos - Camera::GetInstance()->GetSize()/2);
  DrawTarget();
  static_cast<RPG *>(projectile)->SetTarget(m_target->pos.x, m_target->pos.y);
}

void AutomaticBazooka::DrawTarget() const
{
  if (!m_target->selected) return;

  GetMainWindow().Blit(m_target->image,
                       m_target->pos - (m_target->image.GetSize()>>1)
                       - Camera::GetInstance()->GetPosition());

  GetWorld().ToRedrawOnMap(Rectanglei(m_target->pos - (m_target->image.GetSize()>>1),
                                      m_target->image.GetSize()));
}

bool AutomaticBazooka::IsReady() const
{
  return (EnoughAmmo() && m_target->selected);
}

AutomaticBazookaConfig &AutomaticBazooka::cfg()
{
  return static_cast<AutomaticBazookaConfig &>(*extra_params);
}

AutomaticBazookaConfig::AutomaticBazookaConfig()
{
  uncontrolled_turn_speed = PI*8;
  max_controlled_turn_speed = PI*4;
  fuel_time = 10;
  rocket_force = 2500;
}

void AutomaticBazookaConfig::LoadXml(const xmlNode* elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  XmlReader::ReadDouble(elem, "uncontrolled_turn_speed", uncontrolled_turn_speed);
  XmlReader::ReadDouble(elem, "max_controlled_turn_speed", max_controlled_turn_speed);
  XmlReader::ReadDouble(elem, "fuel_time", fuel_time);
  XmlReader::ReadDouble(elem, "rocket_force", rocket_force);
}

std::string AutomaticBazooka::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext("%s team has won %u automatic bazooka!",
                         "%s team has won %u automatic bazookas!",
                         items_count), TeamName, items_count);
}
