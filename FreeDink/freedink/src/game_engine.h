/**
 * Game-specific processing

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2008, 2009, 2012  Sylvain Beucler

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

#ifndef _GAME_ENGINE_H
#define _GAME_ENGINE_H

#include <time.h>

#include "SDL.h"
#include "io_util.h"
#include "dinkc.h"
#include "gfx_tiles.h"
#include "inventory.h"

struct attackinfo_struct
{
	int time;
	/*bool*/int active;
	int script;
	/*bool*/int hitme;
	int last_power;
	Uint32 wait;
	Uint32 pull_wait;
};
extern struct attackinfo_struct bow;

extern int walk_off_screen;

struct wait_for_button
{
	int script;
	int button;
	/*bool*/int active;
};
extern struct wait_for_button wait4b;

extern int stop_entire_game;

extern int screenlock;


extern int last_saved_game;

extern char *dversion_string;
extern int dversion;
#define LEN_SAVE_GAME_INFO 200
extern char save_game_info[LEN_SAVE_GAME_INFO];

extern time_t time_start;

extern int smooth_follow;


/* Editor sprite containing the current warp (teleporter), 0 if no active warp: */
extern int process_warp;
extern int process_downcycle;
extern int process_upcycle;
extern unsigned long cycle_clock;
extern int cycle_script;

extern /*bool*/int screen_main_is_running;

extern Uint32 thisTickCount;
extern Uint32 lastTickCount;
extern int fps_final;
extern int base_timing;

extern int keep_mouse;
extern int mode;

extern int warp_editor_sprite;
extern int show_inventory;


extern void game_init(int version);
extern void game_quit();
extern void game_restart();

extern void game_compute_speed();
extern Uint32 game_GetTicks(void);
extern void game_set_high_speed(void);
extern void game_set_normal_speed(void);

extern int next_raise();
extern void add_exp_force(int num, int source_sprite);
extern void add_exp(int num, int killed_sprite);

extern void fix_dead_sprites();

extern int game_load_screen(int num);
extern void update_screen_time(void);
extern void update_play_changes( void );
extern void game_place_sprites(void);
extern void game_place_sprites_background(void);
extern void fill_back_sprites(void);

extern void draw_screen_game(void);
extern void draw_screen_game_background(void);

extern void set_mode(int new_mode);
extern void apply_mode();
extern void set_keep_mouse(int on);

#endif
