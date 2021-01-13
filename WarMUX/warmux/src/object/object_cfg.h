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
 * Classe for object physics
 *****************************************************************************/

#ifndef OBJECT_CFG_H
#define OBJECT_CFG_H
//-----------------------------------------------------------------------------
#include <string>
#include <WARMUX_types.h>
//-----------------------------------------------------------------------------

class ObjectConfig
{
public:
  // Masse of the objct
  Double m_mass;

  // Wind effect factor on the object. 0 means not affected.
  Double m_wind_factor;

  // Air resistance factor. 1 = normal air resistance.
  Double m_air_resist_factor;

  // Water resistance factor. Related to air_resist_factor.
  Double m_water_resist_factor;

  // Define how the object is affected by gravity.
  Double m_gravity_factor;

  // Tell if it's a rebounding object...
  bool  m_rebounding;

  // Object rebound factor when the object collide with the ground.
  Double m_rebound_factor;

  // Active alignment for wind particles
  bool m_align_particle_state;

  ObjectConfig(void);
  virtual ~ObjectConfig() { };

  void LoadXml(const std::string & obj_name, 
               const std::string & config_file);
};

#endif // OBJECT_CFG_H
