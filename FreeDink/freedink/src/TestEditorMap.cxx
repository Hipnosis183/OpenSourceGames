/**
 * FreeDink test suite - Map

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

#include "EditorMap.h"
#include "DMod.h"
#include "paths.h"

class TestEditorMap : public CxxTest::TestSuite {
public:
	void setUp() {}
	void tearDown() {}
	
	void test_new() {
		ts_paths_init();
		TS_ASSERT_EQUALS(g_dmod.map.load(), false);
		TS_ASSERT_EQUALS(g_dmod.map.dink_dat.c_str(), "dink.dat");
		TS_ASSERT_EQUALS(g_dmod.map.loc[1], 0);
		
		EditorMap testmap2("dink2.dat", "map2.dat");
		TS_ASSERT_EQUALS(testmap2.load(), false);
		TS_ASSERT_EQUALS(testmap2.dink_dat.c_str(), "dink2.dat");
		TS_ASSERT_EQUALS(testmap2.map_dat.c_str(), "map2.dat");
		
		EditorMap testmapsave;
		TS_ASSERT_EQUALS(testmapsave.dink_dat.c_str(), "dink.dat");
		TS_ASSERT_EQUALS(testmapsave.map_dat.c_str(), "map.dat");
		TS_ASSERT_EQUALS(testmapsave.loc[1], 0);
		TS_ASSERT_EQUALS(testmapsave.music[1], 0);
		TS_ASSERT_EQUALS(testmapsave.indoor[1], 0);
		testmapsave.loc[1] = 1;
		testmapsave.music[1] = 1;
		testmapsave.indoor[1] = 1;
		testmapsave.loc[768] = 768;
		testmapsave.music[768] = 768;
		testmapsave.indoor[768] = 768;
		testmapsave.save();

		EditorMap testmapload;
		TS_ASSERT_EQUALS(testmapload.dink_dat.c_str(), "dink.dat");
		TS_ASSERT_EQUALS(testmapload.map_dat.c_str(), "map.dat");
		TS_ASSERT_EQUALS(testmapload.load(), true);
		TS_ASSERT_EQUALS(testmapload.loc[1], 1);
		TS_ASSERT_EQUALS(testmapload.music[1], 1);
		TS_ASSERT_EQUALS(testmapload.indoor[1], 1);
		TS_ASSERT_EQUALS(testmapload.loc[768], 768);
		TS_ASSERT_EQUALS(testmapload.music[768], 768);
		TS_ASSERT_EQUALS(testmapload.indoor[768], 768);
	}
};
