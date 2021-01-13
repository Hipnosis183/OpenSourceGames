/**
 * Hardness tileset for game map

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2012, 2014, 2015  Sylvain Beucler

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

#include "hardness_tiles.h"
#include "EditorMap.h"
#include "paths.h"
#include "log.h"

/* HWND                    hWndMain = NULL; */
struct hardness hmap;

char current_hard[50] = "hard.dat";

/***
 * Saves hard.dat (only used from the editor)
 */
void save_hard(void)
{
  char skipbuf[10000]; // more than any fseek we do
  memset(skipbuf, 0, 10000);

  FILE *f = paths_dmodfile_fopen(current_hard, "wb");
  if (!f)
    {
      perror("Couldn't save hard.dat");
      return;
    }

  /* Portably dump struct hardness hmap to disk */
  int i = 0;
  for (i = 0; i < HARDNESS_NB_TILES; i++)
    {
      for (int x = 0; x < 50+1; x++)
	for (int y = 0; y < 50+1; y++)
	  fwrite(&hmap.htile[i].hm[x][y], 1, 1, f);
      fputc(hmap.htile[i].used, f);
      fwrite(skipbuf, 2, 1, f); // reproduce memory alignment
      fwrite(skipbuf, 4, 1, f); // unused 'hold' field
    }
  for (i = 0; i < GFX_TILES_NB_SQUARES; i++)
    write_lsb_int(hmap.btile_default[i], f);
  fseek(f, (8000-GFX_TILES_NB_SQUARES)*4, SEEK_CUR); // reproduce unused data

  fclose(f);
}


/**
 * Load hard.dat which contains tile hardness information.
 * 
 * Unlike 1.08, don't reset and save hard.dat during game in case
 * e.g. it was just being written by an external editor.
 */
void load_hard(void)
{
  memset(&hmap, 0, sizeof(struct hardness));

  char skipbuf[10000]; // more than any fseek we do

  FILE *f = NULL;

  /* Try loading the D-Mod hard.dat */
  f = paths_dmodfile_fopen(current_hard, "rb");

  /* Fallback to the default hard.dat */
  if (f == NULL)
    f = paths_fallbackfile_fopen(current_hard, "rb");

  if (f == NULL)
    {
      log_error("Did not find existing hard.dat, using empty data.");
      return;
    }

  /* Portably load struct hardness hmap from disk */
  int i = 0;
  for (i = 0; i < HARDNESS_NB_TILES; i++)
    {
      for (int x = 0; x < 50+1; x++)
	for (int y = 0; y < 50+1; y++)
	  fread(&hmap.htile[i].hm[x][y], 1, 1, f);
      hmap.htile[i].used = fgetc(f);
      fread(skipbuf, 2, 1, f); // reproduce memory alignment
      fread(skipbuf, 4, 1, f); // unused 'hold' field
    }
  for (i = 0; i < GFX_TILES_NB_SQUARES; i++)
    hmap.btile_default[i] = read_lsb_int(f);
  fseek(f, (8000-GFX_TILES_NB_SQUARES)*4, SEEK_CUR); // reproduce unused data

  fclose(f);
}
