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
#include "EditorMap.h"
#include "editor_screen.h"
#include "freedink.h"
#include "paths.h"

/* test headers */
void dc_fade_down(int script, int* yield, int* preturnint);
void dc_fade_up(int script, int* yield, int* preturnint);
void dc_sp_custom(int script, int* yield, int* preturnint, char* key, int sprite, int val);

class Test_dinkc_bindings : public CxxTest::TestSuite {
public:
  /* fade_up() is prioritary over fade_down(),
     post-fade callback script is still overwritten
     
     - broken during 108 merge of truecolor fade
     
     - half-fixed following:
     http://lists.gnu.org/archive/html/bug-freedink/2009-08/msg00042.html
     6b6bef615c4aae45206fa98bb8b4bfea96eb0f3b
     "Give priority to fade_up() over fade_down() - fix SoB intro in truecolor mode"
     -> in Pilgrim Quest, not Stone of Balance
     
     - 2nd half-fixed following:
     http://www.dinknetwork.com/forum.cgi?MID=189461#189461 "FreeDink-specific"
     http://www.dinknetwork.com/forum.cgi?MID=107994
     a54fb13973e6b733e594766f981b724436218061
  */
  void test_dinkc_concurrent_fades() {
    int yield, returnint;
    dinkc_init();
    dinkc_bindings_init();

    int script_id1 = ts_script_init("fade1", strdup(""));
    int script_id2 = ts_script_init("fade2", strdup(""));
    
    process_upcycle = process_downcycle = 0;
    dc_fade_down(script_id1, &yield, &returnint);
    dc_fade_up(script_id2, &yield, &returnint);
    // callback set to last script that fade_xxx()'d
    TS_ASSERT_EQUALS(cycle_script, script_id2);
    // on fade_up/fade_down conflict, cancel fade effect
    TS_ASSERT_EQUALS(process_downcycle, 0);
    // TODO: in 108, flip_it would basically cancel the fade effect
    // FreeDink just forces up_cycle=1 and let it finish
    //TS_ASSERT_EQUALS(process_upcycle, 0);
    
    process_upcycle = process_downcycle = 0;
    dc_fade_up(script_id1, &yield, &returnint);
    dc_fade_down(script_id2, &yield, &returnint);
    // callback set to last script that fade_xxx()'d
    TS_ASSERT_EQUALS(cycle_script, script_id2);
    // on fade_up/fade_down conflict, cancel fade effect
    TS_ASSERT_EQUALS(process_downcycle, 0);
    // TODO: in 108, flip_it would basically cancel the fade effect
    // FreeDink just forces up_cycle=1 and let it finish
    //TS_ASSERT_EQUALS(process_upcycle, 0);
    
    kill_script(script_id1);
    kill_script(script_id2);
    dinkc_bindings_quit();
    dinkc_quit();
  }

  /* Run {2,2} / (char*,char*) functions without crashing */
  void test_dinkc_make_global_function() {
    dinkc_init();
    dinkc_bindings_init();
    int ret = dinkc_execute_one_liner("make_global_function(\"test\", \"my_function\")");
    TS_ASSERT_EQUALS(ret, 0);
    dinkc_bindings_quit();
    dinkc_quit();
  }

  void test_dinkc_sp_custom() {
    int yield, returnint;
    int script_id = ts_script_init("test_dinkc_sp_custom", strdup(""));
    
    dinkc_init();
    dinkc_bindings_init();
    
    spr[1].active = true;  // player
    spr[1].custom = new std::map<std::string, int>;  // mode = 0
    int sprite = add_sprite(0,0,0,0,0);
    TS_ASSERT_EQUALS(sprite, 2);

    /* Segfaults for sprite 1 in dink108 - and in FreeDink as well :P */
    dc_sp_custom(script_id, &yield, &returnint, "foo", 1, 3);
    dc_sp_custom(script_id, &yield, &returnint, "foo", 1, -1);
    TS_ASSERT_EQUALS(returnint, 3);

    dc_sp_custom(script_id, &yield, &returnint, "foo", sprite, 4);
    dc_sp_custom(script_id, &yield, &returnint, "foo", sprite, -1);
    TS_ASSERT_EQUALS(returnint, 4);
    
    dc_sp_custom(script_id, &yield, &returnint, "foo", sprite, 4);
    dc_sp_custom(script_id, &yield, &returnint, "bar", sprite, 34);

    dc_sp_custom(script_id, &yield, &returnint, "foo", sprite, -1);
    TS_ASSERT_EQUALS(returnint, 4);
    dc_sp_custom(script_id, &yield, &returnint, "bar", sprite, -1);
    TS_ASSERT_EQUALS(returnint, 34);
    
    dc_sp_custom(script_id, &yield, &returnint, "idontexit", sprite, -1);
    TS_ASSERT_EQUALS(returnint, 0);

    delete(spr[sprite].custom);
  }
};
