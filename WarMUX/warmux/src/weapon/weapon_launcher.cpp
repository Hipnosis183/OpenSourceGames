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
 * WeaponLauncher: generic weapon to launch a projectile
 *****************************************************************************/

#include <sstream>
#include <WARMUX_debug.h>
#include <limits.h>

#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"
#include "weapon/weapon_launcher.h"

#ifdef DEBUG
//#define DEBUG_EXPLOSION_CONFIG
#include "graphic/video.h"
#include "include/app.h"
#endif

const uint INVALID_TIMEOUT_START = UINT_MAX;

WeaponBullet::WeaponBullet(const std::string &name,
                           ExplosiveWeaponConfig& cfg,
                           WeaponLauncher * p_launcher) :
  WeaponProjectile(name, cfg, p_launcher)
{
  explode_colliding_character = true;
  m_allow_negative_y = false;
  ResetTimeOut();
}

// Signal that the bullet has hit the ground
void WeaponBullet::SignalGroundCollision(const Point2d& speed_before, const Double& contactAngle)
{
  JukeBox::GetInstance()->Play("default", "weapon/ricoche1");
  WeaponProjectile::SignalGroundCollision(speed_before, contactAngle);
  launcher->IncMissedShots();
}

void WeaponBullet::SignalOutOfMap()
{
  WeaponProjectile::SignalOutOfMap();
  launcher->IncMissedShots();
  Camera::GetInstance()->FollowObject(&ActiveCharacter());
}

void WeaponBullet::SignalObjectCollision(const Point2d& my_speed_before,
                                         PhysicalObj * obj,
                                         const Point2d& /*obj_speed*/)
{
#if 1
  if (cfg.speed_on_hit.IsNotZero()) {
    Double angle = my_speed_before.ComputeAngle();
    if (angle > ZERO && angle <= HALF_PI)        angle -= ONE_HALF * QUARTER_PI;
    else if (angle > HALF_PI && angle <= PI)     angle += ONE_HALF * QUARTER_PI;
    else if (angle <= ZERO && angle >= -HALF_PI) angle -= ONE_HALF * QUARTER_PI;
    else if (angle < -HALF_PI && angle >= -PI)   angle += ONE_HALF * QUARTER_PI;

    obj->AddSpeed(cfg.speed_on_hit, angle);
  }
#else
  // multiply by ten to get something more funny
  Double bullet_mass = GetMass()/* * 10*/;
  Double total_mass = bullet_mass + obj->GetMass();
  // computing new speed of character
  Point2d v2 = (my_speed_before * (1 + 0.8) * bullet_mass +
                obj->GetSpeed() * (obj->GetMass() - 0.8 * bullet_mass)) / total_mass;
  // Pushing a little upward character to allow him to be pushed by the projectile
  obj->SetXY(Point2i(obj->GetX(), obj->GetY() - 3));
  obj->SetSpeedXY(v2);
#endif

  obj->SetEnergyDelta(-(int)cfg.damage, &ActiveCharacter());
  if (!obj->IsCharacter())
    Explosion();
  Ghost();

  WeaponProjectile::Collision();
}

void WeaponBullet::Refresh()
{
  WeaponProjectile::Refresh();
  image->SetRotation_rad(GetSpeedAngle());
}

void WeaponBullet::DoExplosion()
{
  Point2i pos = GetCenter();
  ApplyExplosion(pos, cfg, "", false, ParticleEngine::LittleESmoke);
}
//-----------------------------------------------------------------------------


WeaponProjectile::WeaponProjectile(const std::string &name,
                                   ExplosiveWeaponConfig& p_cfg,
                                   WeaponLauncher * p_launcher):
  PhysicalObj(name),
  timeout_start(INVALID_TIMEOUT_START),
  cfg(p_cfg)
{
  m_allow_negative_y = true;
  SetCollisionModel(true, true, true);
  launcher = p_launcher;

  explode_colliding_character = false;
  // explode_with_timeout defaults to false as
  // the default value true has caused a few hard to find bugs.
  explode_with_timeout = false;
  explode_with_collision = true;
  can_drown = true;
  camera_follow_closely = false;

  image = GetResourceManager().LoadSprite(weapons_res_profile, name);
  SetSize(image->GetSize());

  // Set rectangle test
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect(dx, dx, dy, dy);

  ResetTimeOut();

  // generate a unique id for the projectile
  m_unique_id = name + Game::GetUniqueId();
}

WeaponProjectile::~WeaponProjectile()
{
  delete image;
}

void WeaponProjectile::Shoot(Double strength)
{
  MSG_DEBUG("weapon.projectile", "shoot with strength:%s", Double2str(strength).c_str());

  Init();

  if (launcher != NULL)
    launcher->IncActiveProjectile();

  // Set the physical factors
  ResetConstants();

  // Set the initial position.
  SetOverlappingObject(&ActiveCharacter(), 100);
  ObjectsList::GetRef().AddObject(this);

  Double angle = ActiveCharacter().GetFiringAngle();
  RandomizeShoot(angle, strength);

  Point2i hand_position;
  ActiveCharacter().GetHandPosition(hand_position);
  MSG_DEBUG("weapon.projectile", "shoot from position %d,%d (size %d, %d) - hand position:%d,%d",
            ActiveCharacter().GetX(), ActiveCharacter().GetY(),
            ActiveCharacter().GetWidth(), ActiveCharacter().GetHeight(),
            hand_position.GetX(), hand_position.GetY());

  MSG_DEBUG("weapon.projectile", "shoot with strength:%.1f, angle:%.1f, position:%d,%d",
            strength.tofloat(), angle.tofloat(), GetX(), GetY());

  StartTimeout();

  ShootSound();

#if 0
  // bug #10236 : problem with flamethrower collision detection
  // Check if the object is colliding something between hand position and gun hole
  hand_position -= GetSize() / 2;
  Point2i hole_position = launcher->GetGunHolePosition() - GetSize() / 2;
  Point2d f_hand_position = hand_position * METER_PER_PIXEL;
  Point2d f_hole_position = hole_position * METER_PER_PIXEL;
  SetXY(hand_position);
  SetSpeed(strength, angle);

  // Camera::FollowObject must be called after setting initial speed else
  // camera_follow_closely will have no effect
  Camera::GetInstance()->FollowObject(this, camera_follow_closely);

  collision_t collision = NotifyMove(f_hand_position, f_hole_position);
  if (collision == NO_COLLISION) {
    // Set the initial position and speed.
    SetXY(hole_position);
    SetSpeed(strength, angle);
    PutOutOfGround(angle);
  }
#else
  Point2i hole_position = launcher->GetGunHolePosition() - GetSize() / 2;
  Point2d f_hole_position = hole_position * METER_PER_PIXEL;
  SetXY(hole_position);
  PutOutOfGround(angle);
  SetSpeed(strength, angle);

  // Camera::FollowObject must be called after setting initial speed else
  // camera_follow_closely will have no effect
  Camera::GetInstance()->FollowObject(this, camera_follow_closely);
  //PutOutOfGround(angle);
#endif
}

void WeaponProjectile::ShootSound()
{
  JukeBox::GetInstance()->Play(ActiveTeam().GetSoundProfile(), "fire");
}

void WeaponProjectile::Refresh()
{
  if (m_energy == 0) {
    Explosion();
    return;
  }
  // The RefreshSurface() call is necessary as the image determines the size of the projectile
  // and we don't want to rely on Draw to call RefreshSurface() as the draw rate differs
  // for the players in a multiplayer game.
  image->RefreshSurface();
  SetSize(image->GetSizeMax());

  if (cfg.timeout) {
     if (((int)GetMSSinceTimeoutStart()) > 1000 * GetTotalTimeout())
       SignalTimeout();
  }
}

void WeaponProjectile::SetEnergyDelta(int /*delta*/, bool /*do_report*/)
{
  // Don't call Explosion here, we're already in an explosion
  m_energy = 0;
}

void WeaponProjectile::Draw()
{
  image->Draw(GetPosition());

  int tmp = GetTotalTimeout();

  if (cfg.timeout && tmp != 0)
  {
    tmp -= (int)((GetMSSinceTimeoutStart()) / 1000);
    if (tmp >= 0)
    {
      std::ostringstream ss;
      ss << tmp ;
      int txt_x = GetX() + GetWidth() / 2;
      int txt_y = GetY() - GetHeight();
          Text text(ss.str());
      text.DrawCenterTop(Point2i(txt_x, txt_y) - Camera::GetInstance()->GetPosition());
    }
  }

#ifdef DEBUG
  if (IsLOGGING("test_rectangle"))
  {
    Rectanglei test_rect(GetTestRect());
    test_rect.SetPosition(test_rect.GetPosition() - Camera::GetInstance()->GetPosition());
    GetMainWindow().RectangleColor(test_rect, primary_red_color, 1);

    Rectanglei rect(GetPosition() - Camera::GetInstance()->GetPosition(), image->GetSizeMax());
    GetMainWindow().RectangleColor(rect, primary_blue_color, 1);
  }
#endif
}

bool WeaponProjectile::IsImmobile() const
{
  if (explode_with_timeout && GetTotalTimeout() * 1000 > (int) GetMSSinceTimeoutStart())
    return false;
  return PhysicalObj::IsImmobile();
}

// projectile explode and signal to the launcher the collision
void WeaponProjectile::SignalObjectCollision(const Point2d& /* my_speed_before */,
                                             PhysicalObj * obj,
                                             const Point2d& /* obj_speed_before */)
{
  ASSERT(obj != NULL);
  MSG_DEBUG("weapon.projectile", "SignalObjectCollision \"%s\" with \"%s\": %d, %d",
            m_name.c_str(), obj->GetName().c_str(), GetX(), GetY());
  if (explode_colliding_character)
    Explosion();

  Collision();
}

// projectile explode when hiting the ground
void WeaponProjectile::SignalGroundCollision(const Point2d& /*speed_before*/, const Double& /*contactAngle*/)
{
  MSG_DEBUG("weapon.projectile", "SignalGroundCollision \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  if (explode_with_collision)
    Explosion();

  Collision();
}

// Signal to launcher a collision
void WeaponProjectile::Collision()
{
  MSG_DEBUG("weapon.projectile", "Collision \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  if (launcher != NULL && !launcher->ignore_collision_signal)
    launcher->SignalProjectileCollision();
}


// Default behavior : signal to launcher projectile is drowning
void WeaponProjectile::SignalDrowning()
{
  MSG_DEBUG("weapon.projectile", "SignalDrowning \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  PhysicalObj::SignalDrowning();
  if (launcher != NULL && !launcher->ignore_drowning_signal)
    launcher->SignalProjectileDrowning();

  if (can_drown) JukeBox::GetInstance()->Play("default", "sink");
  //else JukeBox::GetInstance()->Play("default", "pschiiit");
}

// Default behavior : signal to launcher a projectile is going out of water
void WeaponProjectile::SignalGoingOutOfWater()
{
  MSG_DEBUG("weapon.projectile", "SignalDrowning \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  PhysicalObj::SignalGoingOutOfWater();
  if (launcher != NULL && !launcher->ignore_going_out_of_water_signal)
    launcher->SignalProjectileGoingOutOfWater();
}

// Signal a ghost state
void WeaponProjectile::SignalGhostState(bool)
{
  MSG_DEBUG("weapon.projectile", "SignalGhostState \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  if (launcher != NULL && !launcher->ignore_ghost_state_signal)
    launcher->SignalProjectileGhostState();
}

void WeaponProjectile::SignalOutOfMap()
{
  MSG_DEBUG("weapon.projectile", "SignalOutOfMap \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
}

// the projectile explode and signal the explosion to launcher
void WeaponProjectile::Explosion()
{
  MSG_DEBUG("weapon.projectile", "Explosion \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  DoExplosion();
  SignalExplosion();
  Ghost();
}

void WeaponProjectile::SignalExplosion()
{
  MSG_DEBUG("weapon.projectile", "SignalExplosion \"%s\": %d, %d", m_name.c_str(), GetX(), GetY());
  if (launcher != NULL && !launcher->ignore_explosion_signal)
    launcher->SignalProjectileExplosion();
}

void WeaponProjectile::DoExplosion()
{
  Point2i pos = GetCenter();
  ApplyExplosion(pos, cfg, "weapon/explosion", true, ParticleEngine::BigESmoke);
}

void WeaponProjectile::IncrementTimeOut()
{
  if (cfg.allow_change_timeout && GetTotalTimeout()<(int)cfg.timeout*2)
    m_timeout_modifier += 1 ;
}

void WeaponProjectile::DecrementTimeOut()
{
  // -1s for grenade timout. 1 is min.
  if (cfg.allow_change_timeout && GetTotalTimeout()>1)
    m_timeout_modifier -= 1 ;
}

void WeaponProjectile::SetTimeOut(int timeout)
{
  if (cfg.allow_change_timeout && timeout <= (int)cfg.timeout*2 && timeout >= 1)
    m_timeout_modifier = timeout - cfg.timeout ;
}

int WeaponProjectile::GetTotalTimeout() const
{
  return (int)(cfg.timeout)+m_timeout_modifier;
}

void WeaponProjectile::StartTimeout()
{
  timeout_start = GameTime::GetInstance()->Read();
}

uint WeaponProjectile::GetMSSinceTimeoutStart() const
{
  uint now = GameTime::GetInstance()->Read();
  ASSERT (timeout_start  != INVALID_TIMEOUT_START);
  ASSERT(now >= timeout_start);
  return now - timeout_start;
}

// Signal a projectile timeout and explode
void WeaponProjectile::SignalTimeout()
{
  MSG_DEBUG("weapon.projectile", "\"%s\" timeout has expired", m_name.c_str());
  if (launcher != NULL && !launcher->ignore_timeout_signal)
    launcher->SignalProjectileTimeout();
  if (explode_with_timeout)
    Explosion();
}

//Public function which let know if changing timeout is allowed.
bool WeaponProjectile::change_timeout_allowed() const
{
  return cfg.allow_change_timeout;
}

//-----------------------------------------------------------------------------

WeaponLauncher::WeaponLauncher(Weapon_type type,
                               const std::string &id,
                               EmptyWeaponConfig * params,
                               bool drawable) :
    Weapon(type, id, params, drawable)
{
  projectile = NULL;
  nb_active_projectile = 0;
  missed_shots = 0;
  announce_missed_shots = true;
  ignore_timeout_signal = false;
  ignore_collision_signal = false;
  ignore_explosion_signal = false;
  ignore_ghost_state_signal = false;
  ignore_drowning_signal = false;
  ignore_going_out_of_water_signal = false;
}

WeaponLauncher::~WeaponLauncher()
{
  if (projectile)
    delete projectile;
}

int WeaponLauncher::GetDamage() const
{
  return cfg().damage;
}


bool WeaponLauncher::p_Shoot()
{
//   if (m_strength == max_strength)
//   {
//     m_strength = 0;
//     DirectExplosion();
//     return true;
//   }
  projectile->Shoot(m_strength);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

bool WeaponLauncher::ReloadLauncher()
{
  if (projectile) {
    return false;
  }
  projectile = GetProjectileInstance();
  return true;
}

// Direct Explosion when pushing weapon to max power !
void WeaponLauncher::DirectExplosion()
{
  Point2i pos = ActiveCharacter().GetCenter();
  ApplyExplosion(pos, cfg());
}

void WeaponLauncher::Draw()
{
  //Display timeout for projectil if can be changed.
  if (projectile->change_timeout_allowed()) {
    if (IsOnCooldownFromShot()) //Do not display after launching.
      return;

    int tmp = projectile->GetTotalTimeout();
    std::ostringstream ss;
    ss << tmp;
    ss << "s";
    int txt_x = ActiveCharacter().GetX() + ActiveCharacter().GetWidth() / 2;
    int txt_y = ActiveCharacter().GetY() - 4*ActiveCharacter().GetHeight()/5;
    Text text(ss.str());
    text.DrawCenterTop(Point2i(txt_x, txt_y) - Camera::GetInstance()->GetPosition());
  }

  Weapon::Draw();

#ifdef DEBUG_EXPLOSION_CONFIG
  ExplosiveWeaponConfig* cfg = dynamic_cast<ExplosiveWeaponConfig*>(extra_params);
  if (cfg) {
    Point2i p = ActiveCharacter().GetHandPosition() - Camera::GetInstance()->GetPosition();
    // Red color for the blast range (should be superior to the explosion_range)
    GetMainWindow().CircleColor(p.x, p.y, (int)cfg->blast_range, c_red);
    // Yellow color for the blast range (should be superior to the explosion_range)
    GetMainWindow().CircleColor(p.x, p.y, (int)cfg->explosion_range, c_black);
  }
  GetMainWindow().CircleColor(GetGunHolePosition().x-Camera::GetInstance()->GetPositionX(),
                              GetGunHolePosition().y-Camera::GetInstance()->GetPositionY(), 5, c_black);
#endif
}

void WeaponLauncher::p_Select()
{
  missed_shots = 0;
  if (projectile->change_timeout_allowed()) {
    projectile->ResetTimeOut();
  }
  Weapon::p_Select();
}

void WeaponLauncher::IncMissedShots()
{
  missed_shots++;
  if (announce_missed_shots)
    GameMessages::GetInstance()->Add(_("Your shot has missed!"), ActiveTeam().GetColor());
}

void WeaponLauncher::SetTimeoutForAllPlayers(int timeout)
{
  Action * a = new Action(Action::ACTION_WEAPON_SET_TIMEOUT, timeout);
  ActionHandler::GetInstance()->NewAction(a);
}

ExplosiveWeaponConfig& WeaponLauncher::cfg() const
{
  return static_cast<ExplosiveWeaponConfig&>(*extra_params);
}
