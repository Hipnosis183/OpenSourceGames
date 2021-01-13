/**
 * Game main loop

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003, 2004, 2005, 2007, 2008, 2009, 2010, 2012, 2014, 2015  Sylvain Beucler

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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "AppFreeDink.h"
#include "SDL.h"

/**
 * Bootstrap
 */
int main(int argc, char* argv[])
{
	// Create app
	// Store it in the heap as Emscripten will trash the stack
	AppFreeDink* freedink = new AppFreeDink();
	int ret = freedink->main(argc, argv);
	delete freedink;
	return ret;
}
