/**
 * System initialization, common to FreeDink and FreeDinkEdit

 * Copyright (C) 2007, 2008  Sylvain Beucler

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

#ifndef _INIT_H
#define _INIT_H

#include "SDL.h"
#include <string>

class App {
public:
	std::string description;
	char* splash_path;

	int run = 1;

	int g_b_no_write_ini; // -noini passed to command line?
	int opt_version;
	bool dinkini_playmidi;
	bool dinkgl;
	bool windowed;

	App();
	virtual ~App();

	virtual void init() = 0;
	virtual void input(SDL_Event* ev) = 0;
	virtual void logic() = 0;

	int main(int argc, char* argv[]);
	void loop();
	void one_iter();

	void print_version();
	void print_help(int argc, char *argv[]);
	bool check_arg(int argc, char *argv[]);
};

#endif
