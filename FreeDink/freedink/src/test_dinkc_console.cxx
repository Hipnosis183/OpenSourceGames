/**
 * FreeDink test suite

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dinkc_console.h"
#include "SDL.h"

using namespace std;

/* Pleases gnulib's error module */
char* program_name = __FILE__;

/* Stub */
static string last_line;
int dinkc_execute_one_liner(char* line) {
	last_line = line;
	return 0;
}

class TestDinkCConsole : public CxxTest::TestSuite {
public:
	void setUp() {
		dinkc_console_show();
	}
	void tearDown() {
		dinkc_console_hide();
	}
	
	void test_console() {
		TS_ASSERT_EQUALS(dinkc_console_get_cur_line(), "");
		SDL_KeyboardEvent kev;
		kev.type = SDL_KEYDOWN;
		kev.keysym.scancode = SDL_SCANCODE_RETURN;
		dinkc_console_process_key((SDL_Event*)&kev);
	}
};
