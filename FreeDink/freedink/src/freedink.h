/**
 * Header for FreeDink-specific code (not FreeDinkedit)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003, 2004, 2005, 2007, 2008, 2009, 2010, 2012, 2014, 2015  Sylvain Beucler

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

#ifndef _FREEDINK_H
#define _FREEDINK_H

#include "SDL.h"

/* show_bmp() currently ran */
struct show_bmp
{
	/*bool*/int active;
	/*bool*/int showdot;
	int reserved;
	int script;
	int stime;
	int picframe;
};
extern struct show_bmp showb;

extern Uint32 but_timer;

extern int check_if_move_is_legal(int u);
extern int did_player_cross_screen();
extern void process_show_bmp( void );
extern void process_warp_man(void);

extern void run_through_tag_list(int h, int strength);
extern int hurt_thing(int h, int damage, int special);
extern /*bool*/int run_through_tag_list_talk(int h);
extern void run_through_touch_damage_list(int h);

extern int special_block(int block);
extern void CyclePalette();
extern void up_cycle(void);

#endif
