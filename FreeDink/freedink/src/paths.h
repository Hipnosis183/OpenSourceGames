/**
 * Compute and store the search paths

 * Copyright (C) 2007, 2008, 2009  Sylvain Beucler

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

#ifndef _PATH_H
#define _PATH_H

#include <stdio.h>

extern bool paths_init(char* argv0, char* refdir_opt, char* dmoddir_opt);
extern const char *paths_getdmoddir(void);
extern const char *paths_getdmodname(void);
extern const char *paths_getfallbackdir(void);
extern const char *paths_getpkgdatadir(void);

extern char* paths_dmodfile(const char* file);
extern char* paths_fallbackfile(const char* file);
extern char* paths_pkgdatafile(const char* file);

extern FILE* paths_dmodfile_fopen(const char* file, const char* mode);
extern FILE* paths_fallbackfile_fopen(const char* file, const char* mode);
extern FILE* paths_pkgdatafile_fopen(const char* file, const char* mode);

extern FILE* paths_savegame_fopen(int num, const char* mode);
extern void paths_quit(void);

extern void ts_paths_init();

#endif
