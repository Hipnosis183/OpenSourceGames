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
 * Base classes for weapons configuration.
 *****************************************************************************/

#include "weapon/weapon_cfg.h"
#include <iostream>
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

void EmptyWeaponConfig::LoadXml(const xmlNode* /*elem*/)
{}

//-----------------------------------------------------------------------------

WeaponConfig::WeaponConfig()
{ damage = 10; }

void WeaponConfig::LoadXml(const xmlNode* elem)
{
  bool r;

  r = XmlReader::ReadUint(elem, "damage", damage);
  if (!r)
    damage = 10;
}

//-----------------------------------------------------------------------------

ExplosiveWeaponConfig::ExplosiveWeaponConfig()
{
  timeout = 0;
  allow_change_timeout = false;
  explosion_range = 0 ;
  particle_range = explosion_range;
  blast_range = 0 ;
  blast_force = 0 ;
  speed_on_hit = 0 ;
}

void ExplosiveWeaponConfig::LoadXml(const xmlNode* elem)
{
  bool r;

  WeaponConfig::LoadXml (elem);
  r = XmlReader::ReadUint(elem, "timeout", timeout);
  if (!r)
    timeout = 0;

  r = XmlReader::ReadBool(elem, "allow_change_timeout", allow_change_timeout);
  if (!r)
    allow_change_timeout = false;

  r = XmlReader::ReadDouble(elem, "explosion_range", explosion_range);
  if (!r)
    explosion_range = 0;

  r = XmlReader::ReadDouble(elem, "particle_range", particle_range);
  if (!r)
    particle_range = 0;

  r = XmlReader::ReadDouble(elem, "blast_range", blast_range);
  if (!r)
    blast_range = 0;

  r = XmlReader::ReadDouble(elem, "blast_force", blast_force);
  if (!r)
    blast_force = 0;

  r = XmlReader::ReadDouble(elem, "speed_on_hit", speed_on_hit);
  if (!r)
    speed_on_hit = 0;
}

//-----------------------------------------------------------------------------

// timeout before explosion
// max_strength
// min_angle, max_angle
// coeff rebond ?
// nb_replay
// can_change_weapon
// force, longueur (baseball)
// nbr_obus
