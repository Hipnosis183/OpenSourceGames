/**
 * Graphics utilities: palettes...

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2003, 2004, 2007, 2008, 2009, 2014  Sylvain Beucler

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

#include "gfx_palette.h"

#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "gfx.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"

/* Copy of the physical screen palette - SDL provides a getter for the
   logical palette, but not for the hardware palette */
static SDL_Color phys_palette[256];


/**
 * Get local copy of physical palette
 */
void gfx_palette_get_phys(SDL_Color* palette)
{
  memcpy(palette, phys_palette, sizeof(phys_palette));
}

/**
 * Set physical palette
 */
void gfx_palette_set_phys(SDL_Color* new_palette)
{
  /* Now this one is tricky: DX/Woe has a "feature" where palette
     indexes 0 and 255 are fixed to black and white,
     respectively. This is the opposite of the default Dink palette
     - which is why fill_screen(0) is black and not white as in the
     Dink palette. This also makes "Lyna's Story"'s palette change a
     bit ugly, because pure black and white colors are not reversed
     when you enter "negative" color mode. This does not affect
     other indexes. Technically this happens when you get a palette
     from GetEntries(), and when you CreatePalette() without
     specifying DDPCAPS_ALLOW256 (so respectively, in
     change_screen_palette() and load_palette_from_*()). But well,
     reproducing the bug is important for backward compatibility. */
  
  memcpy(phys_palette, new_palette, sizeof(phys_palette));
  
  phys_palette[0].r = 0;
  phys_palette[0].g = 0;
  phys_palette[0].b = 0;
  phys_palette[255].r = 255;
  phys_palette[255].g = 255;
  phys_palette[255].b = 255;
}

/**
 * Return physical palette as SDL_PixelFormat for easy SDL_MapRGB and
 * SDL_ConvertSurface
 */
SDL_PixelFormat* gfx_palette_get_phys_format() {
	SDL_PixelFormat* fmt = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
	if (!fmt->palette)  // fmt is cached ref from SDL2
		fmt->palette = SDL_AllocPalette(256);
	gfx_palette_get_phys(fmt->palette->colors);
	return fmt;
}

/* Generate a default/fallback 216 colors palette */
void gfx_palette_reset()
{
  SDL_Color palette[256];
  int i = 0;
  int r, g, b;

  /* Set a 3,3,2 color cube */
  for (r = 0; r < 256; r += 0x33)
    {
      for (g = 0; g < 256; g += 0x33)
	{
	  for (b = 0; b < 256; b += 0x33)
	    {
	      palette[i].r = r;
	      palette[i].g = g;
	      palette[i].b = b;
	      i++;
	    }
	}
    }

  /* Set the rest of the colors to black */
  for (; i < 256; i++)
    palette[i].r
      = palette[i].g
      = palette[i].b
      = 0;

  /* Set palette with DX bug overwrite */
  gfx_palette_set_phys(palette);
}

/* Get a colors palette from the specified image */
int
gfx_palette_set_from_surface(SDL_Surface *bmp)
{
  SDL_Color palette[256];
  int i;

  if (bmp == NULL || bmp->format->palette == NULL)
    return -1;

  for (i = 0; i < bmp->format->palette->ncolors; i++)
    {
      palette[i].r = bmp->format->palette->colors[i].r;
      palette[i].g = bmp->format->palette->colors[i].g;
      palette[i].b = bmp->format->palette->colors[i].b;
      // Caution: SDL_Color.a is significant in SDL2
      palette[i].a = bmp->format->palette->colors[i].a;
      //log_debug("palette[%03d] = #%02X%02X%02X %02X", i,
      //  palette[i].r, palette[i].g, palette[i].b, palette[i].a);
    }

  gfx_palette_set_phys(palette);
  return 0;
}

/* Get a colors palette from the specified image */
int
gfx_palette_set_from_bmp(char *filename)
{
  SDL_Surface *bmp;
  int success = -1;
  char *fullpath = NULL;

  fullpath = paths_dmodfile(filename);
  bmp = IMG_Load(fullpath);
  free(fullpath);
  if (bmp == NULL)
    {
      fullpath = paths_fallbackfile(filename);
      bmp = IMG_Load(fullpath);
      free(fullpath);
      if (bmp == NULL)
	{
	  log_error("load_palette_from_bmp: couldn't open '%s': %s",
		    filename, SDL_GetError());
	  return -1;
	}
    }

  success = gfx_palette_set_from_surface(bmp);
  SDL_FreeSurface(bmp);
  return success;
}
