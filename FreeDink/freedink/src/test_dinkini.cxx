/**
 * Test suite for Dink INI parsing and sequence configuration

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

#include <cxxtest/TestSuite.h>

#include "dinkini.h"
#include "SDL.h"
#include "IOGfxDisplay.h"

/* Mocks */
int truecolor = 1;
SDL_Surface *GFX_backbuffer, *GFX_tmp2;
IOGfxDisplay* g_display = NULL;

extern int PlayMidi(char *sFileName) { return 1; }
extern SDL_Surface* load_bmp_from_fp(FILE* in) { return NULL; }

extern int nb_idata;

class TestDinkIni : public CxxTest::TestSuite {
public:
	void setUp() {
	}
	void tearDown() {
	}
	
	void test_dinkini_init() {
		dinkini_init(123);
		TS_ASSERT_EQUALS(nb_idata, 123);
	}
};
