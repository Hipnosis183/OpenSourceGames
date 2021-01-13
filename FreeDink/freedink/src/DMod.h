/**
 * Game top static structure

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

#ifndef DMOD_H
#define DMOD_H

#include <string>
using namespace std;

#include "EditorMap.h"
#include "BgTilesetsManager.h"

class DMod {
public:
	DMod();
	void load(string path);
	void unload();
	
	EditorMap map;
	BgTilesetsManager bgTilesets;
};

extern DMod g_dmod;

#endif
