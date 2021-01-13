/**
 * Locate and load resources

 * Copyright (C) 2007, 2008, 2009, 2015  Sylvain Beucler

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

#include "resources.h"
#include "paths.h"

SDL_RWops* find_resource_as_rwops(char *name)
{
  SDL_RWops* rwops = NULL;

  /** curdir, pkgdatadir **/
  FILE *in = NULL;
  if (in == NULL)
    in = paths_pkgdatafile_fopen(name, "rb");

#ifdef __ANDROID__
  /* Get from .apk */
#endif

  if (in != NULL)
    rwops = SDL_RWFromFP(in, /*autoclose=*/SDL_TRUE);

  if (rwops != NULL)
     return rwops;

  /** Not found! **/
  return NULL;
}
