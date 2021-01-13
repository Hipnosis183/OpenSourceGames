/**
 * FreeDink test suite

 * Copyright (C) 2005, 2014, 2015  Sylvain Beucler

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

#include "io_util.h"

/* mkdir */
#include <sys/stat.h>
#include <sys/types.h>
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#include <direct.h>
#define mkdir(name,mode) mkdir(name)
#endif
/* rmdir */
#include <unistd.h>

#define PREFIX "check_freedink privateprefix/"
#define TESTDIR PREFIX "subdir1/"
class TestIOUtil : public CxxTest::TestSuite {
public:
  void setUp() {
    mkdir(PREFIX, 0300);
    mkdir(TESTDIR, 0777);
    mkdir(TESTDIR "SubDir2", 0777);
  }
  void tearDown() {
    rmdir(TESTDIR "SubDir2");
    rmdir(TESTDIR);
    rmdir(PREFIX);
  }
  
  int ciconvert_ext(const char* wrong_case, const char* good_case)
  {
    /* Create file with proper case */
    {
      FILE *f = NULL;
      if ((f = fopen(good_case, "w")) == NULL)
	return 0;
      fclose(f);
    }
    
    /* Attempt to open it using wrong case */
    int success = 0;
    {
      FILE *f = NULL;
      char *fixed_case = strdup(wrong_case);
      ciconvert(fixed_case);
      if ((f = fopen(fixed_case, "r")) != NULL)
	success = 1;
      
      if (f != NULL) fclose(f);
      unlink(good_case);
      free(fixed_case);
    }
    return success;
  }
  
  void test_ioutil_ciconvert() {
    TS_ASSERT( ciconvert_ext(TESTDIR "toto",  TESTDIR "toto"));
    TS_ASSERT( ciconvert_ext(TESTDIR "ToTo",  TESTDIR "toto"));
    TS_ASSERT( ciconvert_ext(TESTDIR "ToTo",  TESTDIR "TOTO"));
    TS_ASSERT( ciconvert_ext(TESTDIR "ToTo",  TESTDIR "tOtO"));
    TS_ASSERT(!ciconvert_ext(TESTDIR "ToTo",  TESTDIR "t0t0"));

    /* - with multiple slashes: .//file */
    TS_ASSERT( ciconvert_ext(TESTDIR "/ToTo", TESTDIR "toto"));

    /* - absolute path */
    char* dir = (char*)malloc(PATH_MAX);
    getcwd(dir, PATH_MAX);
    char* good_case  = (char*)calloc(1, strlen(dir) + 1 + strlen(TESTDIR) + 4 + 1);
    char* wrong_case = (char*)calloc(1, strlen(dir) + 1 + strlen(TESTDIR) + 4 + 1);
    strcat(good_case, dir);
    strcat(good_case, "/");
    strcat(good_case, TESTDIR);
    strcat(wrong_case, good_case);
    strcat(good_case, "toto");
    strcat(wrong_case, "ToTo");
    TS_ASSERT( ciconvert_ext(wrong_case, good_case));
    free(good_case);
    free(wrong_case);
    free(dir);

    /* - access to subsubdirectories using '/' */
    TS_ASSERT( ciconvert_ext(TESTDIR "SubdIr2/toto", TESTDIR "SubDir2/toto"));

    /* - access to subsubdirectories using '\' */
    TS_ASSERT( ciconvert_ext(TESTDIR "SubdIr2\\toto", TESTDIR "SubDir2/toto"));

    /* - files containing '\' ... works on GNU/Linux but not on Windows,
       not portable, not supported */
    /* ck_assert(?ciconvert_ext(TESTDIR "to\\to", TESTDIR "to\\to")); */

    /* - with a file and a directory with the same name, in the same
       directory: currently we don't support that, as it didn't work on
       original Dink / woe anyway, and it's not portable */
    /* ck_assert(?ciconvert_ext(TESTDIR "subdir", TESTDIR "subdir")); */
  }
};
