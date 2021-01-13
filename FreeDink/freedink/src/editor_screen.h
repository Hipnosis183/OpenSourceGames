/**
 * Editor screen

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

#ifndef _SCREEN_H
#define _SCREEN_H

#include "rect.h"
#include "gfx_tiles.h"
#include "io_util.h"

#define MAX_SPRITES_EDITOR 99


struct editor_sprite
{
  int x, y;
  int seq, frame, type;  /* DinkC: editor_seq, editor_frame, editor_type */
  int size;
  BOOL_1BYTE active;
  int special, brain;
  
  char script[13+1]; /* attached DinkC script */
  int speed, base_walk, base_idle, base_attack, timing, que;
  int hard;
  rect alt; /* trim left/top/right/bottom */
  int is_warp;
  int warp_map;
  int warp_x;
  int warp_y;
  int parm_seq;
  
  int base_die, hitpoints, strength, defense, exp, sound, vision, nohit, touch_damage;

  int rotation; /* unused */
  int base_hit; /* default spr[].base_hit, unused but exported to undocumented sp_base_hit() */
  int gold; /* default spr[].gold; unused but exported to sp_gold() */
};


/* Background square in a screen */
struct editor_screen_tilerefs
{
  short square_full_idx0; /* bg tile index */
  short althard; /* hardness tile index, 0 = bg tile's default hardness tile */
};

/* one screen from map.dat */
struct editor_screen
{
  struct editor_screen_tilerefs t[12*8+1]; // 97 background tile refs
  struct editor_sprite sprite[100+1];
  char script[20+1]; /* script to run when entering the script */
  char ts_script_id; /* script to run when entering the script (pre-loaded for testsuite) */
};

extern int load_screen_to(const char* path, const int num, struct editor_screen* screen);
extern void save_screen(const char* path, const int num);
extern void screen_rank_editor_sprites(int rank[]);
extern int realhard(int tile);

#endif
