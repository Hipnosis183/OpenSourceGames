/**
 * Displayed sprite

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

#ifndef _LIVE_SPRITE_H
#define _LIVE_SPRITE_H

#include "SDL.h"
#include "rect.h"
#include <map>
#include <string>

#include "IOGfxSurface.h"

struct sp
{
  int x,moveman;
  int y;
  int mx,my;
  int lpx[51],lpy[51];
  int speed;
  int brain;
  int seq_orig,dir;
  int seq;
  int frame;
  unsigned long delay;
  int pseq;
  int pframe;
  
  bool active;
  int attrib;
  unsigned long wait;
  int timing;
  int skip;
  int skiptimer;
  int size;
  int que;
  int base_walk;
  int base_idle;
  int base_attack;
  
  int base_hit;
  int last_sound;
  int hard;
  rect alt;
  int althard;
  int sp_index;  /* editor_sprite */
  /*BOOL*/int nocontrol;
  int idle;
  int strength;
  int damage;
  int defense;
  int hitpoints;
  int exp;
  int gold;
  int base_die;
  int kill_ttl; /* time to live */
  Uint32 kill_start; /* birth */
  int script_num;
  char text[200];
  int text_owner; /* if text sprite, sprite that is saying it */
  IOGfxSurface* text_cache;
  SDL_Rect text_cache_reldst;
  SDL_Color text_cache_color;
  int script;
  int sound;
  int say_stop_callback; /* callback from a say_stop*() DinkC function */
  int freeze;
  /*bool*/int move_active;
  int move_script;
  int move_dir;
  int move_num;
  /*BOOL*/int move_nohard;
  int follow;
  int nohit;
  /*BOOL*/int notouch;
  unsigned long notouch_timer;
  /*BOOL*/int flying;
  int touch_damage;
  int brain_parm;
  int brain_parm2;
  /*BOOL*/int noclip;
  /*BOOL*/int reverse;
  /*BOOL*/int disabled;
  int target;
  int attack_wait;
  Uint32 move_wait;
  int distance;
  int last_hit;
  /*BOOL*/int live; /* sprite created from say_stop_xy(), and ignored by lsm_kill_all_nonlive_sprites */
  int range;
  int attack_hit_sound;
  int attack_hit_sound_speed;
  int action;
  int nodraw;
  int frame_delay;
  int picfreeze;
  /* v1.08 */
  int bloodseq;
  int bloodnum;
  std::map<std::string, int>* custom;
};

extern int getpic(int sprite_no);
extern void check_sprite_status_full(int sprite_no);
extern void check_sprite_status(int h);

extern void live_sprite_set_kill_start(int h, Uint32 thisTickCount);
extern bool live_sprite_is_expired(int h, Uint32 thisTickCount);

extern void live_sprite_animate(int h, Uint32 thisTickCount, bool* p_isHitting);

#endif
