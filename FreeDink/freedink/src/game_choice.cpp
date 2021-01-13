/**
 * Game choice interface for DinkC

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

#include "game_choice.h"
#include "SDL.h"
#include "log.h"
#include "game_engine.h" /* play */
#include "gfx_fonts.h"
#include "input.h"
#include "sfx.h"

struct game_choice_struct game_choice;

void game_choice_start(int script, int nb_choices) {
  game_choice.last = nb_choices;
  game_choice.cur = 1;
  game_choice.active = /*true*/1;
  game_choice.page = 1;
  game_choice.cur_view = 1;
  game_choice.script = script;

  int ret = SDL_SetRelativeMouseMode(SDL_TRUE);
  if (ret == -1)
    log_error("Relative mouse positionning not supported on this platform.");
  // TODO INPUT: relative mode is messed with pen tablets
}

void game_choice_stop() {
  game_choice.active = /*false*/0;
  SDL_SetRelativeMouseMode(SDL_FALSE);
  // Avoid spurious mouse events in case when we set relative mouse
  // mode back and forth in a single frame during talk_stop/talk_start
  // (aka text submenu):
  SDL_PumpEvents();
}

void game_choice_clear()
{
  memset(&game_choice, 0, sizeof(game_choice));
  play.mouse = 0;
}

void game_choice_logic() {
	if (!game_choice.active)
		return;

	int talk_hold = game_choice.cur;
	if (sjoy.rightd)
		game_choice.cur++;
	if (sjoy.downd)
		game_choice.cur++;
	if (sjoy.upd)
		game_choice.cur--;
	if (sjoy.leftd)
		game_choice.cur--;

	if (play.mouse > 20) {
		game_choice.cur++;
		play.mouse = 0;
	}
	
	if (play.mouse < -20) {
		game_choice.cur--;
		play.mouse = 0;
	}

 begin:
	// TODO: move out view logic to renderer
	int sx = 184;
	int x_depth = 335;
	game_choice.choices_y = 94;

	int sy_hold, y_hold = 0;
	if (game_choice.newy != -5000)
		game_choice.choices_y = game_choice.newy;

	sy_hold = game_choice.choices_y;


	if (talk_hold != game_choice.cur) {
		if (game_choice.cur >= game_choice.cur_view)
			if (game_choice.cur <= game_choice.cur_view_end)
				SoundPlayEffect(11, 22050,0,0,0);
	}
	
	//tabulate distance needed by text, LORDII experience helped here
	{
		int i = game_choice.cur_view;
		for (; i < game_choice.last; i++) {
			rect rcRect;
			rect_set(&rcRect, sx,y_hold, 463,x_depth+100);
			/* Don't print, only check the height in pixel: */
			y_hold = print_text_wrap(game_choice.line[i], &rcRect, 1,
									 1, FONT_DIALOG);
			sy_hold += y_hold;
			
			if (sy_hold > x_depth) {
				game_choice.cur_view_end = i-1;
				goto death;
			}
		}
		
		game_choice.cur_view_end = i;
	}

	if (game_choice.cur_view == 1 && game_choice.cur_view_end == game_choice.last) {
		// Small enough to fit on one screen, lets center it!
		game_choice.choices_y += ((x_depth - sy_hold) / 2) - 20;
	}
	
 death:
	if (game_choice.cur > game_choice.last) {
		SoundPlayEffect(11, 22050,0,0,0);
		game_choice.cur = 1;
	}
	if (game_choice.cur < 1) {
		SoundPlayEffect(11, 22050,0,0,0);
		game_choice.cur = game_choice.last;
	}

	if (game_choice.cur > game_choice.cur_view_end) {
		game_choice.cur_view = game_choice.cur;
		game_choice.page++;
		goto begin;
	}
	if (game_choice.cur < game_choice.cur_view) {
		game_choice.cur_view = 1;
		game_choice.page--;
		goto begin;
	}


	// Prepare arrow animation
	if (game_choice.timer < thisTickCount) {
		game_choice.curf++;
		game_choice.timer = thisTickCount+100;
	}
	if (game_choice.curf == 0) game_choice.curf = 1;
	if (game_choice.curf > 7) game_choice.curf = 1;

	// Return to script on choice
	if (sjoy.button[ACTION_ATTACK] || mouse1) {
		mouse1 = /*false*/0;
		game_choice_stop();
		*presult = game_choice.line_return[game_choice.cur];
		SoundPlayEffect(17, 22050,0,0,0);
		
		if (game_choice.script != 0) {
			//we need to continue a script
			run_script(game_choice.script);
		}
	}
}
