/**
 * Test suite for DinkC parser and bindings

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dinkc.h"

int get_parms(char proc_name[20], int script, char *str_params, int* spec);

/* mocks / link seams */
#include "game_engine.h"
int dversion = 107;
struct player_info play;
#include "live_sprites_manager.h"
struct sp spr[MAX_SPRITES_AT_ONCE];
#include "game_choice.h"
struct game_choice_struct game_choice;
int last_sprite_created;
int input_get_button_action(int button_index) { return -1; }
void game_choice_start(int script, int nb_choices) {}
void game_choice_clear() {}
void kill_text_owned_by(int sprite) {}
Uint32 game_GetTicks(void) {return 0; }


class TestDinkC : public CxxTest::TestSuite {
public:
  int script_id;
  void setUp() {
    dinkc_init();
    script_id = ts_script_init("unit test", strdup(""));
  }
  void tearDown() {
    kill_script(script_id);
    dinkc_quit();
  }

  void test_dinkc_getparms_bounds() {
    // memory bounds
    char* str_params = strdup("(\"");
    int spec[] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(get_parms("ignored", script_id, str_params, spec), 0);
    free(str_params);
  }
  void test_dinkc_getparms_int() {
    // Basic int test
    char str_params[] = "(21,22050, 0,0,0);";
    int spec[] = { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
    TS_ASSERT_EQUALS(get_parms("ignored", script_id, str_params, spec), 1);
  }
  void test_dinkc_getparms_emptyint() {
    {
      // [empty] is considered a valid int
      char str_params[] = "(,)";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(get_parms("ignored", script_id, str_params, spec));
    }
    // e.g. it's OK to have empty arguments list when a single int is expected
    {
      char str_params[] = "()";
      int spec[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(get_parms("ignored", script_id, str_params, spec));
    }
    // this doesn't apply to strings
    {
      char str_params[] = "()";
      int spec[] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    // nor does this make parameters optional
    {
      char str_params[] = "(1)";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    // Good test
    {
      char str_params[] = "(1,1)";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(get_parms("ignored", script_id, str_params, spec));
    }
  }

  void test_dinkc_getparms_parens() {
    // Opening paren is mandatory
    {
      char str_params[] = "sp_dir[1,2)";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    // Closing paren is mandatory
    {
      char str_params[] = "(1,1";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    {
      char str_params[] = "(1,1,)";
      int spec[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    {
      char str_params[] = "(1,1;";
      int spec[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(!get_parms("ignored", script_id, str_params, spec));
    }
    // Good test
    {
      char str_params[] = "(1,\"a\")";
      int spec[] = { 1, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
      TS_ASSERT(get_parms("ignored", script_id, str_params, spec));
    }
  }

  void lookup_var_ext() {
    TS_ASSERT(!lookup_var("&toto", DINKC_GLOBAL_SCOPE));
    make_int("&toto", 1, DINKC_GLOBAL_SCOPE, script_id);
    TS_ASSERT(lookup_var("&toto", DINKC_GLOBAL_SCOPE));
    make_int("&tata", 1, script_id, script_id);
    TS_ASSERT(!lookup_var("&tata", DINKC_GLOBAL_SCOPE));
    TS_ASSERT(lookup_var("&tata", script_id));
  }
  void test_dinkc_lookup_var_107() {
    dversion = 107;
    lookup_var_ext();
    
    // v107 has no scope priority
    int var_id;
    make_int("&titi", 1, DINKC_GLOBAL_SCOPE, script_id);
    make_int("&titi", 2, script_id, script_id);
    TS_ASSERT_RELATION(std::greater<int>, var_id = lookup_var("&titi", script_id), 0);
    TS_ASSERT_EQUALS(play.var[var_id].var, 2);
    TS_ASSERT_RELATION(std::greater<int>, var_id = ts_lookup_var_local_global("&titi", script_id), 0);
    TS_ASSERT_EQUALS(play.var[var_id].var, 1);
    
    // needed for CK_FORK=no (woe)
    kill_all_vars();
  }
  void test_dinkc_lookup_var_108() {
    dversion = 108;
    lookup_var_ext();
    
    // v108 has scope priority
    int var_id;
    make_int("&titi", 1, DINKC_GLOBAL_SCOPE, script_id);
    make_int("&titi", 2, script_id, script_id);
    TS_ASSERT_RELATION(std::greater<int>, var_id = lookup_var("&titi", script_id), 0);
    TS_ASSERT_EQUALS(play.var[var_id].var, 2);
    TS_ASSERT_RELATION(std::greater<int>, var_id = ts_lookup_var_local_global("&titi", script_id), 0);
    TS_ASSERT_EQUALS(play.var[var_id].var, 2);
    
    // needed for CK_FORK=no (woe)
    kill_all_vars();
  }

  void test_dinkc_dont_return_same_script_id_twice() {
    int script_id1 = ts_script_init("script1", strdup(""));
    int script_id2 = ts_script_init("script2", strdup(""));
    TS_ASSERT_DIFFERS(script_id1, script_id2);
    kill_script(script_id1);
    kill_script(script_id2);
  }
};
