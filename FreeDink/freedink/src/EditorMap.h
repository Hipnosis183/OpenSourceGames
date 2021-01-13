/**
 * Map - group of screen references (dink.dat)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2014, 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _MAP_H
#define _MAP_H

#include "editor_screen.h"
#include <string>

using namespace std;

class EditorMap
{
 public:
  std::string dink_dat;
  std::string map_dat;

  EditorMap(string dink_dat = "dink.dat", string map_dat = "map.dat");
  bool load();
  void save();
  
  int loc[768+1];  // screen index in map.dat
  struct editor_screen* ts_loc_mem[768+1]; // pre-loaded screen for testsuite
  int music[768+1];
  int indoor[768+1];
};

#endif
