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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "str_util.h"

/* Pleases gnulib's error module */
char* program_name = __FILE__;

#include <cxxtest/TestSuite.h>
class Test_str_util : public CxxTest::TestSuite {
public:
  void test_strtoupper() {
    char str[] = "toto";
    char* str2 = strdup(str);
    TS_ASSERT_EQUALS(str, str2);
    strtoupper(str2);
    TS_ASSERT_EQUALS(str2, "TOTO");
    TS_ASSERT_DIFFERS(str, str2);
  }

  void test_reverse() {
    char str[] = "toto";
    reverse(str);
    TS_ASSERT_EQUALS(str, "otot");
  }

  void test_separate_string() {
    char str[] = "a!:b!c";
    char* ret;
    ret = separate_string(str, 1, '-');
    TS_ASSERT_EQUALS(ret, str);
    free(ret);
    ret = separate_string(str, 2, '-');
    TS_ASSERT_EQUALS(ret, "");
    free(ret);
    ret = separate_string(str, 3, '-');
    TS_ASSERT_EQUALS(ret, "");
    free(ret);
    
    ret = separate_string(str, 1, ':');
    TS_ASSERT_EQUALS(ret, "a!");
    free(ret);
    
    ret = separate_string(str, 1, '!');
    TS_ASSERT_EQUALS(ret, "a");
    free(ret);
    ret = separate_string(str, 2, '!');
    TS_ASSERT_EQUALS(ret, ":b");
    free(ret);
    ret = separate_string(str, 3, '!');
    TS_ASSERT_EQUALS(ret, "c");
    free(ret);
    ret = separate_string(str, 4, '!');
    TS_ASSERT_EQUALS(ret, "");
    free(ret);
  }
};
