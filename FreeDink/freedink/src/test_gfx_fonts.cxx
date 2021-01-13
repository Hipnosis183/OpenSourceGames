/**
 * Test text sprites generation

 * Copyright (C) 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public
 * License along with GNU FreeDink.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cxxtest/TestSuite.h>

#include "gfx_fonts.h"
#include "FakeIOGfxDisplay.h"
#include "ImageLoader.h"
IOGfxSurface *IOGFX_backbuffer = NULL;
IOGfxDisplay* g_display = new FakeIOGfxDisplay(0, 0, true, 0);
#include "test_gfx_fonts_libe.h"
#include <iostream>
using namespace std;

char* vgasys_fon;
int truecolor;
extern TTF_Font *dialog_font;
extern SDL_Surface* print_text(TTF_Font * font, char *str, SDL_Color color);

class TestGfxFonts : public CxxTest::TestSuite {
public:
	void setUp() {
		putenv("FREETYPE_PROPERTIES=truetype:interpreter-version=35");
		TTF_Init();
		SDL_RWops* rw = SDL_RWFromMem(libe_ttf, libe_ttf_len);
		dialog_font = TTF_OpenFontRW(rw, 1, FONT_SIZE);
		setup_font(dialog_font);
	}
	void tearDown() {
	}
	
	Uint8 getIndexAt(SDL_Surface* img, int x, int y) {
		return ((Uint8*)img->pixels)[x + y*img->pitch];
	}

	void test_print_textTruecolor() {
		truecolor = 1;
		SDL_Color ce;
		Uint32 key;

		SDL_Surface* img = print_text(dialog_font, "toto", {255,255,2});
		TS_ASSERT_EQUALS(img->h, 18); // 19 for SDL_ttf <= 2.0.14, 18 for SDL_ttf >= 2.0.15
		TS_ASSERT_EQUALS(getIndexAt(img, 0,0), 0);
		TS_ASSERT_EQUALS(getIndexAt(img, 1,4), 1);
		ce = { 255, 255, 2, 255 };
		TS_ASSERT_SAME_DATA(&img->format->palette->colors[1], &ce, sizeof(SDL_Color));
		ce = { 255-255, 255-255, 255-2, 0 };
		TS_ASSERT_SAME_DATA(&img->format->palette->colors[0], &ce, sizeof(SDL_Color));
		TS_ASSERT_EQUALS(SDL_GetColorKey(img, &key), 0);
		TS_ASSERT_EQUALS(key, 0);
	}

	void test_print_text_wrap_getcmdsTruecolor() {
		truecolor = 1;
		ImageLoader::initBlitFormat(SDL_PIXELFORMAT_RGB888);
		std::vector<TextCommand> cmds;
		rect r;
		FONTS_SetTextColor(255, 0, 0);
		r = {0,0, 150,100};
		TS_ASSERT_EQUALS(print_text_wrap_getcmds("toto", &r, 1, 0, FONT_DIALOG, &cmds), 19);
		TS_ASSERT_EQUALS(cmds.size(), 1);
		FONTS_SetTextColor(0, 0, 0);
		r = {-2,0, 150,100};
		TS_ASSERT_EQUALS(print_text_wrap_getcmds("toto", &r, 1, 0, FONT_DIALOG, &cmds), 19);
		TS_ASSERT_EQUALS(cmds.size(), 2);
		print_text_flatten_cmds(&cmds);
		TS_ASSERT_EQUALS(cmds.size(), 1);
	}
};
