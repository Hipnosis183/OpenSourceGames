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
 * Cluzooka: launches a rocket, which works like cluster bomb
 *****************************************************************************/

#include "weapon/cluzooka.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"
#include "game/game_time.h"
#include "game/config.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "sound/jukebox.h"
#include "network/randomsync.h"
#include "tool/xml_document.h"

// define this if you want multiple spawns (can be slow)
//#define CLUSTERS_SPAWN_CLUSTERS

template< typename ClusterType >
class Spawner
{
protected:
  bool m_spawned_clusters;

  Spawner() : m_spawned_clusters(false) {};
  virtual ~Spawner() {};

  virtual void Spawn(uint fragments, uint recursion_depth,
                     const Point2i& pos, const Double& angle, const Double& angle_range,
                     ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher)
  {
#ifndef CLUSTERS_SPAWN_CLUSTERS
    ASSERT(recursion_depth == 0);
#endif

    // fake explosion
    JukeBox::GetInstance()->Play( "default", "weapon/cluzooka_shot" );
    ParticleEngine::AddExplosionSmoke(pos, 50, ParticleEngine::LittleESmoke);

    ClusterType * cluster;

    for (uint i = 0; i < fragments; ++i) {
      Double cluster_deviation = (angle_range * i)/fragments - angle_range*ONE_HALF;

      cluster = new ClusterType(cfg, p_launcher);
      cluster->Shoot(pos, 25, angle+cluster_deviation, recursion_depth);

      ObjectsList::GetRef().AddObject(cluster);
    }
    m_spawned_clusters = true;
  }
};

class CluzookaConfig : public ExplosiveWeaponConfig
{
public:
  uint m_fragments;
  uint m_angle_dispersion;

  CluzookaConfig();
  virtual void LoadXml(const xmlNode *elem);
};

CluzookaConfig::CluzookaConfig() :
  ExplosiveWeaponConfig(),
  m_fragments(5),
  m_angle_dispersion(45)
{
}

void CluzookaConfig::LoadXml(const xmlNode *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "nb_fragments", m_fragments);
  XmlReader::ReadUint(elem, "nb_angle_dispersion", m_angle_dispersion);
}

class CluzookaCluster : public WeaponProjectile, public Spawner< CluzookaCluster >
{
#ifdef CLUSTERS_SPAWN_CLUSTERS
  uint m_recursion_depth;
#endif

  Double m_time_before_spawn;

public:
  CluzookaCluster(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher);
  void Refresh();
  void Shoot(const Point2i & start_pos, Double strength, Double angle, uint recurse_times);
  virtual void SetEnergyDelta(int delta, bool do_report = true);

protected:
  void DoSpawn();
  virtual void DoExplosion();
  virtual void Draw();
};

CluzookaCluster::CluzookaCluster(ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("cluz_cluster", cfg, p_launcher)
{
  explode_colliding_character = true;
}

void CluzookaCluster::Shoot(const Point2i & start_pos, Double strength, Double angle, uint recurse_times)
{
#ifdef CLUSTERS_SPAWN_CLUSTERS
  m_recursion_depth = recurse_times;
#else
  recurse_times = 0; // to keep compiler happy
#endif

  Camera::GetInstance()->FollowObject(this);
  ResetConstants();
  SetCollisionModel(true, true, false ); // a bit hackish...
  // we do need to collide with objects, but if we allow for this, the clusters
  // will explode on spawn (because of colliding with each other)
  SetXY(start_pos);
  SetSpeed(strength, angle);

  explode_with_timeout = true;
  StartTimeout();
  m_time_before_spawn = 750;
  // make time a bit random to unsychronize particles

  m_time_before_spawn += RandomSync().GetDouble(-300, 100);
}

void CluzookaCluster::Refresh()
{
#ifdef CLUSTERS_SPAWN_CLUSTERS
  if (m_recursion_depth > 1) {
    uint time = GameTime::GetInstance()->Read();
    Double flying_time = Double(time - begin_time);

    if (flying_time >= m_time_before_spawn) {
        DoSpawn();
        Explosion();
        return;
    };
  };
#endif

  WeaponProjectile::Refresh();
// no SetRotation needed as images are balls :)
  //image->SetRotation_rad(GetSpeedAngle());
}

void CluzookaCluster::DoSpawn()
{
  Double angle;
  Double speed;
  GetSpeed(speed, angle);
  Point2i parent_position = GetPosition();

  Double angle_range = QUARTER_PI;

  uint rec_depth = 0;
#ifdef CLUSTERS_SPAWN_CLUSTERS
  rec_depth = m_recursion_depth - 1;
#endif

  Spawner< CluzookaCluster >::Spawn(2, rec_depth, parent_position,
                                    angle, angle_range, cfg, launcher);
}

void CluzookaCluster::DoExplosion()
{
  if (!m_spawned_clusters) {
    ApplyExplosion(GetPosition(), cfg, "weapon/cluzooka_hit", false, ParticleEngine::LittleESmoke);
  }
  else
    Ghost();    // just hide ourselvers
}

void CluzookaCluster::Draw()
{
  // custom Draw() is needed to avoid drawing timeout on top of clusters
  image->Draw(GetPosition());
}

void CluzookaCluster::SetEnergyDelta(int /* delta */, bool /* do_report */){};


class CluzookaRocket : public WeaponProjectile, public Spawner< CluzookaCluster >
{
  ParticleEngine smoke_engine;
  SoundSample flying_sound;
  bool m_timed_out;
public:
  CluzookaRocket(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher);
  void Refresh();
  void Explosion();
  void Shoot(Double strength);
protected:
  virtual void DoSpawn();
  virtual void DoExplosion();
  virtual void SignalTimeout();
  virtual void SignalOutOfMap();
  virtual void SignalDrowning();
};

CluzookaRocket::CluzookaRocket(ExplosiveWeaponConfig& cfg,
                                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("cluz_rocket", cfg, p_launcher), smoke_engine(20),
  m_timed_out( false )
{
  explode_colliding_character = true;
  explode_with_timeout = true;
}

void CluzookaRocket::Refresh()
{
  WeaponProjectile::Refresh();
  if (!IsDrowned()) {
    //image->SetRotation_rad(GetSpeedAngle());
    Double flying_time(GetMSSinceTimeoutStart());

    Double speed_angle = GetSpeedAngle();
    const Double time_to_rotate = 500;
    const Double num_of_full_rotates = 4;

    // make it rotate itself for first N msec
    if (flying_time < time_to_rotate) {
      Double t = flying_time / time_to_rotate; // portion of time
      Double inv_t = ONE - t;
      // rotate speed is max when t is close to 0, and slows down to 1
      // when t is approaching 1
      //Double rotate_speed = 1 + num_of_full_rotates * ( 1.0f - t );
      inv_t *= inv_t;
      image->SetRotation_rad(speed_angle + TWO_PI * num_of_full_rotates * inv_t * inv_t);
    } else {
      image->SetRotation_rad(speed_angle);
    }

    smoke_engine.AddPeriodic(Point2i(GetX() + GetWidth() / 2,
                                     GetY() + GetHeight()/ 2), particle_DARK_SMOKE, false, -1, 2.0);
  } else {
    image->SetRotation_rad(HALF_PI);
  }
}

void CluzookaRocket::DoSpawn()
{
  uint fragments = static_cast<CluzookaConfig &>(cfg).m_fragments;
  Double angle_range = static_cast<CluzookaConfig &>(cfg).m_angle_dispersion * PI / 180;

  Double angle;
  Double speed;
  GetSpeed(speed, angle);

  Spawner< CluzookaCluster >::Spawn(fragments, 0,
                                    GetPosition(), angle, angle_range, cfg, launcher);
}

void CluzookaRocket::DoExplosion()
{
  if (!m_spawned_clusters) {
    ASSERT( !m_timed_out );
    WeaponProjectile::DoExplosion();
  }
/*
  // only explode if no clusters were spawned
  if (!m_spawned_clusters)
    WeaponProjectile::DoExplosion();
  else
    Ghost();// just go ghost to prevent collisions with clusters
*/
}

void CluzookaRocket::Shoot(Double strength)
{
  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  flying_sound.Play("default","weapon/rocket_flying", -1);

  WeaponProjectile::Shoot(strength);
}

void CluzookaRocket::SignalOutOfMap()
{
  Weapon::Message(_("The rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();

  flying_sound.Stop();
}

void CluzookaRocket::SignalDrowning()
{
  smoke_engine.Stop();
  WeaponProjectile::SignalDrowning();

  flying_sound.Stop();
}

void CluzookaRocket::Explosion()
{
  if (m_timed_out) {
    DoSpawn();
    Ghost();
  } else
    WeaponProjectile::Explosion();

  flying_sound.Stop();
}

void CluzookaRocket::SignalTimeout()
{
  m_timed_out = true;

  WeaponProjectile::SignalTimeout();
};

//-----------------------------------------------------------------------------

Cluzooka::Cluzooka() :
  WeaponLauncher(WEAPON_CLUZOOKA, "cluzooka", new CluzookaConfig())
{
  UpdateTranslationStrings();

  m_category = HEAVY;
  ReloadLauncher();
}

WeaponProjectile * Cluzooka::GetProjectileInstance()
{
  return new CluzookaRocket(cfg(), this);
}

void Cluzooka::UpdateTranslationStrings()
{
  m_name = _("Cluzooka");
  m_help = _("Initial fire angle: Up/Down\nFire: Press space until desired strength is reached\nOne ammo per turn");
}

std::string Cluzooka::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext("%s team has won %u Cluster bazookas!",
                         "%s team has won %u Cluster bazookas!",
                         items_count), TeamName, items_count);
}
