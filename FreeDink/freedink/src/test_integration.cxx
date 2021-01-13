/**
 * FreeDink test suite

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

/**
 * Currently the testsuite is grossly in the same directory as the
 * code.  The reason is that there's no existing test suite, and that
 * we need to write tests with minimal changes to the code to assess
 * how the code _currently_ works.  In a second step we'll modularize
 * FreeDink more so that tests can target independent units of code -
 * and check if we broke anything in the process with the test suite
 * :)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cxxtest/TestSuite.h>

#include "dinkc.h"
#include "dinkc_bindings.h"
#include "game_engine.h"
#include "live_sprites_manager.h"
#include "DMod.h"
#include "editor_screen.h"
#include "freedink.h"
#include "paths.h"
#include "gfx.h"
#include "IOGfxSurfaceSW.h"

class TestIntegration : public CxxTest::TestSuite {
public:
  // See also http://www.dinknetwork.com/forum.cgi?MID=186069#186263
  void test_integration_player_position_is_updated_after_screen_is_loaded() {
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    ts_paths_init();
    memset(&g_dmod.map.ts_loc_mem, 0, sizeof(g_dmod.map.ts_loc_mem));
	SDL_Surface* GFX_background = SDL_CreateRGBSurface(0, 1, 1, 32, 0,0,0,0);
	IOGFX_background = new IOGfxSurfaceSW(GFX_background);
	SDL_Surface* GFX_tmp1 = SDL_CreateRGBSurface(0, 1, 1, 32, 0,0,0,0);
	IOGFX_tmp1 = new IOGfxSurfaceSW(GFX_tmp1);

    dinkc_init();
    dinkc_bindings_init();
    make_int("&tsx", 123451, DINKC_GLOBAL_SCOPE, 0);
    make_int("&tsy", 123452, DINKC_GLOBAL_SCOPE, 0);
    int tsx_id = lookup_var("&tsx", DINKC_GLOBAL_SCOPE);
    int tsy_id = lookup_var("&tsy", DINKC_GLOBAL_SCOPE);
    const char* screen_main_code =
      "void main(void) {\n"
      "  &tsx = sp_x(1, -1);\n"
      "  &tsy = sp_y(1, -1);\n"
      "}";
    int screen_script_id = ts_script_init("ts_pos_check", strdup(screen_main_code));
    rinfo[screen_script_id]->sprite = 1000; // don't kill me
    
    int player_map = 33;
    pplayer_map = &player_map;
    int vision = 0;
    pvision = &vision;
    
    // Create 5 connected screens
    struct editor_screen s;
    s.ts_script_id = screen_script_id;
    g_dmod.map.loc[33] = 1; g_dmod.map.ts_loc_mem[33] = &s;
    g_dmod.map.loc[32] = 1; g_dmod.map.ts_loc_mem[32] = &s;
    g_dmod.map.loc[34] = 1; g_dmod.map.ts_loc_mem[34] = &s;
    g_dmod.map.loc[1]  = 1; g_dmod.map.ts_loc_mem[1]  = &s;
    g_dmod.map.loc[65] = 1; g_dmod.map.ts_loc_mem[65] = &s;
    
    screenlock = 0;
    walk_off_screen = 0;
    spr[1].active = true;
    
    *pplayer_map = 33;
    spr[1].x = -1;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(play.var[tsx_id].var, -1);
    TS_ASSERT_EQUALS(spr[1].x, 619);
    
    *pplayer_map = 33;
    spr[1].y = -1;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(play.var[tsy_id].var, -1);
    TS_ASSERT_EQUALS(spr[1].y, 399);
    
    *pplayer_map = 33;
    spr[1].x = 620;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(play.var[tsx_id].var, 620);
    TS_ASSERT_EQUALS(spr[1].x, 20);
    
    *pplayer_map = 33;
    spr[1].y = 401;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(play.var[tsy_id].var, 401);
    TS_ASSERT_EQUALS(spr[1].y, 0);
    
    
    walk_off_screen = 1;
    
    spr[1].x = -1;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, -1);
    
    spr[1].y = -1;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, -1);
    
    spr[1].x = 700;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, 700);
    
    spr[1].y = 500;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, 500);
    
    walk_off_screen = 0;
    
    
    screenlock = 1;
    
    spr[1].x = 19;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, 20);
    
    spr[1].x = 20;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, 20);
    
    spr[1].y = -1;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, 0);
    
    spr[1].y = 0;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, 0);
    
    spr[1].x = 620;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, 619);
    
    spr[1].x = 619;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].x, 619);
    
    spr[1].y = 400;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, 399);
    
    spr[1].y = 399;
    did_player_cross_screen();
    TS_ASSERT_EQUALS(spr[1].y, 399);
    
    screenlock = 0;
        
    dinkc_bindings_quit();
    dinkc_quit();
    paths_quit();
  }
};
