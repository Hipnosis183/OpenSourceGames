/**
 * Map - group of screen references (dink.dat)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2012, 2014, 2015  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "EditorMap.h"

#include "paths.h"
#include "io_util.h"

using namespace std;

EditorMap::EditorMap(string dink_dat, string map_dat) :
	dink_dat(dink_dat), map_dat(map_dat) {}

/**
 * Load dink.dat, an offsets index to screens stored in map.dat, with
 * some metadata (midi #, indoor/outdoor)
 */
bool EditorMap::load() {
  FILE *f = NULL;

  f = paths_dmodfile_fopen(dink_dat.c_str(), "rb");
  if (!f)
    return false;

  /* Portably load EditorMap from disk */
  int i = 0;
  fseek(f, 20, SEEK_CUR); // unused 'name' field
  for (i = 0; i < 769; i++)
    loc[i]    = read_lsb_int(f);
  for (i = 0; i < 769; i++)
    music[i]  = read_lsb_int(f);
  for (i = 0; i < 769; i++)
    indoor[i] = read_lsb_int(f);
  fseek(f, 2240, SEEK_CUR); // unused space

  fclose(f);

  memset(&ts_loc_mem, 0, sizeof(ts_loc_mem));

  return true;
}

/**
 * Save dink.dat (index of map offsets + midi# + indoor/outdoor)
 * Only used by the editor
 */
void EditorMap::save() {
	FILE *f = paths_dmodfile_fopen(dink_dat.c_str(), "wb");
	if (f == NULL)
		{
			perror("Cannot save dink.dat");
			return;
		}
	
	/* Portably dump EditorMap to disk */
	int i = 0;
	char name[20] = "Smallwood";
	fwrite(name, 20, 1, f);
	for (i = 0; i < 769; i++)
		write_lsb_int(loc[i],    f);
	for (i = 0; i < 769; i++)
		write_lsb_int(music[i],  f);
	for (i = 0; i < 769; i++)
		write_lsb_int(indoor[i], f);
	fseek(f, 2240, SEEK_CUR); // unused field
	
	fclose(f);
}
