/**
 * Game world state

 * Copyright (C) 2015  Sylvain Beucler

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

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "dinkc.h"
#include "gfx_tiles.h"

/* Game state. */
// for storing current tiles in save game
struct player_info_tile
{
  char file[50];
};

enum item_type { ITEM_REGULAR, ITEM_MAGIC };
#define NB_MITEMS 8
#define NB_ITEMS 16
struct item_struct
{
  BOOL_1BYTE active;
  char name[10+1];
  int seq;
  int frame;
};

struct editor_sprite_override
{
  unsigned char type[100];  // DinkC's editor_type(i)
  unsigned short seq[100];  // DinkC's editor_seq(i)
  unsigned char frame[100]; // DinkC's editor_frame(i)
  Uint32 last_time; // ticks when type 6, 7 or 8 was set
};

struct player_info
{
  int minutes;
  
  struct item_struct mitem[NB_MITEMS];
  struct item_struct item[NB_ITEMS];
  
  int curitem; // highlighted item in the inventory
  enum item_type item_type;

  struct editor_sprite_override spmap[769]; /* editor sprite override */
  
  struct varman var[MAX_VARS];
  
  int last_map; /* screen to show on the map */
  
  char palette[50];
  struct player_info_tile tile[GFX_TILES_NB_SETS+1];
  struct global_function func[100];

  /* Variables that need not be saved */
  bool push_active; /* player started maintaining direction against hard object */
  int push_dir; /* pushing animation direction */
  unsigned int push_timer; /* time the player started maintaining the direction */

  int last_talk; /* script of last 'say_stop*' sprite */

  int mouse; /* vertical position of the mouse when selecting a dialog option */
};
extern struct player_info play;

/* TODO: non-backed-up game state */
extern int dinkspeed; /* player speed, cf. dc_set_dink_speed */
extern int push_active; /* player is allowed to push, cf. dc_push_active */
extern unsigned int dink_base_push; /* base Dink push sequence, cf. set_dink_base_push */


/* Engine variables directly mapped with DinkC variables */
extern int *pvision, *plife, *presult, *pspeed, *ptiming, *plifemax,
  *pexp, *pstrength, *pcur_weapon,*pcur_magic, *pdefense,
  *pgold, *pmagic, *plevel, *plast_text, *pplayer_map, *pmagic_level,
  *pupdate_status, *pmissile_target, *penemy_sprite,
  *pmagic_cost, *pmissle_source;

#endif
