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
 * Usefull class to manage the configuration of a team
 *****************************************************************************/

#ifndef WARMUX_TEAM_CONFIG_H
#define WARMUX_TEAM_CONFIG_H

#include <string>

#include "WARMUX_types.h"

class ConfigTeam
{
public:
  std::string id;
  std::string player_name;
  uint nb_characters;
  std::string ai;

  bool operator ==(const ConfigTeam& other) const { return id == other.id; }
  bool operator ==(const std::string& other_id) const { return id == other_id; }
};

#endif

