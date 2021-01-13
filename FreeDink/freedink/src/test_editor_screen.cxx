/**
 * FreeDink test suite - Screen

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

#include "editor_screen.h"
#include "live_screen.h"
#include "paths.h"

class TestScreen : public CxxTest::TestSuite {
public:
	void setUp() {
		ts_paths_init();
	}
	void tearDown() {
	}
	
	void test_new() {
		TS_ASSERT_EQUALS(cur_ed_screen.script, "");
		
		strcpy(cur_ed_screen.script, "onenter.c");
		save_screen("map.dat", 1);
		strcpy(cur_ed_screen.script, "");
		load_screen_to("map.dat", 1, &cur_ed_screen);
		TS_ASSERT_EQUALS(cur_ed_screen.script, "onenter.c");
	}
};
