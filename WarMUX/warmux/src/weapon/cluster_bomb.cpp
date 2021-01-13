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
 * Cluster Bomb : launch a grenade will exploding, it produce new little cluster
 * exploding bomb
 *****************************************************************************/

#include "weapon/cluster_bomb.h"
#include "weapon/weapon_cfg.h"
#include <sstream>
#include <WARMUX_types.h>
#include "weapon/explosion.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/xml_document.h"
#include "game/game_time.h"

class ClusterBombConfig : public ExplosiveWeaponConfig
{
public:
  uint nb_fragments;
  ClusterBombConfig();
  virtual void LoadXml(const xmlNode* elem);
};

class Cluster : public WeaponProjectile
{
public:
  Cluster(ClusterBombConfig& cfg,
          WeaponLauncher * p_launcher);
  void Refresh();
  void Shoot(const Point2i & pos, Double strength, Double angle);
  virtual void SetEnergyDelta(int delta, bool do_report = true);

protected:
  virtual void SignalTimeout();
  virtual void SignalOutOfMap();
  virtual void DoExplosion();
  virtual void Draw();
};

class ClusterBomb : public WeaponProjectile
{
public:
  ClusterBomb(ClusterBombConfig& cfg,
              WeaponLauncher * p_launcher);
  void Refresh();
  virtual void SetEnergyDelta(int delta, bool do_report = true);

protected:
  void DoExplosion();
  void SignalOutOfMap();
};

Cluster::Cluster(ClusterBombConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("cluster", cfg, p_launcher)
{
  explode_colliding_character = true;
}

void Cluster::Shoot(const Point2i & pos, Double strength, Double angle)
{
  SetCollisionModel(true, true, false ); // a bit hackish...
  // we do need to collide with objects, but if we allow for this, the clusters
  // will explode on spawn (because of colliding with each other)

  StartTimeout();
  Camera::GetInstance()->FollowObject(this);
  ResetConstants();
  SetXY( pos );
  SetSpeed(strength, angle);
}

void Cluster::Refresh()
{
  WeaponProjectile::Refresh();
  // make it rotate
  Double flying_time = (Double) GetMSSinceTimeoutStart();
  const Double rotations_per_second = 4;
  image->SetRotation_rad( rotations_per_second * TWO * PI * flying_time / (Double)1000  );
}

void Cluster::Draw()
{
    // custom Draw() is needed to avoid drawing timeout on top of clusters
    image->Draw(GetPosition());
};

void Cluster::SignalOutOfMap()
{
  WeaponProjectile::SignalOutOfMap();
}

void Cluster::DoExplosion()
{
  ApplyExplosion (GetPosition(), cfg, "weapon/explosion", false, ParticleEngine::LittleESmoke);
}

void Cluster::SetEnergyDelta(int /* delta */, bool /* do_report */){};
// because of game mechanics, clusters will inherit timeout from its parent bomb,
// and as such, will explode right after launch - something we don't want
void Cluster::SignalTimeout(){};


//-----------------------------------------------------------------------------

ClusterBomb::ClusterBomb(ClusterBombConfig& cfg,
                         WeaponLauncher * p_launcher)
  : WeaponProjectile ("cluster_bomb", cfg, p_launcher)
{
  m_rebound_sound = "weapon/grenade_bounce";
  explode_with_collision = false;
  explode_with_timeout = true;
}

void ClusterBomb::Refresh()
{
  WeaponProjectile::Refresh();
  image->SetRotation_rad(GetSpeedAngle());
}

void ClusterBomb::SignalOutOfMap()
{
  Weapon::Message(_("The Cluster Bomb has left the battlefield before it could explode."));
  WeaponProjectile::SignalOutOfMap();
}

void ClusterBomb::DoExplosion()
{
  WeaponProjectile::DoExplosion();

  const uint fragments = static_cast<ClusterBombConfig &>(cfg).nb_fragments;
  Cluster * cluster;

  const Double angle_range = HALF_PI;
  Point2i pos = GetPosition();
  for (uint i = 0; i < fragments; ++i )
  {
    Double angle = -HALF_PI; // this angle is "upwards" here
    Double cluster_deviation = angle_range * i / ( Double )fragments - angle_range / TWO;
    Double speed = RandomSync().GetDouble(10, 25);

    cluster = new Cluster(static_cast<ClusterBombConfig &>(cfg), launcher);
    cluster->Shoot( pos, speed, angle + cluster_deviation );

    ObjectsList::GetRef().AddObject(cluster);
  }
}

void ClusterBomb::SetEnergyDelta(int /* delta */, bool /* do_report */){};

//-----------------------------------------------------------------------------

ClusterLauncher::ClusterLauncher() :
  WeaponLauncher(WEAPON_CLUSTER_BOMB, "cluster_bomb", new ClusterBombConfig())
{
  UpdateTranslationStrings();

  m_category = THROW;
  ignore_collision_signal = true;
  ReloadLauncher();
}

void ClusterLauncher::UpdateTranslationStrings()
{
  m_name = _("Cluster Bomb");
  m_help = _("Timeout: Mouse wheel or Page Up/Down\nAngle: Up/Down\nFire: Press space until desired strength is reached\nOne ammo per turn");
}

WeaponProjectile * ClusterLauncher::GetProjectileInstance()
{
  return new ClusterBomb(cfg(), this);
}

ClusterBombConfig& ClusterLauncher::cfg()
{
  return static_cast<ClusterBombConfig&>(*extra_params);
}

std::string ClusterLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u cluster bomb!",
            "%s team has won %u cluster bombs!",
            items_count), TeamName, items_count);
}
//-----------------------------------------------------------------------------

ClusterBombConfig::ClusterBombConfig() :
  ExplosiveWeaponConfig()
{
  nb_fragments = 5;
}

void ClusterBombConfig::LoadXml(const xmlNode* elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "nb_fragments", nb_fragments);
}
