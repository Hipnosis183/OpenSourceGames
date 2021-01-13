/**
 * Hardness tileset for game map

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

#include "gfx_tiles.h"
#include "io_util.h"

/**
 * 1 hardness block
 */
struct ts_block
{
  unsigned char hm[50+1][50+1];  // tile hardness/hitmap
  BOOL_1BYTE used;
};

//struct for hardness info, INDEX controls which hardness each block has.  800 max
//types available.
#define HARDNESS_NB_TILES 800
struct hardness
{
  struct ts_block htile[HARDNESS_NB_TILES];
  /* default hardness for each background tile square */
  /* TODO: move me to gfx_tiles for clarity: */
  short btile_default[GFX_TILES_NB_SQUARES];
};

extern struct hardness hmap;


extern void load_hard(void);
extern void save_hard(void);
