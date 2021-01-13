/**
 * Talk interface for DinkC

 * Copyright (C) 2014, 2015  Sylvain Beucler

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

#ifndef _TALK_H
#define _TALK_H

#include "SDL.h"

/* Talk choices */
#define TALK_TITLE_BUFSIZ 3000
#define TALK_LINE_BUFSIZ 101
struct game_choice_struct {
	/*bool*/int active;
	int script;
	
	char buffer[TALK_TITLE_BUFSIZ];   /* title */
	int color;
	
	char line[21][TALK_LINE_BUFSIZ];  /* dialog choices */
	int line_return[21]; /* return this number if chosen */

	int cur;
	int last;
	
	int cur_view;
	int cur_view_end;
	int page;
	
	int newy;
	int choices_y;
	
	int curf;
	Uint32 timer;
};


extern struct game_choice_struct game_choice;
extern void game_choice_start(int script, int nb_choices);
extern void game_choice_stop();
extern void game_choice_clear();

extern void game_choice_logic();

#endif
