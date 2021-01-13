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
 * Class containing object physic constants
 *****************************************************************************/

//-----------------------------------------------------------------------------
#include <cstring>
#include <iostream>
#include "object/object_cfg.h"
#include "game/config.h"
#include "game/game_mode.h"
#include <WARMUX_base.h>
#include <WARMUX_debug.h>
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

static const Double DEFAULT_WATER_RESIST_FACTOR = 40;

ObjectConfig::ObjectConfig(void):
  m_mass(1.0),
  m_wind_factor(1.0),
  m_air_resist_factor(1.0),
  m_water_resist_factor(DEFAULT_WATER_RESIST_FACTOR),
  m_gravity_factor(1.0),
  m_rebounding(false),
  m_rebound_factor(0.01),
  m_align_particle_state(false)
{
}

void ObjectConfig::LoadXml(const std::string & obj_name, 
                           const std::string & config_file)
{
  const xmlNode* elem = NULL;
  XmlReader      doc;

  if ("" == config_file) {
    const GameMode *mode = GameMode::GetConstInstance();
    MSG_DEBUG("game_mode", "Load %s configuration from %s\n",
              obj_name.c_str(), mode->GetName().c_str());

    const XmlReader* ddoc = mode->GetXmlObjects();
    elem = XmlReader::GetMarker(ddoc->GetRoot(), obj_name);
  } else {
    MSG_DEBUG("game_mode", "** Load %s configuration from file %s\n",
              obj_name.c_str(), config_file.c_str());

    // Load Xml configuration
    ASSERT(doc.Load(config_file));
    elem = XmlReader::GetMarker(doc.GetRoot(), obj_name);
  }

  ASSERT(elem != NULL);
  XmlReader::ReadDouble(elem, "mass",                m_mass);
  XmlReader::ReadDouble(elem, "wind_factor",         m_wind_factor);
  XmlReader::ReadDouble(elem, "air_resist_factor",   m_air_resist_factor);
  XmlReader::ReadDouble(elem, "water_resist_factor", m_water_resist_factor);
  XmlReader::ReadDouble(elem, "gravity_factor",      m_gravity_factor);
  XmlReader::ReadDouble(elem, "rebound_factor",      m_rebound_factor);
  XmlReader::ReadBool(elem,   "rebounding",          m_rebounding);
  XmlReader::ReadBool(elem,   "auto_align_particle", m_align_particle_state);
}
