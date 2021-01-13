/**
 * Displayed screen

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

#ifndef LIVE_SCREEN_H
#define LIVE_SCREEN_H

#include "live_sprite.h"
#include "editor_screen.h"
#include "IOGfxSurface.h"

/* base editor screen */
extern struct editor_screen cur_ed_screen;

// struct for hardness map
extern unsigned char screen_hitmap[600+1][400+1];

extern int playl;
extern int playx;
extern int playy;

extern /*bool*/int transition_in_progress;


extern void live_screen_init();
extern void add_hardness(int sprite, int num);
extern unsigned char get_hard(int x1, int y1, int screenlock);
extern void fill_hard_sprites(void);
extern void fill_whole_hard(void);
extern void fill_hardxy(rect box);

extern void screen_rank_game_sprites(int* rank);
/*bool*/int get_box (int h, rect * box_scaled, rect * box_real, bool skip_screen_clipping);
extern void draw_sprite_game(IOGfxSurface* GFX_lpdest, int h);
extern void grab_trick(int dir);
extern bool transition(int fps_final);

#endif
