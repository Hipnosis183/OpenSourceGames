/**
 * FreeDink editor-specific code

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2011, 2012, 2014, 2015  Sylvain Beucler

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

#include <gettext.h>
#define _(String) gettext (String)

/* #define WIN32_LEAN_AND_MEAN */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* for tolower */
#include <ctype.h>
/* #include <io.h> */
/* #include <direct.h> */
#include <unistd.h>
#include <getopt.h>

#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL2_rotozoom.h"
#include "SDL2_framerate.h"

#include "app.h"
#include "AppFreeDinkedit.h"
#include "DMod.h"
#include "hardness_tiles.h"
#include "fastfile.h"
#include "IOGfxPrimitives.h"
#include "IOGfxDisplay.h"
#include "IOGfxSurface.h"
#include "ImageLoader.h"
#include "gfx.h"
#include "editor_screen.h"
#include "live_screen.h"
#include "live_sprites_manager.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "gfx_fonts.h"
#include "dinkini.h" /* check_seq_status */
#include "sfx.h"
#include "input.h"
#include "io_util.h"
#include "resources.h"
#include "paths.h"
#include "log.h"

//Dinkedit-only vars


/*
 * I count 10 modes in Dinkedit: map(1), screen tiles(3), screen
 * sprites(6), screen hardness initialization (9, switches to 8
 * immediately), screen hardness(8), tile hardness(4), sprite
 * chooser(5), tile chooser(2), sprite hardness editor(7), plus input
 * dialog(0). */
/* TODO: The goal is to split the big keybinding functions into these
 * modes, and in each mode, call a function instead of inlining the
 * code. And we may use 'else if', or even a hashmap to do the
 * bindings.
*/

/* Use constants for readability */
enum editor_mode {
	EDITOR_MODE_INIT,

	EDITOR_MODE_MINIMAP,

	EDITOR_MODE_TILE_PICKER,
	EDITOR_MODE_SPRITE_PICKER,

	EDITOR_MODE_SCREEN_TILES,
	EDITOR_MODE_SCREEN_SPRITES,
	EDITOR_MODE_SCREEN_HARDNESS,
	EDITOR_MODE_SCREEN_HARDNESS_INIT,

	EDITOR_MODE_TILE_HARDNESS,
	EDITOR_MODE_SPRITE_HARDNESS,
};
static enum editor_mode g_editorMode = EDITOR_MODE_INIT;

#define INPUT_MINIMAP_LOAD  30
#define INPUT_SCREEN_VISION 32
#define INPUT_SCREEN_MIDI   33
#define INPUT_SCREEN_TYPE   34

enum editor_buttons {
  EDITOR_ACTION_FIRST  = 0, // min constant for loops, like SDLKey
  EDITOR_ACTION_NOOP = 0,
  EDITOR_ACTION_ESCAPE = 1,
  EDITOR_ACTION_RETURN,
  EDITOR_ACTION_X,
  EDITOR_ACTION_Z,
  EDITOR_ACTION_TAB,
  EDITOR_ACTION_LAST // max+1 constant for loops
};

EditorMap buffmap;
/*bool*/int buf_mode = /*false*/0;
static char buf_path[100];
static int buf_map = 0;
static int cur_map;

static int draw_screen_tiny = -1;

/* Save x and y coordinates for mode 4, 5 and 6 */
static int m4x,m4y,m5x,m5y,m6x,m6y,m5ax,m5ay;

static int sp_base_walk = -1;
static int sp_base_idle = -1;
static int sp_base_attack = -1;
static int sp_base_hit = -1;
static int sp_base_die = -1;
static int sp_gold, sp_defense, sp_strength, sp_exp, sp_hitpoints;
static int sp_timer = 33;
static int sp_que;
static int sp_hard = 1;
static int sp_sound = 0;
static int sp_type = 1;
static int sp_is_warp = 0;
static int sp_warp_map = 0;
static /*bool*/int show_display = /*true*/1;
static int sp_picker = 0;
static int sp_nohit = 0;
static int sp_touch_damage = 0;
static int sp_warp_x = 0;
static int sp_warp_y = 0;
static int sp_parm_seq = 0;
static char sp_script[15];

static int old_command;
static int sp_cycle = 0;
static int cur_tileset;
static int selx = 1;
static int sely = 1;
static int last_mode = 0;
static int last_modereal = 0;
static int hold_warp_map, hold_warp_x,hold_warp_y;
static int sp_seq,sp_frame = 0;


/* const int NUM_SOUND_EFFECTS = 6; */
#define NUM_SOUND_EFFECTS 2

typedef enum enum_EFFECT
{
    SOUND_STOP = 0,
/*     SOUND_THROW, */
    SOUND_JUMP,
/*     SOUND_STUNNED, */
/*     SOUND_BEARSTRIKE, */
/*     SOUND_BEARMISS, */
} EFFECT;

static char *szSoundEffects[NUM_SOUND_EFFECTS] =
{
    "stop.wav",
/*     "THROW.WAV", */
    "jump.wav"
/*     "STUNNED.WAV", */
/*     "STRIKE02.WAV", */
/*     "MISS02.WAV" */
};


static int x = 640;
static int y = 480;

static char in_temp[200];
static int in_command;
static int in_onflag;
static unsigned int in_max = 10;
static int in_huh = 0;
static char in_default[200];
static int in_master = 0;

static int *in_int;
static char *in_string;
static int in_x, in_y;
static /*bool*/int in_enabled;

static int sp_mode;
static int sp_speed = 0;
static int sp_brain = 0;
static /*bool*/int sp_screenmatch = 0;
static int hard_tile = 0;
static int last_sprite_added = 0;
static int map_vision = 0;

static int m1x,m1y;
static int m2x,m2y;
static int m3x,m3y;
static int cur_tile;


/**
 * Append a line of text at the end of a file
 */
static void add_text(char *text, char *filename)
{
  if (strlen(text) < 1)
    return;
  
  FILE *fp = paths_dmodfile_fopen(filename, "ab");
  if (fp != NULL)
    {
      fwrite(text, strlen(text), 1, fp);
      fclose(fp);
    }
  else
    {
      perror("add_text");
    }
}

/**
 * Get the screen tile under coords x,y
 */
int xy2screentile(int x, int y)
{
  return
    ((y + 1) * 12) / 50
    + x / 50;
}



// PROC NAMES

void draw_screen_editor(void);
void draw_minimap(void);
/* void dderror(HRESULT hErr); */

static bool skip_screen_clipping() {
	return (g_editorMode == EDITOR_MODE_MINIMAP || g_editorMode == EDITOR_MODE_SPRITE_PICKER) && draw_screen_tiny < 1;
}

void draw_sprite(IOGfxSurface* IOGFX_lpdest, int h) {
  rect box_crap,box_real;
/*   HRESULT             ddrval; */
/*   DDBLTFX     ddbltfx; */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */

  if (get_box(h, &box_crap, &box_real, skip_screen_clipping()))
    {
/*       while(1) */
/* 	{ */
/* 	again: */
/* 	  ddrval = lpdest->Blt(&box_crap, k[getpic(h)].k, */
/* 			       &box_real, DDBLT_KEYSRC, &ddbltfx); */

	  // GFX
	  {
	    SDL_Rect src, dst;
	    src.x = box_real.left;
	    src.y = box_real.top;
	    src.w = box_real.right - box_real.left;
	    src.h = box_real.bottom - box_real.top;
	    dst.x = box_crap.left;
	    dst.y = box_crap.top;
	    dst.w = box_crap.right - box_crap.left;
	    dst.h = box_crap.bottom - box_crap.top;
	    IOGFX_lpdest->blitStretch(GFX_k[getpic(h)].k, &src, &dst);
	  }

/* 	  if (ddrval != DD_OK) */
/* 	    { */
/* 	      if (ddrval == DDERR_WASSTILLDRAWING) */
/* 		goto again; */

/* 	      //dderror(ddrval); */
/* 	      dderror(ddrval); */
/* 	      if (draw_screen_tiny > 0)  */
/* 		{ */
/* 		  Msg("MainSpriteDraw(): Could not draw sprite %d, pic %d. (Seq %d, Fram %d) (map %d)",h,getpic(h),spr[h].pseq, spr[h].pframe, draw_screen_tiny); */
/* 		  Msg("Box_crap: %d %d %d %d, Box_real: %d %d %d %d",box_crap.left,box_crap.top, */
/* 		      box_crap.right, box_crap.bottom,box_real.left,box_real.top, */
/* 		      box_real.right, box_real.bottom); */
/* 		} */
/* 	      else */
/* 		{ */
/* 		  Msg("MainSpriteDraw(): Could not draw sprite %d, pic %d. (map %d)",h,getpic(h), cur_map); */
/* 		} */
/* 	      check_sprite_status(h); */
/* 	      break; */
/* 	    } */
/* 	  break; */
/* 	} */
    }
}


/**
 * Draw all screen sprites, ordered by queue.
 * 
 * Also cf. game's game_place_sprites(...) and
 * game_place_sprites_background(...).
 */
void place_sprites()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_editor_sprites(rank);
  
  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (cur_ed_screen.sprite[j].active == 1
	  && (cur_ed_screen.sprite[j].vision == 0 || cur_ed_screen.sprite[j].vision == map_vision))
	{
	  //we have instructions to make a sprite
	  
	  if (cur_ed_screen.sprite[j].type == 0 || cur_ed_screen.sprite[j].type == 2)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);

	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      check_sprite_status(sprite);
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[j].alt);
	      
	      if (cur_ed_screen.sprite[j].type == 0)
		draw_sprite(IOGFX_background, sprite);
	      
	      if (spr[sprite].hard == 0)
		{
		  if (cur_ed_screen.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1);
		  else add_hardness(sprite, 100 + j);
		}
	      
	      lsm_remove_sprite(sprite);
	    }
	  
	  if (cur_ed_screen.sprite[j].type == 1)
	    {
	      //make it a living sprite
	      
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);
	      
	      spr[sprite].que = cur_ed_screen.sprite[j].que;
	      check_sprite_status(sprite);
	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      
	      rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[j].alt);
	      
	      if (spr[sprite].hard == 0)
		{
		  if (cur_ed_screen.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else add_hardness(sprite,100+j);
		}
	    }
	}
    }
}

void editor_save_screen(int num) {
  save_screen(g_dmod.map.map_dat.c_str(), num);
}

void editor_load_screen(int num)
{
  if (load_screen_to(g_dmod.map.map_dat.c_str(), num, &cur_ed_screen) < 0)
    return;
  fill_whole_hard();
}

/* Draw background from tiles */
void draw_screen_editor(void)
{
  IOGFX_background->fillRect(NULL, 0,0,0);
  fill_whole_hard();

  lsm_kill_all_nonlive_sprites();

  gfx_tiles_draw_screen(g_dmod.bgTilesets.slots, cur_ed_screen.t);
  place_sprites();
}




/**
 * Draw the currently selected tile square (in the bottom-right corner
 * of the screen)
 */
void draw_current()
{
  int srctileset_idx0 = cur_tile / 128;
  int srctile_square_idx0 = cur_tile % 128;

  SDL_Rect src;
  int srctile_square_x = srctile_square_idx0 % GFX_TILES_SCREEN_W;
  int srctile_square_y = srctile_square_idx0 / GFX_TILES_SCREEN_W;
  src.x = srctile_square_x * GFX_TILES_SQUARE_SIZE;
  src.y = srctile_square_y * GFX_TILES_SQUARE_SIZE;
  src.w = GFX_TILES_SQUARE_SIZE;
  src.h = GFX_TILES_SQUARE_SIZE;
  
  SDL_Rect dst = {590, 430};
  IOGFX_background->blit(g_dmod.bgTilesets.slots[srctileset_idx0 + 1], &src, &dst);
}


/* Draw harness. */
void drawallhard( void)
{
/*   rect box_crap; */
/*   int ddrval; */
/*   DDBLTFX     ddbltfx; */
  int x1, y1;

  /* TODO: test me! Then factor the code */
  for (x1=0; x1 < 600; x1++)
    for (y1=0; y1 < 400; y1++)
      {
	if (screen_hitmap[x1][y1] == 1)
	  {
/* 	    ddbltfx.dwFillColor = 1; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[1].r, GFX_ref_pal[1].g, GFX_ref_pal[1].b);
	    }
	  }

	if (screen_hitmap[x1][y1] == 2)
	  {
/* 	    ddbltfx.dwFillColor = 128; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[128].r, GFX_ref_pal[128].g, GFX_ref_pal[128].b);
	    }
	  }

	if (screen_hitmap[x1][y1] == 3)
	  {
/* 	    ddbltfx.dwFillColor = 45; */
/* 	    ddbltfx.dwSize = sizeof(ddbltfx); */
/* 	    box_crap.top = y1; */
/* 	    box_crap.bottom = y1+1; */
/* 	    box_crap.left = x1+playl; //20 is to compensate for the border */
/* 	    box_crap.right = x1+1+playl; */
/* 	    ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 	    if (ddrval != DD_OK) Msg("There was an error!"); */
	    // GFX
	    {
	      SDL_Rect GFX_box_crap;
	      GFX_box_crap.x = x1 + playl;
	      GFX_box_crap.y = y1;
	      GFX_box_crap.w = 1;
	      GFX_box_crap.h = 1;
	      IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[45].r, GFX_ref_pal[45].g, GFX_ref_pal[45].b);
	    }
	  }

	if (screen_hitmap[x1][y1] > 100)
	  {

	    if (cur_ed_screen.sprite[  (screen_hitmap[x1][y1]) - 100].is_warp == 1)
	      {
		//draw a little pixel
/* 		ddbltfx.dwFillColor = 20; */
/* 		ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		box_crap.top = y1; */
/* 		box_crap.bottom = y1+1; */
/* 		box_crap.left = x1+playl; //20 is to compensate for the border */
/* 		box_crap.right = x1+1+playl; */
/* 		ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 		if (ddrval != DD_OK) Msg("There was an error!"); */
		// GFX
		{
		  SDL_Rect GFX_box_crap;
		  GFX_box_crap.x = x1 + playl;
		  GFX_box_crap.y = y1;
		  GFX_box_crap.w = 1;
		  GFX_box_crap.h = 1;
		  IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[20].r, GFX_ref_pal[20].g, GFX_ref_pal[20].b);
		}
	      }
	    else
	      {
		//draw a little pixel
/* 		ddbltfx.dwFillColor = 23; */
/* 		ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		box_crap.top = y1; */
/* 		box_crap.bottom = y1+1; */
/* 		box_crap.left = x1+playl; //20 is to compensate for the border */
/* 		box_crap.right = x1+1+playl; */
/* 		ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
/* 		if (ddrval != DD_OK) Msg("There was an error!"); */
		// GFX
		{
		  SDL_Rect GFX_box_crap;
		  GFX_box_crap.x = x1 + playl;
		  GFX_box_crap.y = y1;
		  GFX_box_crap.w = 1;
		  GFX_box_crap.h = 1;
		  IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[23].r, GFX_ref_pal[23].g, GFX_ref_pal[23].b);
		}
	      }
	  }
      }
}

/* Edit a tile hardness - show semi-transparent red/blue/orange
   squares on pixels with different kinds of hardness */
void draw_hard( void)
{
 //RECT                rcRect;
  int x;
  for (x = 0; x < 50; x++)
    {
      int y;
      for (y = 0; y < 50; y++)
	{
	  /* red */
	  if (hmap.htile[hard_tile].hm[x][y] == 1)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9), y*9, k[seq[10].frame[2]].k, */
/* 				 &k[seq[10].frame[2]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		IOGFX_backbuffer->blit(GFX_k[seq[10].frame[2]].k, NULL, &dst);
	      }
	    }

	  /* blue */
	  if (hmap.htile[hard_tile].hm[x][y] == 2)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9),y*9, k[seq[10].frame[9]].k, */
/* 				 &k[seq[10].frame[9]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		IOGFX_backbuffer->blit(GFX_k[seq[10].frame[9]].k, NULL, &dst);
	      }
	    }

	  /* orange */
	  if (hmap.htile[hard_tile].hm[x][y] == 3)
	    {
/* 	      lpDDSBack->BltFast(95+(x*9),y*9, k[seq[10].frame[10]].k, */
/* 				 &k[seq[10].frame[10]].box, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = 95 + x*9;
		dst.y = y*9;
		IOGFX_backbuffer->blit(GFX_k[seq[10].frame[10]].k, NULL, &dst);
	      }
	    }
	}
    }
}


void
draw_this_map(EditorMap* pmap)
{
  int x;
  for (x = 0; x < 768; x++)
    {
      /* Blue square - unused screen */
      if (pmap->loc[x+1] == 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[6]].k, */
/* 			    &k[seq[10].frame[6]].box, DDBLTFAST_NOCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    IOGFX_background->blitNoColorKey(GFX_k[seq[10].frame[6]].k, NULL, &dst);
	  }
	}
      /* Red square - used screen */
      if (pmap->loc[x+1] > 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[7]].k, */
/* 			    &k[seq[10].frame[7]].box, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    IOGFX_background->blitNoColorKey(GFX_k[seq[10].frame[7]].k, NULL, &dst);
	  }
	}

      /* M mark - screen has MIDI */
      if (pmap->music[x+1] != 0)
	{
/* 	  lpDDSTwo->BltFast((x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[12]].k, */
/* 			    &k[seq[10].frame[12]].box, DDBLTFAST_SRCCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    IOGFX_background->blit(GFX_k[seq[10].frame[12]].k, NULL, &dst);
	  }
	}
      /* S mark - screen has screentype / is indoor */
      if (pmap->indoor[x+1] != 0)
	{
/* 	  lpDDSTwo->BltFast( (x) * 20 - ((x / 32) * 640), (x / 32) * 20, k[seq[10].frame[13]].k, */
/*            &k[seq[10].frame[13]].box, DDBLTFAST_SRCCOLORKEY| DDBLTFAST_WAIT ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x * 20 - x/32*640;
	    dst.y = x/32 * 20;
	    IOGFX_background->blit(GFX_k[seq[10].frame[13]].k, NULL, &dst);
	  }
	}
    }
}

/* Draw all squares in the map picker mode, including the 'M' (midi)
   and 'S' (screentype) marks */
void draw_minimap(void)
{
  /*box_crap.top = spr[h].y + k[spr[h].pic].hardbox.top;
    box_crap.bottom = spr[h].y + k[spr[h].pic].hardbox.bottom;
    box_crap.left = spr[h].x + k[spr[h].pic].hardbox.left;
    box_crap.right = spr[h].x + k[spr[h].pic].hardbox.right;
  */
  //lpDDSBack->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL, &ddbltfx);
  draw_this_map(&g_dmod.map);
}




/*bool*/int load_screen_buf(const int num)
{
  char mapdat_path[120];
  /* TODO: Dinkedit historically loads map with a filename relative to
     the current D-Mod directory. Maybe change that to handle absolute
     paths and paths relative to the refdir. */
  sprintf(mapdat_path, "%sMAP.DAT", buf_path);
  load_screen_to(mapdat_path, num, &cur_ed_screen);
  
  return /*true*/1;
}

void load_info_buff(void)
{
  char dinkdat_path[120];

  sprintf(dinkdat_path, "%sDINK.DAT", buf_path);

  buffmap.dink_dat = dinkdat_path;
  if (!buffmap.load())
    {
      log_error("World not found in %s.", buf_path);
      buf_mode = /*false*/0;
    }
  else
    {
      log_info("World data loaded.");
      buf_mode = /*true*/1;
    }
}

/* draw_minimap() but on a different map ('L' in map picker mode) */
void draw_minimap_buff(void)
{
  load_info_buff();

  if (!buf_mode)
    {
      //failed
      /* TODO: display error message to the user */
      draw_minimap();
      return;
    }

  draw_this_map(&buffmap);
}


/**
 * Add new screen to the map
 */
int add_new_map()
{
  int loc_max = 0;
  for (int i = 0; i < 768+1; i++)
    if (g_dmod.map.loc[i] > loc_max)
      loc_max = g_dmod.map.loc[i];
  
  int loc_new = loc_max + 1;
  if (loc_new > 768)
    return -1;

  editor_save_screen(loc_new);
  return loc_new;
}


void show_text_box(int enabled) {
  if (enabled) {
    in_enabled = 1;
    SDL_EventState(SDL_TEXTINPUT, SDL_ENABLE);
  } else {
    in_enabled = 0;
    SDL_EventState(SDL_TEXTINPUT, SDL_IGNORE);
  }
}


/* Update mouse position with relative motion coordinates */
void freedinkedit_update_cursor_position(SDL_Event* ev)
{
  if (ev->type != SDL_MOUSEMOTION)
    return;

  int dx = ev->motion.xrel;
  int dy = ev->motion.yrel;

  sp_cycle = 0;
  spr[1].x += dx;
  spr[1].y += dy;
}

void check_joystick()
{
  /* Clean-up */
  /* Actions */
  {
    int a = EDITOR_ACTION_FIRST;
    for (a = EDITOR_ACTION_FIRST; a < EDITOR_ACTION_LAST; a++)
      sjoy.joybit[a] = 0;
  }

  /* Arrows */
  sjoy.right = 0;
  sjoy.left = 0;
  sjoy.up = 0;
  sjoy.down = 0;

  if (joystick)
    {
      SDL_JoystickUpdate();
      Sint16 x_pos = 0, y_pos = 0;
      /* SDL counts buttons from 0, not from 1 */
      int i = 0;
      for (i = 0; i < NB_BUTTONS; i++)
	if (SDL_JoystickGetButton(jinfo, i))
	  {
	    if (i == 1-1)
	      sjoy.joybit[EDITOR_ACTION_ESCAPE] = 1;
	    else if (i == 2-1)
	      sjoy.joybit[EDITOR_ACTION_RETURN] = 1;
	    else if (i == 3-1)
	      sjoy.joybit[EDITOR_ACTION_X] = 1;
	    else if (i == 4-1)
	      sjoy.joybit[EDITOR_ACTION_Z] = 1;
	    else if (i == 5-1)
	      sjoy.joybit[EDITOR_ACTION_TAB] = 1;
	  }

      x_pos = SDL_JoystickGetAxis(jinfo, 0);
      y_pos = SDL_JoystickGetAxis(jinfo, 1);
      /* Using thresold=10% (original game) is just enough to get rid
	 of the noise. Let's use 30% instead, otherwise Dink will go
	 diags too easily. */
      {
	Sint16 threshold = 32767 * 30/100;
	if (x_pos < -threshold) sjoy.left  = 1;
	if (x_pos > +threshold) sjoy.right = 1;
	if (y_pos < -threshold) sjoy.up    = 1;
	if (y_pos > +threshold) sjoy.down  = 1;
      }
    }

  if (input_getscancodestate(SDL_SCANCODE_ESCAPE)) sjoy.joybit[EDITOR_ACTION_ESCAPE] = 1;
  if (input_getscancodestate(SDL_SCANCODE_RETURN)) sjoy.joybit[EDITOR_ACTION_RETURN] = 1;
  if (input_getcharstate(SDLK_x)) sjoy.joybit[EDITOR_ACTION_X] = 1;
  if (input_getcharstate(SDLK_z)) sjoy.joybit[EDITOR_ACTION_Z] = 1;
  if (input_getscancodestate(SDL_SCANCODE_TAB)) sjoy.joybit[EDITOR_ACTION_TAB] = 1;
  if (input_getscancodestate(SDL_SCANCODE_RIGHT)) sjoy.right = 1;
  if (input_getscancodestate(SDL_SCANCODE_LEFT)) sjoy.left = 1;
  if (input_getscancodestate(SDL_SCANCODE_DOWN)) sjoy.down = 1;
  if (input_getscancodestate(SDL_SCANCODE_UP)) sjoy.up = 1;
  
  {
    int a = EDITOR_ACTION_FIRST;
    for (a = EDITOR_ACTION_FIRST; a < EDITOR_ACTION_LAST; a++)
      {
	sjoy.button[a] = 0;
	if (sjoy.joybit[a] && sjoy.joybitold[a] == 0)
	  /* Button was just pressed */
	  sjoy.button[a] = 1;
	sjoy.joybitold[a] = sjoy.joybit[a];
      }
  }
}


/* Displays a tile fullscreen, so we can select some squares */
void loadtile(int tileset)
{
/*   DDBLTFX     ddbltfx; */
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */

  //feel tile background with a color

/*   ddbltfx.dwFillColor = 0; */
/*   ddbltfx.dwSize = sizeof(ddbltfx); */
/*   lpDDSTwo->Blt(NULL,NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  IOGFX_background->fillRect(NULL, 0,0,0);

  spr[1].seq = 3; spr[1].seq_orig = 3;
  //  if (mode == 3)
  if (g_editorMode == EDITOR_MODE_SCREEN_TILES)
    {
      m3x = spr[1].x; m3y = spr[1].y;
      spr[1].x = m2x; spr[1].y = m2y; g_editorMode = EDITOR_MODE_TILE_PICKER;
      spr[1].speed = 50;
    }

  //  if (mode == 4)
  if (g_editorMode == EDITOR_MODE_TILE_HARDNESS)
    {
      spr[1].x = m2x; spr[1].y = m2y; g_editorMode = EDITOR_MODE_TILE_PICKER;
      spr[1].speed = 50;
    }

/*   lpDDSTwo->BltFast(0, 0, tiles[tileset], &tilerect[tileset], DDBLTFAST_NOCOLORKEY |DDBLTFAST_WAIT); */
  // GFX
  IOGFX_background->blit(g_dmod.bgTilesets.slots[tileset], NULL, NULL);
  cur_tileset = tileset;

  last_mode = tileset;

  lsm_kill_all_nonlive_sprites();
}


int sp_get( int num)
{

int t = 1;
 int j;
	for (j = 1; j < MAX_SEQUENCES; j++)
	{
     check_frame_status(j, 1);

		if (seq[j].frame[1] != 0)
	 {
       if (t == num)
	   {


		   return(j);

	   }
		   t++;

	 }



	}

return(0);

}

/* In sprite picking mode, draw a grid to separate sprites or
   sequences' 50x50 previews */
static void draw_sprite_picker_grid(void)
{
	for (int x = 1; x <= 12; x++)
		IOGFX_background->vlineRGB(x*50-1, 0,400, 123,132,99);
	for (int y = 1; y <= 8; y++)
		IOGFX_background->hlineRGB(0,600, y*50-1, 123,132,99);
}

/* Main sprite selection screen - display a page of the sequence
   selector */
void draw15(int num)
{
/*   int crap; */
/*   DDBLTFX ddbltfx; */
/*   rect  crapRec, Rect, box_crap; */
  int frame;
/*   int ddrval; */
  int se;
/*   int dd; */
  int x1;
  //get_sp_seq(2);

  lsm_kill_all_nonlive_sprites();


/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
/*   crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  IOGFX_background->fillRect(NULL, 0,0,0);

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */

  Say("Please wait, loading sprite data into SmartCache system...", 147,160);

  g_display->flipStretch(IOGFX_backbuffer);

  /* Draw sprites */
  for (x1 = 0; x1 <= 11; x1++)
    {
      int y1;
      for (y1=0; y1 <= 7; y1++)
	{
	  num++;

	  se = sp_get(num);
	  check_seq_status(se);

	  if (se > 0)
	    {
	      frame = 1;

/* 	      Rect.left = x1 * 50; */
/* 	      Rect.top = y1 * 50; */
/* 	      Rect.right = Rect.left + 50; */
/* 	      Rect.bottom = Rect.top + 50; */

/* 	      crapRec = k[seq[se].frame[frame]].box; */

/* 	      dd = lpDDSTwo->Blt(&Rect, k[seq[se].frame[frame]].k, */
/* 				 &crapRec, DDBLT_KEYSRC | DDBLT_DDFX | DDBLT_WAIT, &ddbltfx); */
	      // GFX
	      {
		SDL_Rect dst;
		dst.x = x1 * 50;
		dst.y = y1 * 50;
		dst.w = 50;
		dst.h = 50;
		IOGFX_background->blitStretch(GFX_k[seq[se].frame[frame]].k, NULL, &dst);
	      }


/* 	      if (dd != DD_OK) Msg("Error with drawing sprite! Seq %d, Spr %d.", se, frame); */
	    }
	}
    }
  draw_sprite_picker_grid();
}

/* Inner sprite selection screen - display all frames/sprites in a
   sequence */
void draw96(int def)
{
/*   int crap; */
/*   DDBLTFX ddbltfx; */
/*   rect crapRec, Rect, box_crap; */
/*   int ddrval; */
  int se;
/*   int dd; */
  int x1;

  //get_sp_seq(2);
  se = sp_seq;
  check_seq_status(se);
  int num = 0;
  lsm_kill_all_nonlive_sprites();

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */
/*   ddbltfx.dwFillColor = 0; */
/*   crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
  // GFX
  IOGFX_background->fillRect(NULL, 0,0,0);

/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof( ddbltfx); */

  for (x1=0; x1 <= 11; x1++)
    {
      int y1;
      for (y1=0; y1 <= 7; y1++)
	{
	  num++;
	  if (seq[se].frame[num] == 0)
	    {
	      //all done displaying
	      goto pass;
	    }
	  //se = sp_get(num);

/* 	  Rect.left = x1 * 50; */
/* 	  Rect.top = y1 * 50; */
/* 	  Rect.right = Rect.left + 50; */
/* 	  Rect.bottom = Rect.top + 50; */

	  if (def > 0 && num == def)
	    {
	      //set default location to sprite they are holding
	      spr[1].x = x1*50;
	      spr[1].y = y1 *50;
	    }

/* 	  crapRec = k[seq[se].frame[num]].box; */

/* 	  dd = lpDDSTwo->Blt(&Rect, k[seq[se].frame[num]].k, */
/* 			     &crapRec, DDBLT_KEYSRC | DDBLT_DDFX | DDBLT_WAIT, &ddbltfx ); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = x1 * 50;
	    dst.y = y1 * 50;
	    dst.w = 50;
	    dst.h = 50;
	    IOGFX_background->blitStretch(GFX_k[seq[se].frame[num]].k, NULL, &dst);
	  }

/* 	  if (dd != DD_OK) Msg("Error with drawing sprite! Seq %d, Spr %d.",se,frame); */
	}
    }

 pass:
  draw_sprite_picker_grid();
}


void sp_add( void )
{
  int j;
	for (j = 1; j < 100; j++)
	{
		if (cur_ed_screen.sprite[j].active == /*false*/0)
		{

			last_sprite_added = j;
			//Msg("Adding sprite %d, seq %d, frame %d.",j,sp_seq,sp_frame);
			cur_ed_screen.sprite[j].active = /*true*/1;
            cur_ed_screen.sprite[j].frame = sp_frame;
            cur_ed_screen.sprite[j].seq = sp_seq;
			cur_ed_screen.sprite[j].x = spr[1].x;
			cur_ed_screen.sprite[j].y = spr[1].y;
			cur_ed_screen.sprite[j].size = spr[1].size;
			cur_ed_screen.sprite[j].type = sp_type;
			cur_ed_screen.sprite[j].brain = sp_brain;
			cur_ed_screen.sprite[j].speed = sp_speed;
			cur_ed_screen.sprite[j].base_walk = sp_base_walk;
			cur_ed_screen.sprite[j].base_idle = sp_base_idle;
			cur_ed_screen.sprite[j].base_attack = sp_base_attack;
			cur_ed_screen.sprite[j].base_hit = sp_base_hit;
			cur_ed_screen.sprite[j].timing = sp_timer;
			cur_ed_screen.sprite[j].que = sp_que;
			cur_ed_screen.sprite[j].hard = sp_hard;
			cur_ed_screen.sprite[j].is_warp = sp_is_warp;
			cur_ed_screen.sprite[j].warp_map = sp_warp_map;
			cur_ed_screen.sprite[j].warp_x = sp_warp_x;
			cur_ed_screen.sprite[j].warp_y = sp_warp_y;
			cur_ed_screen.sprite[j].parm_seq = sp_parm_seq;
			strcpy(cur_ed_screen.sprite[j].script, sp_script);
			cur_ed_screen.sprite[j].base_die = sp_base_die;
			cur_ed_screen.sprite[j].gold = sp_gold;
			cur_ed_screen.sprite[j].exp = sp_exp;
			cur_ed_screen.sprite[j].strength = sp_strength;
			cur_ed_screen.sprite[j].defense = sp_defense;
			cur_ed_screen.sprite[j].hitpoints = sp_hitpoints;
			cur_ed_screen.sprite[j].sound = sp_sound;
		    cur_ed_screen.sprite[j].vision = map_vision;
		    cur_ed_screen.sprite[j].nohit = sp_nohit;
			cur_ed_screen.sprite[j].touch_damage = sp_touch_damage;
			rect_copy(&cur_ed_screen.sprite[j].alt , &spr[1].alt);
		    return;
		}



	}


}


void smart_add(void)
{




	sp_add();

if (sp_screenmatch)
{

if (spr[1].size == 100)
{
//lets see if the sprite crosses into another screen
//use screenmatch technology

	if ((spr[1].x - k[getpic(1)].xoffset) + k[getpic(1)].box.right > 620)
	{
	log_debug("need to add it to the right");

		//need to add it to the right
		if (g_dmod.map.loc[cur_map+1] > 0)
		{
        	editor_save_screen(g_dmod.map.loc[cur_map]);
          log_debug("loading right map");
			editor_load_screen(g_dmod.map.loc[cur_map+1]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].x -= 600;
			sp_add();
			editor_save_screen(g_dmod.map.loc[cur_map+1]);
			editor_load_screen(g_dmod.map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].x - k[getpic(1)].xoffset) - k[getpic(1)].box.right < 20)
	{
	log_debug("need to add it to the right");

		//need to add it to the left
		if (g_dmod.map.loc[cur_map-1] > 0)
		{
        	editor_save_screen(g_dmod.map.loc[cur_map]);
          log_debug("loading right map");
			editor_load_screen(g_dmod.map.loc[cur_map-1]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].x += 600;
			sp_add();
			editor_save_screen(g_dmod.map.loc[cur_map-1]);
			editor_load_screen(g_dmod.map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].y - k[getpic(1)].yoffset) + k[getpic(1)].box.bottom > 400)
	{
	log_debug("need to add it to the bottom");

		//need to add it to the bottom
		if (g_dmod.map.loc[cur_map+32] > 0)
		{
        	editor_save_screen(g_dmod.map.loc[cur_map]);
          log_debug("loading bottom ");
			editor_load_screen(g_dmod.map.loc[cur_map+32]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].y -= 400;
			sp_add();
			editor_save_screen(g_dmod.map.loc[cur_map+32]);
			editor_load_screen(g_dmod.map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}


	if ((spr[1].y - k[getpic(1)].yoffset) - k[getpic(1)].box.bottom < 0)
	{
	log_debug("need to add it to the top");

		//need to add it to the left
		if (g_dmod.map.loc[cur_map-32] > 0)
		{
        	editor_save_screen(g_dmod.map.loc[cur_map]);
          log_debug("loading top map");
			editor_load_screen(g_dmod.map.loc[cur_map-32]);
               int holdx = spr[1].x;
               int holdy = spr[1].y;
			   spr[1].y += 400;
			sp_add();
			editor_save_screen(g_dmod.map.loc[cur_map-32]);
			editor_load_screen(g_dmod.map.loc[cur_map]);
           spr[1].x = holdx;
		   spr[1].y = holdy;
		}
	}

}

}





}





void blit(int seq1, int frame, IOGfxSurface* IOGFX_lpdest, int tx, int ty)
{
/* RECT math; */

/* 	math = k[seq[seq1].frame[frame]].box; */
/* 	OffsetRect(&math, tx, ty); */
/* 	ddrval = lpdest->Blt(&math, k[seq[seq1].frame[frame]].k, &k[seq[seq1].frame[frame]].box, DDBLT_WAIT, NULL); */
	// GFX
	// No scaling needed here
	{
	  SDL_Rect dst;
	  dst.x = tx;
	  dst.y = ty;
	  IOGFX_lpdest->blit(GFX_k[seq[seq1].frame[frame]].k, NULL, &dst);
	}
}


/**
 * Display the appropriate input dialog
 */
void check_in(void)
{
  in_huh = in_master;

  /**
   * - Sprite dialogs:
   *  1 = size
   *  2 = type (background|sprite)
   *  3 = brain
   *  4 = speed
   *  5 = timer (timing)
   *  6 = base_walk
   *  7 = base_idle
   *  8 = que
   *  9 = hard (hardness_type - background|sprite)
   * 10 = is_warp
   * 11 = warp_map
   * 12 = warp_x
   * 13 = warp_y
   * 14 = parm_seq
   * 15 = script
   * 16 = base_die
   * 17 = sound (looping sound)
   * 18 = hitpoints
   * 19 = nohit
   * 20 = touch_damage
   * 21 = base_attack
   * 22 = defense
   * 
   * - Minimap dialogs:
   * 30 = 'L' - load screen from another map
   * 
   * - Screen dialogs:
   * (called from minimap or screen mode)
   * 31 = 'B' - screen script
   * 32 = 'V' - vision
   * (called from minimap mod)
   * 33 = 'M' - screen midi
   * 34 = 'S' - screen type (indoors / outside)
   **/

  /* In: */
  /* in_master = identifier for the property we need to edit */
  /* Out: */
  /* in_max = maximum length of input (as text) + 1 */
  /* in_command = data type (int|string) */
  /* in_int =  pointer to the integer value to update */
  /* in_string = pointer to the string to update */
  /* in_onflag = trigger the creation of an input window */
  /* in_enabled = text box input mode */

  if (in_master == 1)
    {
      in_command = 1; //number
      in_int = &spr[1].size;
      in_max = 10; //max _length
      sprintf(in_default,"%d",spr[1].size); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Size?",260,175);
    }

  if (in_master == 2)
    {
      in_command = 1; //number
      in_int = &sp_type;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_type); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Type?",260,175);
      Say("Type controls the sprite's basic type - 0 means it is ornamental only"
	  "(cannot walk behind or think) 1 - means normal sprite.  (for a tree or person)"
	  ,10,10);
    }

  if (in_master == 3)
    {
      in_command = 1; //number
      in_int = &sp_brain;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_brain); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Brain?",260,175);
      Say("Brains are a predefined way for this sprite to behave. 0 = No movement, 1 = Dink,"
	  " 2 = Dumb Sprite Bouncer, 3 = Duck, 4 = Pig, 6 = repeat, 7 = one loop then kill,"
	  " 9 = monster (all diag), 10 = monster(no diag)"
	  ,10,10);
    }

  if (in_master == 4)
    {
      in_command = 1; //number
      in_int = &sp_speed;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_speed); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Speed?",260,175);
      Say("Speed rating allows you to adjust how fast a certain sprite moves.  Works with"
	  " most brains."
	  ,10,10);
    }

  if (in_master == 5)
    {
      in_command = 1; //number
      in_int = &sp_timer;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_timer); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Timing?",260,175);
      Say("This is the delay the CPU waits before processing the sprite after each cycle.  "
	  "(in thousands of a second - so 33 would mean 30 times a second)"
	  ,10,10);
    }

  if (in_master == 6)
    {
      in_command = 1; //number
      in_int = &sp_base_walk;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_base_walk); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Base Walk?",260,175);
      Say("The base for which the CPU adds 1 through 9 to make the sprite move, depending on"
	  " direction.  Must be a multiple of ten. (ie, 20 to look like a duck, 40 to look like a pig)"
	  ,10,10);
    }

  if (in_master == 7)
    {
      in_command = 1; //number
      in_int = &sp_base_idle;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_base_idle); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Base Idle?",260,175);
      Say("Some brains can optionally use extra sprites for their \'idle\' pose."
	  ,10,10);
    }

  if (in_master == 8)
    {
      in_command = 1; //number
      in_int = &sp_que;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_que); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Depth Que?",250,175);
      Say("From 1 to 20000, 0 for default.  (defaults to y cord)"
	  ,10,10);
    }

  if (in_master == 9)
    {
      in_command = 1; //number
      in_int = &sp_hard;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_hard); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Hardness?",260,175);
      Say("Sets how hardness works.  1 means normal, (monsters) 0 means added to background. (walls, trees)"
	  ,10,10);
    }

  if (in_master == 10)
    {
      in_command = 1; //number
      in_int = &sp_is_warp;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_is_warp); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("New Properties?",260,175);
      Say("Sets special properties for the hardblock.  0 = normal (just hard) 1 = warp."
	  ,10,10);
    }

  if (in_master == 11)
    {
      in_command = 1; //number
      in_int = &sp_warp_map;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_warp_map); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("Warp Map #",260,175);
      Say("These parms are valid if the hard block property setting is 1.  (warp)"
	  ,10,10);
    }

  if (in_master == 12)
    {
      in_command = 1; //number
      in_int = &sp_warp_x;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_warp_x); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("Warp X:",260,175);
      Say("The X location to warp to.  (20 to 619)"
	  ,10,10);
    }

  if (in_master == 13)
    {
      in_command = 1; //number
      in_int = &sp_warp_y;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_warp_y); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("Warp Y:",260,175);
      Say("The Y location to warp to.  (0 to 499)"
	  ,10,10);
    }

  if (in_master == 14)
    {
      in_command = 1; //number
      in_int = &sp_parm_seq;
      in_max = 10; //max _length
      sprintf(in_default,"%d",sp_parm_seq); //set default
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("Sequence:",260,175);
      Say("This parm is used by some brains/settings if set.  A sequence is an animation #."
	  ,10,10);
    }

  if (in_master == 15)
    {
      in_command = 2; //string
      sprintf(in_default, "%s", sp_script);
      in_max = 13;
      in_string = sp_script;
      blit(30,1,IOGFX_backbuffer,250,170);
      Say("Script:",260,175);
      Say("Filename of script this sprite uses."
	  ,10,10);
    }

    if (in_master == 16)
      {
	in_command = 1; //number
	in_int = &sp_base_die;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_base_die); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Base Death:",260,175);
	Say("If this sprite dies, this will be used."
	    ,10,10);
      }

    if (in_master == 17)
      {
	in_command = 1; //number
	in_int = &sp_sound;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_sound); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Sound:",260,175);
	Say("This sprite will play this sound looped until it dies."
	    ,10,10);
      }

    if (in_master == 18)
      {
	in_command = 1; //number
	in_int = &sp_hitpoints;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_hitpoints); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Hitpoints:",260,175);
	Say("How strong is this creature?  (0 = not alive/invincible)"
	    ,10,10);
      }

    if (in_master == 19)
      {
	in_command = 1; //number
	in_int = &sp_nohit;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_nohit); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Nohit:",260,175);
	Say("Can this be punched? 0 if yes.  Either way it will"
	    "still check for hit() if a script is attached."
	    ,10,10);
      }

    if (in_master == 20)
      {
	in_command = 1; //number
	in_int = &sp_touch_damage;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_touch_damage); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Touch Damage:",260,175);
	Say("If not 0, the hardbox of this sprite will cause this"
	    "much damage if touched."
	    ,10,10);
      }

    if (in_master == 21)
      {
	in_command = 1; //number
	in_int = &sp_base_attack;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_base_attack); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Base Attack:",260,175);
	Say("If not -1, this monster can attack with this sprite base. (base + dir)"
	    ,10,10);
      }

    if (in_master == 22)
      {
	in_command = 1; //number
	in_int = &sp_defense;
	in_max = 10; //max _length
	sprintf(in_default,"%d",sp_defense); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Defense:",260,175);
	Say("This will be deducted from any attack."
	    ,10,10);
      }

    if (in_master == INPUT_MINIMAP_LOAD)
      {
	in_command = 2; //string
	sprintf(in_default, "%s",  buf_path);
	in_max = 80;
	in_string = buf_path;
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Path:",260,175);
	Say("Enter the path with trailing backslash to a dir containing"
	    " another dink.dat and map.dat file to choose a replacement"
	    " for this block. (or enter to choose a replacement from the"
	    " current map)"
	    ,10,10);
      }

    if (in_master == 31)
      {
	in_command = 2; //string
	sprintf(in_default, "%s",  cur_ed_screen.script);
	in_max = 20;
	in_string = cur_ed_screen.script;
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Script:",260,175);
	Say("This script will be run before the screen is drawn.  A good place"
	    "to change the vision, ect."
	    ,10,10);
      }

    if (in_master == INPUT_SCREEN_VISION)
      {
	in_command = 1; //number
	in_int = &map_vision;
	in_max = 10; //max _length
	sprintf(in_default,"%d",map_vision); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Vision:",260,175);
	Say("Current vision.  If not 0, any sprites you add will ONLY show up"
	    " in the game if the vision level matches this one."
	    ,10,10);
      }

    if (in_master == INPUT_SCREEN_MIDI)
      {
	in_command = 1; //number
	in_max = 10; //max _length
	sprintf(in_default,"%d",*in_int); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Music # for screen?:",260,175);
	Say("Will play #.MID for this screen if nothing else is playing."
	    ,10,10);
      }

    if (in_master == INPUT_SCREEN_TYPE)
      {
	in_command = 1; //number
	in_max = 10; //max _length
	sprintf(in_default,"%d",*in_int); //set default
	blit(30,1,IOGFX_backbuffer,250,170);
	Say("Screentype?:",260,175);
	Say("Enter 1 for 'indoors'.  (so it won't show up on the player map)."
	    ,10,10);
      }

    old_command = in_master;

    in_master = 0;
    in_onflag = /*true*/1;
}


//this changes all none 0 blocks in this tile to num
void change_tile(int tile, int num)
{
  int x;
  for (x = 0; x < 50; x++)
    {
      int y;
      for (y = 0; y < 50; y++)
	if (hmap.htile[tile].hm[x][y] != 0)
	  hmap.htile[tile].hm[x][y] = num;
    }
}



void copy_front_to_two( void)
{
  IOGFX_background->blit(IOGFX_backbuffer, NULL, NULL);
}


/* Create a 20x20 view of lpDDSTwo for use in the minimap */
void shrink_screen_to_these_cords(int x1, int y1)
{
/*   RECT crapRec, Rect; */
/*   DDBLTFX ddbltfx; */
/*   ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
/*   ddbltfx.dwSize = sizeof(ddbltfx); */

/*   SetRect(&crapRec, playl, 0, playx, 400); */
/*   SetRect(&Rect, x1, y1, x1+20, y1+20); */

/*   lpDDSBack->Blt(&Rect, lpDDSTwo, */
/* 		 &crapRec, DDBLT_DDFX | DDBLT_WAIT, &ddbltfx); */

  // GFX
  /* Generic scaling - except no transparency */
  {
    SDL_Rect src, dst;
    src.x = playl;
    src.y = 0;
    src.w = playx - playl;
    src.h = 400;
    dst.x = x1;
    dst.y = y1;
    dst.w = 20;
    dst.h = 20;
    IOGFX_backbuffer->blitStretch(IOGFX_background, &src, &dst);
  }
}


/**
 * So-called "Movie2000 sprite movie maker" feature. It will dump a
 * series of DinkC moves that you specify using the mouse
 * (destination) and the numpad (direction). Check
 * http://www.dinknetwork.com/file/movie2000_tutorial
 */
void write_moves(void)
{
  char crap[100+6+2];
  char move[100];
  char fname[100];

  strcpy(fname, sp_script);
  if (strlen(sp_script) <= 2)
    strcpy(fname, "CRAP");


  if (input_getscancodejustpressed(SDL_SCANCODE_KP_8))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 8, %d, 1)\n", spr[1].y);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_4))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 4, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_5))
    {
      EditorSoundPlayEffect(SOUND_JUMP);
      sprintf(crap, "story/%s.c",fname);
      add_text("//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n", crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_2))
    {
      EditorSoundPlayEffect(SOUND_JUMP);
      sprintf(crap, "story/%s.c", fname);
      sprintf(move, "move_stop(&current_sprite, 2, %d, 1)\n", spr[1].y);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_6))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 6, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_7))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 7, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_1))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 1, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_9))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 9, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
  if (input_getscancodejustpressed(SDL_SCANCODE_KP_3))
    {
      EditorSoundPlayEffect( SOUND_JUMP );
      sprintf(crap, "story/%s.c",fname);
      sprintf(move, "move_stop(&current_sprite, 3, %d, 1)\n", spr[1].x);
      add_text(move, crap);
    }
}


/**
 * Draw hardness single tile #'tile', pixel by pixel, in white. Used
 * to draw the currently selected hardness tile.
 */
void draw_hard_tile(int x1, int y1, int tile)
{
  SDL_Rect dst;
  dst.w = 1;
  dst.h = 1;

  int x = 0;
  for (x = 0; x < 50; x++)
    {
      int y = 0;
      for (y = 0; y < 50; y++)
	{
	  if (hmap.htile[tile].hm[x][y] == 1)
	    {
	      //draw it
	      dst.x = x1 + x
		+ 20/* status bar length */;
	      dst.y = y1 + y;
	      /* TODO: not very efficient */
	      IOGFX_backbuffer->fillRect(&dst, 255,255,255);
	    }
	}
    }
}

/**
 * Move out brain==1 here
 * Return:
 * 0 : continue
 * 1 : goto skip_draw
 * 2 : return from ::logic()
 */
int gui_logic(int h) {
	rect box_crap,box_real;
	int holdx;
	int xx;
	/*BOOL*/int cool;
	rect  Rect;

	if ((spr[h].seq == 0) || (g_editorMode == EDITOR_MODE_TILE_HARDNESS)) {
		//if (mode == 7)
		if (g_editorMode == EDITOR_MODE_SPRITE_HARDNESS) {
			//editing a sprite, setting hard box and depth dot.
			spr[1].pseq = 1;
			spr[1].pframe = 1;

			if (sjoy.button[EDITOR_ACTION_ESCAPE]) {
				//they want out
				//mode = 5;
				g_editorMode = EDITOR_MODE_SPRITE_PICKER;
				draw96(0);
				spr[1].x = m5x;
				spr[1].y = m5y;
				spr[1].pseq = 10;
				spr[1].pframe = 5;
				spr[1].speed = 50;
				goto sp_edit_end;

			}

			if (input_getscancodejustpressed(SDL_SCANCODE_TAB)) {

				//they hit tab, lets toggle what mode they are in
				if (sp_mode == 0) sp_mode = 1; else if (sp_mode == 1) sp_mode = 2; else if (sp_mode == 2) sp_mode = 0;


			}
			if (input_getcharjustpressed(SDLK_s)) {

				//they hit tab, lets toggle what mode they are in
				char death[150];
				char filename[10];

				sprintf(death, "SET_SPRITE_INFO %d %d %d %d %d %d %d %d\n",
						sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].xoffset,  k[seq[sp_seq].frame[sp_frame]].yoffset,

						k[seq[sp_seq].frame[sp_frame]].hardbox.left, k[seq[sp_seq].frame[sp_frame]].hardbox.top,
						k[seq[sp_seq].frame[sp_frame]].hardbox.right,k[seq[sp_seq].frame[sp_frame]].hardbox.bottom);

				strcpy(filename, "dink.ini");
				add_text(death,filename);
				EditorSoundPlayEffect( SOUND_JUMP );
			}


			int modif = 1;
			if (SDL_GetModState()&KMOD_SHIFT)
				modif += 9;


			if (sp_mode == 0) {

				//ok, we are editing depth dot

				if (SDL_GetModState()&KMOD_CTRL) {
					if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
						k[seq[sp_seq].frame[sp_frame]].xoffset += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {

						k[seq[sp_seq].frame[sp_frame]].xoffset -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
						k[seq[sp_seq].frame[sp_frame]].yoffset -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
						k[seq[sp_seq].frame[sp_frame]].yoffset += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

				} else {
					if (sjoy.right) {
						k[seq[sp_seq].frame[sp_frame]].xoffset +=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.left) {
						k[seq[sp_seq].frame[sp_frame]].xoffset -=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (sjoy.up) {
						k[seq[sp_seq].frame[sp_frame]].yoffset -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.down) {
						k[seq[sp_seq].frame[sp_frame]].yoffset += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}


				}

			}



			if (sp_mode == 2) {

				//ok, we are top left hardness

				if (SDL_GetModState()&KMOD_CTRL) {
					if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.right += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {

						k[seq[sp_seq].frame[sp_frame]].hardbox.right -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.bottom += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

				} else {
					if (sjoy.right) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.right +=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.left) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.right -=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (sjoy.up) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.down) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.bottom += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}


				}


				if (k[seq[sp_seq].frame[sp_frame]].hardbox.right <= k[seq[sp_seq].frame[sp_frame]].hardbox.left)
					k[seq[sp_seq].frame[sp_frame]].hardbox.left = k[seq[sp_seq].frame[sp_frame]].hardbox.right -1;


				if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom <= k[seq[sp_seq].frame[sp_frame]].hardbox.top)
					k[seq[sp_seq].frame[sp_frame]].hardbox.top = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom -1;


			}


			if (sp_mode == 1) {

				//ok, we are top left hardness

				if (SDL_GetModState()&KMOD_CTRL) {
					if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.left += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {

						k[seq[sp_seq].frame[sp_frame]].hardbox.left -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.top -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.top += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

				} else {
					if (sjoy.right) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.left +=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.left) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.left -=  modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}
					if (sjoy.up) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.top -= modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}

					if (sjoy.down) {
						k[seq[sp_seq].frame[sp_frame]].hardbox.top += modif;
						EditorSoundPlayEffect( SOUND_STOP );
					}


				}


				if (k[seq[sp_seq].frame[sp_frame]].hardbox.left >= k[seq[sp_seq].frame[sp_frame]].hardbox.right)
					k[seq[sp_seq].frame[sp_frame]].hardbox.right = k[seq[sp_seq].frame[sp_frame]].hardbox.left +1;


				if (k[seq[sp_seq].frame[sp_frame]].hardbox.top >= k[seq[sp_seq].frame[sp_frame]].hardbox.bottom)
					k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom +1;




			}




			if (k[seq[sp_seq].frame[sp_frame]].hardbox.top > 200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.top = 200;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.top <-200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.top = -200;

			if (k[seq[sp_seq].frame[sp_frame]].hardbox.left > 316) k[seq[sp_seq].frame[sp_frame]].hardbox.left = 316;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.left < -320) k[seq[sp_seq].frame[sp_frame]].hardbox.left = -320;


			if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom > 200) k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = 200;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.bottom <-200 ) k[seq[sp_seq].frame[sp_frame]].hardbox.bottom = -200;

			if (k[seq[sp_seq].frame[sp_frame]].hardbox.right > 316) k[seq[sp_seq].frame[sp_frame]].hardbox.right = 316;
			if (k[seq[sp_seq].frame[sp_frame]].hardbox.right < -320) k[seq[sp_seq].frame[sp_frame]].hardbox.right = -320;



			return 0;

		}


		if (g_editorMode == EDITOR_MODE_SCREEN_SPRITES) {
			// place sprite
			if ( (input_getcharjustpressed(SDLK_v)) ) {
				in_master = INPUT_SCREEN_VISION; // Set screen vision?
			}

			int modif = 0;
			if (SDL_GetModState()&KMOD_SHIFT)
				modif = 9;

			if (input_getcharjustpressed(SDLK_m)) {
				if (sp_screenmatch)
					sp_screenmatch = /*false*/0;
				else
					sp_screenmatch = /*true*/1;
			}


			if (SDL_GetModState()&KMOD_ALT) {
				//alt is held down 87
				if (input_getcharjustpressed(SDLK_w)) {
					//pressed W
					if (((spr[1].pseq == 10) && (spr[1].pframe == 8))) {
						//a sprite is not chosen
						hold_warp_map = cur_map;
						hold_warp_x = spr[1].x;
						hold_warp_y= spr[1].y;
						EditorSoundPlayEffect(SOUND_JUMP);
					}
					else {
						sp_warp_map = hold_warp_map ;
						sp_warp_x = hold_warp_x;
						sp_warp_y = hold_warp_y;
						EditorSoundPlayEffect(SOUND_JUMP);
					}
				}
			}

			/**
			 * Edit sprite properties
			 */
			if (!((spr[1].pseq == 10) && (spr[1].pframe == 8))) {
				//they are wheeling around a sprite
				if (spr[1].x > 1500) spr[1].x = 1500;
				if (spr[1].y > 1500) spr[1].y = 1500;

				if (spr[1].size > 1500) spr[1].size = 1500;

				/* if (GetKeyboard(VK_OEM_4 /\* 219 *\/)) // '[' for US */
				if (input_getscancodestate(SDL_SCANCODE_PAGEDOWN) || input_getscancodestate(SDL_SCANCODE_LEFTBRACKET))
					spr[1].size -= 1+modif;
				/* if (GetKeyboard(VK_OEM_6 /\* 221 *\/)) // ']' for US */
				if (input_getscancodestate(SDL_SCANCODE_PAGEUP) || input_getscancodestate(SDL_SCANCODE_RIGHTBRACKET))
					spr[1].size += 1+modif;


				if (SDL_GetModState()&KMOD_SHIFT) {
					//shift is being held down
					if (input_getscancodestate(SDL_SCANCODE_1) || input_getscancodestate(SDL_SCANCODE_KP_1) || input_getscancodestate(SDL_SCANCODE_F1))  in_master = 11;
					if (input_getscancodestate(SDL_SCANCODE_2) || input_getscancodestate(SDL_SCANCODE_KP_2) || input_getscancodestate(SDL_SCANCODE_F2))  in_master = 12;
					if (input_getscancodestate(SDL_SCANCODE_3) || input_getscancodestate(SDL_SCANCODE_KP_3) || input_getscancodestate(SDL_SCANCODE_F3))  in_master = 13;
					if (input_getscancodestate(SDL_SCANCODE_4) || input_getscancodestate(SDL_SCANCODE_KP_4) || input_getscancodestate(SDL_SCANCODE_F4))  in_master = 14;
					if (input_getscancodestate(SDL_SCANCODE_5) || input_getscancodestate(SDL_SCANCODE_KP_5) || input_getscancodestate(SDL_SCANCODE_F5))  in_master = 15;

					if (input_getscancodestate(SDL_SCANCODE_6) || input_getscancodestate(SDL_SCANCODE_KP_6) || input_getscancodestate(SDL_SCANCODE_F6))  in_master = 16;
					if (input_getscancodestate(SDL_SCANCODE_7) || input_getscancodestate(SDL_SCANCODE_KP_7) || input_getscancodestate(SDL_SCANCODE_F7))  in_master = 17;
					if (input_getscancodestate(SDL_SCANCODE_8) || input_getscancodestate(SDL_SCANCODE_KP_8) || input_getscancodestate(SDL_SCANCODE_F8))  in_master = 18;
					if (input_getscancodestate(SDL_SCANCODE_9) || input_getscancodestate(SDL_SCANCODE_KP_9) || input_getscancodestate(SDL_SCANCODE_F9))  in_master = 19;



				}
				else if (SDL_GetModState()&KMOD_ALT) {
					//alt is being held down
					if (input_getscancodestate(SDL_SCANCODE_1) || input_getscancodestate(SDL_SCANCODE_KP_1) || input_getscancodestate(SDL_SCANCODE_F1))  in_master = 20;
					if (input_getscancodestate(SDL_SCANCODE_2) || input_getscancodestate(SDL_SCANCODE_KP_2) || input_getscancodestate(SDL_SCANCODE_F2))  in_master = 21;
					if (input_getscancodestate(SDL_SCANCODE_3) || input_getscancodestate(SDL_SCANCODE_KP_3) || input_getscancodestate(SDL_SCANCODE_F3))  in_master = 22;
					/*(getkeystate('4' /\* 52 *\/))  in_master = 14;
					  if (getkeystate(53))  in_master = 15;

					  if (getkeystate(54))  in_master = 16;
					  if (getkeystate(55))  in_master = 17;
					  if (getkeystate(56))  in_master = 18;
					  if (getkeystate(57))  in_master = 19;

					*/
				}
				else {
					//shift is not being held down
					if (input_getscancodestate(SDL_SCANCODE_1) || input_getscancodestate(SDL_SCANCODE_KP_1) || input_getscancodestate(SDL_SCANCODE_F1)) in_master = 1;
					if (input_getscancodestate(SDL_SCANCODE_2) || input_getscancodestate(SDL_SCANCODE_KP_2) || input_getscancodestate(SDL_SCANCODE_F2)) in_master = 2;
					if (input_getscancodestate(SDL_SCANCODE_3) || input_getscancodestate(SDL_SCANCODE_KP_3) || input_getscancodestate(SDL_SCANCODE_F3)) in_master = 3;
					if (input_getscancodestate(SDL_SCANCODE_4) || input_getscancodestate(SDL_SCANCODE_KP_4) || input_getscancodestate(SDL_SCANCODE_F4)) in_master = 4;
					if (input_getscancodestate(SDL_SCANCODE_5) || input_getscancodestate(SDL_SCANCODE_KP_5) || input_getscancodestate(SDL_SCANCODE_F5)) in_master = 5;
					if (input_getscancodestate(SDL_SCANCODE_6) || input_getscancodestate(SDL_SCANCODE_KP_6) || input_getscancodestate(SDL_SCANCODE_F6)) in_master = 6;
					if (input_getscancodestate(SDL_SCANCODE_7) || input_getscancodestate(SDL_SCANCODE_KP_7) || input_getscancodestate(SDL_SCANCODE_F7)) in_master = 7;
					if (input_getscancodestate(SDL_SCANCODE_8) || input_getscancodestate(SDL_SCANCODE_KP_8) || input_getscancodestate(SDL_SCANCODE_F8)) in_master = 8;
					if (input_getscancodestate(SDL_SCANCODE_9) || input_getscancodestate(SDL_SCANCODE_KP_9) || input_getscancodestate(SDL_SCANCODE_F9)) in_master = 9;
					if (input_getscancodestate(SDL_SCANCODE_0) || input_getscancodestate(SDL_SCANCODE_KP_0) || input_getscancodestate(SDL_SCANCODE_F10)) in_master = 10;
				}


				if (input_getcharjustpressed(SDLK_s)) {
					smart_add();

					draw_screen_editor();
				}

				if ( (sjoy.button[EDITOR_ACTION_RETURN]) | (mouse1) ) {
					smart_add();
					draw_screen_editor();
					spr[1].pseq = 10;
					spr[1].pframe = 8;
					spr[1].size = 100;
					rect_set(&spr[1].alt,0,0,0,0);

				}

				if (input_getscancodejustpressed(SDL_SCANCODE_DELETE)) {

					spr[1].pseq = 10;
					spr[1].pframe = 8;
					spr[1].size = 100;
					rect_set(&spr[1].alt,0,0,0,0);

				}

			} else {
				//no sprite is currently selected

				int max_spr = 0;
				int jj;

				write_moves();



				for (jj=1; jj < 100; jj++) {
					if ( cur_ed_screen.sprite[jj].active) if (cur_ed_screen.sprite[jj].vision == map_vision) max_spr++;
				}


				if (max_spr > 0) {

					/* if (sjoy.keyjustpressed[VK_OEM_4 /\* 219 *\/]) // '[' for US */
					if (input_getscancodejustpressed(SDL_SCANCODE_PAGEUP) || input_getscancodejustpressed(SDL_SCANCODE_LEFTBRACKET)) {
						sp_cycle--;
						if (sp_cycle < 1)
							sp_cycle = max_spr;
					}

					if (input_getscancodejustpressed(SDL_SCANCODE_PAGEDOWN) || input_getscancodejustpressed(SDL_SCANCODE_RIGHTBRACKET)) {
						sp_cycle++;
						if (sp_cycle > max_spr)
							sp_cycle = 1;
					}
				}



				//Msg("Cycle is %d", sp_cycle);
				int realpic = 0;

				if (sp_cycle > 0) {
					//lets draw a frame around the sprite we want
					int dumbpic = 0;
					int jh;
					realpic = 0;
					for (jh = 1; dumbpic != sp_cycle; jh++) {
						if (cur_ed_screen.sprite[jh].active)  if ( cur_ed_screen.sprite[jh].vision == map_vision) {
								dumbpic++;
								realpic = jh;
							}
						if (jh == 99) goto fail;

					}

					last_sprite_added = realpic;


					/* ddbltfx.dwSize = sizeof(ddbltfx); */
					/* ddbltfx.dwFillColor = 235; */

					int	sprite = add_sprite_dumb(cur_ed_screen.sprite[realpic].x,cur_ed_screen.sprite[realpic].y,0,
												 cur_ed_screen.sprite[realpic].seq, cur_ed_screen.sprite[realpic].frame,
												 cur_ed_screen.sprite[realpic].size);
					rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[realpic].alt);
					get_box(sprite, &box_crap, &box_real, skip_screen_clipping());




					get_box(sprite, &box_crap, &box_real, skip_screen_clipping());
					box_crap.bottom = box_crap.top + 5;
					/* ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
					// GFX
					{
						SDL_Rect dst;
						dst.x = box_crap.left;
						dst.y = box_crap.top;
						dst.w = box_crap.right - box_crap.left;
						dst.h = 5;
						IOGFX_backbuffer->fillRect(&dst, 33,41,16);
					}

					get_box(sprite, &box_crap, &box_real, skip_screen_clipping());
					box_crap.right = box_crap.left + 5;
					/* ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
					// GFX
					{
						SDL_Rect dst;
						dst.x = box_crap.left;
						dst.y = box_crap.top;
						dst.w = 5;
						dst.h = box_crap.bottom - box_crap.top;
						IOGFX_backbuffer->fillRect(&dst, 33,41,16);
					}

					get_box(sprite, &box_crap, &box_real, skip_screen_clipping());
					box_crap.left = box_crap.right - 5;
					/* ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
					// GFX
					{
						SDL_Rect dst;
						dst.x = box_crap.right - 5;
						dst.y = box_crap.top;
						dst.w = 5;
						dst.h = box_crap.bottom - box_crap.top;
						IOGFX_backbuffer->fillRect(&dst, 33,41,16);
					}

					get_box(sprite, &box_crap, &box_real, skip_screen_clipping());
					box_crap.top = box_crap.bottom - 5;
					/* ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
					// GFX
					{
						SDL_Rect dst;
						dst.x = box_crap.left;
						dst.y = box_crap.bottom - 5;
						dst.w = box_crap.right - box_crap.left;
						dst.h = 5;
						IOGFX_backbuffer->fillRect(&dst, 33,41,16);
					}

					// if (ddrval != DD_OK) dderror(ddrval);

					lsm_remove_sprite(sprite);

				}

			fail:
				if ( (sjoy.button[EDITOR_ACTION_RETURN]) | (mouse1)) {
					//pick up a sprite already placed by hitting enter
					int uu;

					for (uu = 100; uu > 0; uu--) {
						if ( cur_ed_screen.sprite[uu].active) if ( ( cur_ed_screen.sprite[uu].vision == 0) || (cur_ed_screen.sprite[uu].vision == map_vision)) {

								int	sprite = add_sprite_dumb(cur_ed_screen.sprite[uu].x,cur_ed_screen.sprite[uu].y,0,
															 cur_ed_screen.sprite[uu].seq, cur_ed_screen.sprite[uu].frame,
															 cur_ed_screen.sprite[uu].size);
								rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[uu].alt);
								get_box(sprite, &box_crap, &box_real, skip_screen_clipping());
								if (realpic > 0) goto spwarp;
								//Msg("Got sprite %d's info. X%d Y %d.",uu,box_crap.left,box_crap.right);

								if (inside_box(spr[1].x,spr[1].y,box_crap)) {
									//this is the sprite they want to edit, lets convert them into it
									//						Msg("FOUND SPRITE!  It's %d, huh.",uu);

									if (false) {
									spwarp:
										log_debug("Ah yeah, using %d!", realpic);
										uu = realpic;
									}

									spr[1].x = cur_ed_screen.sprite[uu].x;
									spr[1].y = cur_ed_screen.sprite[uu].y;
									spr[1].size = cur_ed_screen.sprite[uu].size;
									sp_type = cur_ed_screen.sprite[uu].type;
									sp_brain = cur_ed_screen.sprite[uu].brain;
									sp_speed = cur_ed_screen.sprite[uu].speed;
									sp_base_walk = cur_ed_screen.sprite[uu].base_walk;
									sp_base_idle = cur_ed_screen.sprite[uu].base_idle;
									sp_base_attack = cur_ed_screen.sprite[uu].base_attack;
									sp_base_hit = cur_ed_screen.sprite[uu].base_hit;
									sp_timer = cur_ed_screen.sprite[uu].timing;
									sp_que = cur_ed_screen.sprite[uu].que;
									sp_seq = cur_ed_screen.sprite[uu].seq;
									sp_hard = cur_ed_screen.sprite[uu].hard;
									rect_copy(&spr[1].alt , &cur_ed_screen.sprite[uu].alt);
									sp_frame = cur_ed_screen.sprite[uu].frame;
									spr[1].pseq = cur_ed_screen.sprite[uu].seq;
									spr[1].pframe = cur_ed_screen.sprite[uu].frame;

									sp_is_warp = cur_ed_screen.sprite[uu].is_warp;

									sp_warp_map = cur_ed_screen.sprite[uu].warp_map;
									sp_warp_x = cur_ed_screen.sprite[uu].warp_x;
									sp_warp_y = cur_ed_screen.sprite[uu].warp_y;
									sp_parm_seq = cur_ed_screen.sprite[uu].parm_seq;
									strcpy(sp_script, cur_ed_screen.sprite[uu].script);

									sp_base_die = cur_ed_screen.sprite[uu].base_die;
									sp_gold = cur_ed_screen.sprite[uu].gold;
									sp_hitpoints = cur_ed_screen.sprite[uu].hitpoints;

									sp_exp = cur_ed_screen.sprite[uu].exp;
									sp_nohit = cur_ed_screen.sprite[uu].nohit;
									sp_touch_damage = cur_ed_screen.sprite[uu].touch_damage;
									sp_defense = cur_ed_screen.sprite[uu].defense;
									sp_strength = cur_ed_screen.sprite[uu].strength;
									sp_sound = cur_ed_screen.sprite[uu].sound;

									cur_ed_screen.sprite[uu].active = /*false*/0; //erase sprite
									draw_screen_editor();
									lsm_remove_sprite(sprite);
									break;
								}
								lsm_remove_sprite(sprite);

							}
					}


				}


				if ((SDL_GetModState()&KMOD_ALT) && (input_getscancodestate(SDL_SCANCODE_DELETE))) {
					int ll;
					for (ll = 1; ll < 100; ll++) {
						cur_ed_screen.sprite[ll].active = /*false*/0;
					}
					draw_screen_editor();
					rect_set(&spr[h].alt,0,0,0,0);
				}
			}

			/** Trim a sprite **/
			if (input_getcharstate(SDLK_z) || (input_getcharstate(SDLK_x))) {
				if ((spr[h].alt.right == 0) && (spr[h].alt.left == 0)
					&& (spr[h].alt.top == 0) && (spr[h].alt.bottom == 0))
					rect_copy(&spr[h].alt, &k[getpic(h)].box);
			}

			if (input_getcharstate(SDLK_z)) {

				if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
					spr[h].alt.left += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}

				if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {
					spr[h].alt.left -= spr[h].speed +modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
					spr[h].alt.top += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}

				if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
					spr[h].alt.top -= spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}


				if (spr[h].alt.top < 0) spr[h].alt.top = 0;
				if (spr[h].alt.left < 0) spr[h].alt.left = 0;
				return 0;
			}



			if (input_getcharstate(SDLK_x)) {
				if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
					spr[h].alt.right += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}

				if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {
					spr[h].alt.right -= spr[h].speed +modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
					spr[h].alt.bottom += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
					spr[h].alt.bottom -= spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );

					//	Msg("Bottom is %d..",spr[h].alt.bottom);

				}
				if (spr[h].alt.bottom > k[getpic(h)].box.bottom) spr[h].alt.bottom = k[getpic(h)].box.bottom;
				if (spr[h].alt.right > k[getpic(h)].box.right) spr[h].alt.right = k[getpic(h)].box.right;

				return 0;

			}


			if (spr[1].size < 1)
				spr[1].size = 1;

			/* Precise positionning: move the sprite just
			   1 pixel left/right/up/down, then don't do
			   move until an arrow key is released and
			   pressed again. */
			if (SDL_GetModState()&KMOD_CTRL) {
				if (input_getscancodejustpressed(SDL_SCANCODE_RIGHT)) {
				
					sp_cycle = 0;
				
					spr[h].x += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				
				if (input_getscancodejustpressed(SDL_SCANCODE_LEFT)) {
					spr[h].x -= spr[h].speed +modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}
				if (input_getscancodejustpressed(SDL_SCANCODE_UP)) {
					spr[h].y -= spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}
				
				if (input_getscancodejustpressed(SDL_SCANCODE_DOWN)) {
					spr[h].y += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}
				
			} else {
				if (sjoy.right) {
					spr[h].x += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}

				if (sjoy.left) {
					spr[h].x -= spr[h].speed +modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}
				if (sjoy.up) {
					spr[h].y -= spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;
				}

				if (sjoy.down) {
					spr[h].y += spr[h].speed + modif;
					EditorSoundPlayEffect( SOUND_STOP );
					sp_cycle = 0;

				}


			}





			if (  (sjoy.button[EDITOR_ACTION_ESCAPE]) ) {
				//return to edit mode or drop sprite, depending..
				if (((spr[1].pseq == 10) && (spr[1].pframe == 8))  ) {

					rect_set(&spr[1].alt,0,0,0,0);

					spr[1].size = 100;
					g_editorMode = EDITOR_MODE_SCREEN_TILES;
					spr[1].x = m4x;
					spr[1].y = m4y;
					spr[1].seq = 3;
					spr[1].speed = 50;
				} else {
					smart_add();
					draw_screen_editor();
					rect_set(&spr[1].alt,0,0,0,0);

					spr[1].pseq = 10;
					spr[1].pframe = 8;
					spr[1].size = 100;

				}
			}


			if (input_getcharjustpressed(SDLK_e)) {
				//they hit E, go to sprite picker
				rect_set(&spr[1].alt,0,0,0,0);

				spr[1].size = 100;
				//mode = 5;
				g_editorMode = EDITOR_MODE_SPRITE_PICKER;
				m6x = spr[h].x;
				m6y = spr[h].y;
				spr[h].x = m5x;
				spr[h].y = m5y;

				spr[1].seq = 3;
				spr[1].speed = 50;
				if (sp_seq == 0) draw15(sp_picker); else draw96(sp_frame);
				goto sp_edit_end;

			}
			if (sjoy.button[EDITOR_ACTION_TAB]) {
				//they hit tab, return to tile edit mode
				if (  !((spr[1].pseq == 10) && (spr[1].pframe == 8))  ) {
					smart_add();
					rect_set(&spr[1].alt,0,0,0,0);

					draw_screen_editor();
				}
				spr[1].size = 100;
				g_editorMode = EDITOR_MODE_SCREEN_TILES;
				spr[h].x = m4x;
				spr[h].y = m4y;

				spr[1].seq = 3;
				spr[1].speed = 50;
				//	if (sp_seq == 0) draw15(); else draw96();
				goto sp_edit_end;

			}


			return 0;

		}


		if ( (g_editorMode == EDITOR_MODE_SCREEN_TILES)
			 && (sjoy.button[EDITOR_ACTION_TAB])) {

			//they chose sprite picker mode
			//lsm_kill_all_nonlive_sprites();


			g_editorMode = EDITOR_MODE_SCREEN_SPRITES;

			spr[1].pseq = 10;
			spr[1].pframe = 8;

			spr[1].speed = 1;
			selx = 1;
			sely = 1;
			m4x = spr[h].x;
			m4y = spr[h].y;
			//spr[h].x = m5x;
			//spr[h].y = m5y;

			//if (sp_seq == 0)
			//	draw15(); else draw96();

		} else if (g_editorMode == EDITOR_MODE_SPRITE_PICKER) {
			//picking a sprite
			if (sp_seq != 0) {
				//they are in select sprite phase 2

				if (input_getcharjustpressed(SDLK_e)) {
					//they want to 'edit' the sprite
					g_editorMode = EDITOR_MODE_SPRITE_HARDNESS;
					m5x = spr[h].x;
					m5y = spr[h].y;

					//lets blank the screen
					/* ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
					/* ddbltfx.dwSize = sizeof( ddbltfx); */
					/* ddbltfx.dwFillColor = 255; */
					/* crap = lpDDSTwo->Blt(NULL ,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx); */
					// GFX
					IOGFX_background->fillRect(NULL, 255,255,255);

					holdx = (spr[1].x / 50);
					int holdy = (spr[1].y / 50)+1;
					holdx = holdx * 8;
					if (seq[sp_seq].frame[holdx + holdy] == 0) goto sp_fin;

					add_sprite_dumb(320,200 , 0,sp_seq,holdx + holdy,100 );

					sp_frame = holdx + holdy;
					spr[1].pseq = 10;
					spr[1].pframe = 8;


					spr[1].speed = 1;
					goto sp_edit_end;

				}

				if ((sjoy.button[EDITOR_ACTION_ESCAPE])) {


					//returning to main sprite picker mode
					sp_seq = 0;

					draw15(sp_picker);
					spr[h].x = m5ax;
					spr[h].y = m5ay;

					goto sp_edit_end;
				}

				if (sjoy.button[EDITOR_ACTION_TAB]) {
					//leave to screen editor
				sp_fin:
					m5x = spr[h].x;
					m5y = spr[h].y;

					draw_screen_editor();
					spr[h].x = m6x;
					spr[h].y = m6y;

					spr[1].pseq = 10;
					spr[1].pframe = 8;

					spr[h].speed = 1;
					g_editorMode = EDITOR_MODE_SCREEN_SPRITES;
					goto sp_edit_end;

				}

				if (sjoy.button[EDITOR_ACTION_RETURN]) {

					// go to mode 6, sprite placement
					m5x = spr[h].x;
					m5y = spr[h].y;


					holdx = (spr[1].x / 50);
					int holdy = (spr[1].y / 50)+1;
					holdx = holdx * 8;
					if (seq[sp_seq].frame[holdx + holdy] == 0) goto sp_fin;
					spr[1].pseq = sp_seq;
					spr[1].pframe = holdx + holdy;
					sp_frame = holdx + holdy;
					draw_screen_editor();
					spr[h].x = m6x;
					spr[h].y = m6y;
					g_editorMode = EDITOR_MODE_SCREEN_SPRITES;
					spr[h].speed = 1;
					goto sp_edit_end;

				}


				goto sp_edit_end;
			}


			if (sjoy.button[EDITOR_ACTION_TAB] || sjoy.button[EDITOR_ACTION_ESCAPE]) {

				//exit to main editor
				/*m5x = spr[h].x;
				  m5y = spr[h].y;
				  draw_screen_editor();
				  spr[h].x = m4x;
				  spr[h].y = m4y;
				  mode = 3;
				  return 0;
				*/
				m5x = spr[h].x;
				m5y = spr[h].y;

				draw_screen_editor();
				spr[h].x = m6x;
				spr[h].y = m6y;
				spr[h].pseq = 10;
				spr[h].pframe = 8;

				spr[h].speed = 1;
				g_editorMode = EDITOR_MODE_SCREEN_SPRITES;
				return 0;
				//goto sp_edit_end;

			}

			/* if (sjoy.keyjustpressed[VK_OEM_4 /\* 219 *\/]) // '[' for US */
			if (input_getscancodejustpressed(SDL_SCANCODE_PAGEUP) || input_getscancodejustpressed(SDL_SCANCODE_LEFTBRACKET)) {
				if (sp_picker > 95) sp_picker -= 96; else {
					sp_picker = (4 * 96);
				}
				draw15(sp_picker);
			}
			/* if (sjoy.keyjustpressed[VK_OEM_6 /\* 221 *\/]) // ']' for US */
			if (input_getscancodejustpressed(SDL_SCANCODE_PAGEDOWN) || input_getscancodejustpressed(SDL_SCANCODE_RIGHTBRACKET)) {
				if (sp_picker < 400) sp_picker += 96;
				draw15(sp_picker);
			}

			if (sjoy.button[EDITOR_ACTION_RETURN]) {

				//they chose a catagory, switch to phase 2, it will know cuz sp_seq > 0.
				holdx = (spr[1].x / 50);
				int holdy = (spr[1].y / 50)+1;
				holdx = holdx * 8;
				m5ax = spr[1].x;
				m5ay = spr[1].y;
				spr[1].x = 0;
				spr[1].y = 0;
				sp_seq = sp_get(sp_picker+ (holdx + holdy));
				//	Msg("Sp_seq is %d",sp_seq);
				draw96(0);

			}


		}
	sp_edit_end:




		if (g_editorMode == EDITOR_MODE_SCREEN_TILES) draw_current();


		if (g_editorMode == EDITOR_MODE_INIT) {

			spr[h].seq = 2;
			spr[h].seq_orig = 2;
			draw_minimap();
			spr[1].que = 20000;
			g_editorMode = EDITOR_MODE_MINIMAP;
			spr[2].active = false;
			spr[3].active = false;
			spr[4].active = false;
		}

		if (g_editorMode == EDITOR_MODE_INIT) return 0;




		//mode equals 4, they are in hardness edit mode, so lets do this thang

		if (g_editorMode == EDITOR_MODE_TILE_HARDNESS) {
			if (spr[h].seq == 0) {
				if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_RIGHT))) {
					spr[h].seq = 4;
					spr[h].frame = 1;
					if (selx < 8) selx++;
					goto b1fun;
				}

				if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_LEFT))) {
					spr[h].seq = 4;
					spr[h].frame = 1;
					if (selx > 1) selx--;
					goto b1fun;
				}

				if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_UP))) {
					spr[h].seq = 4;
					spr[h].frame = 1;
					if (sely > 1) sely--;
					goto b1fun;
				}

				if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_DOWN))) {
					spr[h].seq = 4;
					spr[h].frame = 1;
					if (sely <  8) sely++;
					goto b1fun;
				}


				if (sjoy.right) {
					spr[h].x += 9;
					spr[h].seq = 4;
					spr[h].frame = 1;
					EditorSoundPlayEffect(SOUND_STOP);
				}
				if (sjoy.left) {
					spr[h].x -= 9;
					spr[h].seq = 4;
					spr[h].frame = 1;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				if (sjoy.up) {
					spr[h].y -= 9;
					spr[h].seq = 4;
					spr[h].frame = 1;
					EditorSoundPlayEffect( SOUND_STOP );
				}
				if (sjoy.down) {
					spr[h].y += 9;
					spr[h].seq = 4;
					spr[h].frame = 1;
					EditorSoundPlayEffect( SOUND_STOP );
				}
			}


		b1fun:

			//make sure they didn't go past the boundrys
			if (g_editorMode != 1) {

				if (spr[h].x + (9 * (selx -1))> 95+441) spr[h].x = (95+441) - (9 * (selx-1));
				if (spr[h].x < 95) spr[h].x = 95;
				if (spr[h].y < 0) spr[h].y = 0;
				if (spr[h].y + (9 * (sely -1))> 441) spr[h].y = 441 - (9 * (sely-1));
			}

			//change a piece to hard
			if (input_getcharstate(SDLK_z)) {
				int y;
				for (y = 0; y < sely; y++) {
					int x;
					for (x = 0; x < selx; x++) {
						hmap.htile[hard_tile].hm[((spr[h].x) + (x*9) - 95) / 9][(spr[h].y + (y *9)) / 9] = 1;

					}
				}
			}


			//change a piece to soft
			if (input_getcharstate(SDLK_x)) {
				int y;
				for (y = 0; y < sely; y++) {
					int x;
					for (x = 0; x < selx; x++) {
						hmap.htile[hard_tile].hm[((spr[h].x) + (x*9) - 95) / 9][(spr[h].y + (y *9)) / 9] = 0;

					}
				}
			}


			if ( (input_getcharstate(SDLK_a)) && (SDL_GetModState()&KMOD_ALT ) ) {
				//change ALL to 'low hard'
				change_tile(hard_tile, 2);
				log_debug("Changing whole tile to 2");

				return 2;
			}

			if ( (input_getcharstate(SDLK_s)) && (SDL_GetModState()&KMOD_ALT ) ) {
				//change ALL to 'low hard'
				change_tile(hard_tile, 3);
				log_debug("Changing whole tile to 3");

				return 2;
			}
			if ( (input_getcharstate(SDLK_x)) && (SDL_GetModState()&KMOD_ALT ) ) {
				//change ALL to 'low hard'
				change_tile(hard_tile, 1);
				log_debug("Changing whole tile to 1");

				return 2;
			}


			if (input_getcharstate(SDLK_a)) {
				int y;
				for (y = 0; y < sely; y++) {
					int x;
					for (x = 0; x < selx; x++) {
						hmap.htile[hard_tile].hm[((spr[h].x) + (x*9) - 95) / 9][(spr[h].y + (y *9)) / 9] = 2;

					}
				}

			}
			if (input_getcharstate(SDLK_s)) {
				int y;
				for (y = 0; y < sely; y++) {
					int x;
					for (x = 0; x < selx; x++) {
						hmap.htile[hard_tile].hm[((spr[h].x) + (x*9) - 95) / 9][(spr[h].y + (y *9)) / 9] = 3;

					}
				}

			}


			//update frame with current hard blocks, slow

			draw_hard();

			if (sjoy.button[EDITOR_ACTION_ESCAPE] == 1
				|| sjoy.button[EDITOR_ACTION_RETURN] == 1) {
				//quit hardness edit

				spr[h].seq = 3;
				spr[h].seq_orig = 3;

				if (last_modereal == 8) {
					//return to alt hardness editor
					draw_screen_editor();
					last_modereal = 0;
					spr[h].x = m4x;
					spr[h].y = m4y;

					selx = 1;
					sely = 1;

					g_editorMode = EDITOR_MODE_SCREEN_HARDNESS_INIT;
					return 2;
				}

				if (last_mode > 0) {
					loadtile(last_mode);
					selx = 1;
					sely = 1;
					return 0;
				}
				fill_whole_hard();

				draw_screen_editor();
				spr[h].x = m4x;
				spr[h].y = m4y;
				g_editorMode = EDITOR_MODE_SCREEN_TILES;
				selx = 1;
				sely = 1;
			}

			return 0;
		}


		//THEY WANT TO EDIT HARDNESS

		if ( (input_getcharjustpressed(SDLK_b)) ) {
			in_master = 31;


		}

		if ( (input_getcharjustpressed(SDLK_v)) ) {
			in_master = INPUT_SCREEN_VISION;
		}



		if (((g_editorMode == EDITOR_MODE_SCREEN_TILES) && (sjoy.button[EDITOR_ACTION_RETURN]))
			|| ((g_editorMode == EDITOR_MODE_TILE_PICKER) && (input_getscancodestate(SDL_SCANCODE_SPACE)))) {

			if (g_editorMode == EDITOR_MODE_SCREEN_TILES)
				cur_tile = cur_ed_screen.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0;

			if (g_editorMode == EDITOR_MODE_TILE_PICKER) {
				cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
				cur_tile += (cur_tileset * 128) - 128;
			}

			lsm_kill_all_nonlive_sprites();
			draw_current();

			if (cur_tile > 0) {
				if (hmap.btile_default[cur_tile] == 0) {
					int j;
					for (j = 1; j < 799; j++) {
						if (hmap.htile[j].used == /*FALSE*/0) {
							hmap.btile_default[cur_tile] = j;
							hmap.htile[j].used = /*TRUE*/1;
							hard_tile = j;
							goto tilesel;
						}
					}
				}
				else
					hard_tile = hmap.btile_default[cur_tile];

			tilesel:
				xx = cur_tile % 128;
				/* Rect.left = (xx * 50- (xx / 12) * 600); */
				/* Rect.top = (xx / 12) * 50; */
				/* Rect.right = Rect.left + 50; */
				/* Rect.bottom = Rect.top + 50; */

				/* crapRec.top = 0; */
				/* crapRec.left = 95; */
				/* crapRec.bottom = 450; */
				/* crapRec.right = 95+450; */

				/* ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
				/* ddbltfx.dwSize = sizeof( ddbltfx); */

				spr[1].seq = 0;
				spr[1].pseq = 10;
				spr[1].pframe = 1;



				// Display the given tile square fullscreen, for hardness editing
				/* lpDDSTwo->Blt(&crapRec , tiles[cool+1], */
				/*               &Rect, DDBLT_DDFX | DDBLT_WAIT,&ddbltfx ); */
				// GFX
				/* Generic scaling */
				/* Not perfectly accurate yet: move a 200% sprite to the
				   border of the screen to it is clipped: it's scaled size
				   will slighly vary. Maybe we need to clip the source zone
				   before scaling it.. */
				{
					SDL_Rect src, dst;
					src.x = (xx % 12) * 50;
					src.y = (xx / 12) * 50;
					src.w = 50;
					src.h = 50;
					dst.x = 95;
					dst.y = 0;
					dst.w = 450;
					dst.h = 450;
				  
					cool = cur_tile / 128;
					IOGFX_background->blitStretch(g_dmod.bgTilesets.slots[cool+1], &src, &dst);
				}

				m4x = spr[h].x;
				m4y = spr[h].y;

				spr[1].x = 95;
				spr[1].y = 0;
				selx = 1;
				sely = 1;

				g_editorMode = EDITOR_MODE_TILE_HARDNESS;
			}
		}


		if ((g_editorMode == EDITOR_MODE_TILE_PICKER) || (g_editorMode == EDITOR_MODE_SCREEN_TILES)) {
			//resizing the box

			if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_RIGHT)) ) {
				spr[h].seq = 3;
				spr[h].seq_orig = 3;
				if (selx < 8) selx++;
				return 0;
			}

			if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_LEFT)) ) {
				spr[h].seq = 3;
				spr[h].seq_orig = 3;
				if (selx > 1) selx--;
				return 0;

			}

			if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_UP)) ) {
				spr[h].seq = 3;
				spr[h].seq_orig = 3;
				if (sely > 1) sely--;
				return 0;
			}

			if ((SDL_GetModState()&KMOD_SHIFT) && (input_getscancodestate(SDL_SCANCODE_DOWN)) ) {
				spr[h].seq = 3;
				spr[h].seq_orig = 3;
				if (sely <  8) sely++;
				return 0;
			}

		}


		if (input_getscancodestate(SDL_SCANCODE_RIGHT)) {
			spr[h].x += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect( SOUND_STOP );
		}


		if ((input_getcharstate(SDLK_s)) && (g_editorMode == EDITOR_MODE_SCREEN_TILES)) {
			int y;
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//EditorSoundPlayEffect( SOUND_JUMP );


			cur_ed_screen.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0 = cur_tile;

			for (y = 0; y < sely; y++) {
				int x;
				for (x = 0; x < selx; x++) {
					holdx = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
					holdx += (y * 12);
					holdx += x;
					cur_ed_screen.t[holdx].square_full_idx0 = (cur_tile + (y * 12) + x);

				}
			}

			draw_screen_editor();
		}



		if ((input_getcharstate(SDLK_c)) && (g_editorMode == EDITOR_MODE_SCREEN_TILES)) {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//SoundPlayEffect( SOUND_JUMP );
			cur_tile = cur_ed_screen.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].square_full_idx0;
			draw_screen_editor();
		}

		/* Tile selection */
		if (g_editorMode == EDITOR_MODE_SCREEN_TILES || g_editorMode == EDITOR_MODE_TILE_PICKER) {
			int unit = 0, tile_no = 0;
			if (input_getscancodestate(SDL_SCANCODE_1) || input_getscancodestate(SDL_SCANCODE_KP_1) || input_getscancodestate(SDL_SCANCODE_F1)) unit = 1;
			if (input_getscancodestate(SDL_SCANCODE_2) || input_getscancodestate(SDL_SCANCODE_KP_2) || input_getscancodestate(SDL_SCANCODE_F2)) unit = 2;
			if (input_getscancodestate(SDL_SCANCODE_3) || input_getscancodestate(SDL_SCANCODE_KP_3) || input_getscancodestate(SDL_SCANCODE_F3)) unit = 3;
			if (input_getscancodestate(SDL_SCANCODE_4) || input_getscancodestate(SDL_SCANCODE_KP_4) || input_getscancodestate(SDL_SCANCODE_F4)) unit = 4;
			if (input_getscancodestate(SDL_SCANCODE_5) || input_getscancodestate(SDL_SCANCODE_KP_5) || input_getscancodestate(SDL_SCANCODE_F5)) unit = 5;
			if (input_getscancodestate(SDL_SCANCODE_6) || input_getscancodestate(SDL_SCANCODE_KP_6) || input_getscancodestate(SDL_SCANCODE_F6)) unit = 6;
			if (input_getscancodestate(SDL_SCANCODE_7) || input_getscancodestate(SDL_SCANCODE_KP_7) || input_getscancodestate(SDL_SCANCODE_F7)) unit = 7;
			if (input_getscancodestate(SDL_SCANCODE_8) || input_getscancodestate(SDL_SCANCODE_KP_8) || input_getscancodestate(SDL_SCANCODE_F8)) unit = 8;
			if (input_getscancodestate(SDL_SCANCODE_9) || input_getscancodestate(SDL_SCANCODE_KP_9) || input_getscancodestate(SDL_SCANCODE_F9)) unit = 9;
			if (input_getscancodestate(SDL_SCANCODE_0) || input_getscancodestate(SDL_SCANCODE_KP_0) || input_getscancodestate(SDL_SCANCODE_F10)) unit = 10;

			tile_no = unit;
			if (SDL_GetModState()&KMOD_SHIFT)
				tile_no = 10 + unit;
			if (SDL_GetModState()&KMOD_CTRL)
				tile_no = 20 + unit;
			if (SDL_GetModState()&KMOD_ALT)
				tile_no = 30 + unit;

			if (unit > 0) /* make sure one key was pressed */
				loadtile(tile_no);

			/* Exception: tile #41 = Alt+[top-left key below escape] */
			if ((SDL_GetModState()&KMOD_ALT)
				&& input_getscancodestate(SDL_SCANCODE_GRAVE))
				loadtile(41);
			/* alternatives */
			if (input_getscancodestate(SDL_SCANCODE_F11) || input_getscancodestate(SDL_SCANCODE_KP_PERIOD))
				loadtile(41);
		}

		//if ( (GetKeyboard(48)) && ( (mode == 3) | (mode ==2)) ) loadtile(11);


		if ((sjoy.button[EDITOR_ACTION_RETURN]) && (g_editorMode == EDITOR_MODE_TILE_PICKER)) {
			// cut to map editer from tile selection
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
			cur_tile += (cur_tileset * 128) - 128;
			//SoundPlayEffect( SOUND_JUMP );
			m2x = spr[h].x;
			m2y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;

			g_editorMode = EDITOR_MODE_SCREEN_TILES;
			spr[h].speed = 50;
			draw_screen_editor();
			last_mode = 0;
		}



		if (sjoy.button[EDITOR_ACTION_ESCAPE] && (g_editorMode == EDITOR_MODE_TILE_PICKER)) {
			// cut to map editer from tile selection
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			//cur_tile = (((spr[1].y+1)*12) / 50)+(spr[1].x / 50);
			//SoundPlayEffect( SOUND_JUMP );
			m2x = spr[h].x;
			m2y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;

			g_editorMode = EDITOR_MODE_SCREEN_TILES;
			draw_screen_editor();
			last_mode = 0;
			return 0;
		}


		if ( (input_getscancodejustpressed(SDL_SCANCODE_SPACE))  && (g_editorMode == EDITOR_MODE_MINIMAP)) {
			//make_map_tiny();
			draw_screen_tiny = 0;

		}

		if ( (input_getcharjustpressed(SDLK_l))  && (g_editorMode == EDITOR_MODE_MINIMAP)) {

			//if (g_dmod.map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] != 0)
			//{
			buf_map = (((spr[1].y+1)*32) / 20)+(spr[1].x / 20);
			in_master = INPUT_MINIMAP_LOAD;
			//}

		}


		if ( (input_getscancodejustpressed(SDL_SCANCODE_ESCAPE)) && (g_editorMode == EDITOR_MODE_MINIMAP)) {
			g_dmod.map.load();
			draw_minimap();
			buf_mode = /*false*/0;

		}


		if ( (input_getcharjustpressed(SDLK_m)) && (g_editorMode == EDITOR_MODE_MINIMAP)) {
			//set music # for this block
			in_int = &g_dmod.map.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = INPUT_SCREEN_MIDI;
		}

		if ( (input_getcharjustpressed(SDLK_s)) && (g_editorMode == EDITOR_MODE_MINIMAP)) {
			//set music # for this block
			in_int = &g_dmod.map.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
			in_master = INPUT_SCREEN_TYPE;
		}

		if ( (input_getcharjustpressed(SDLK_q)) && (g_editorMode == EDITOR_MODE_MINIMAP)) {
			save_hard();
			log_info("Info saved.");
			SDL_Event ev;
			ev.type = SDL_QUIT;
			SDL_PushEvent(&ev);
			return 2;
		}

		if ( (sjoy.button[EDITOR_ACTION_RETURN]) && (g_editorMode == EDITOR_MODE_MINIMAP)) {

			if (buf_mode) {
				//lets replace this screen

				buf_mode = /*false*/0;


				if (!load_screen_buf(buffmap.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)])) {
					draw_minimap();
					sjoy.button[EDITOR_ACTION_RETURN] = /*false*/0;
					return 2;
				}


				g_dmod.map.load();

				if (g_dmod.map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)] == 0) {

					(
					 g_dmod.map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)]) = add_new_map();
					//wrongo, let's add the map

					//draw_minimap();


				}

				g_dmod.map.indoor[buf_map] = buffmap.indoor[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];
				g_dmod.map.music[buf_map] = buffmap.music[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20)];

				editor_save_screen(g_dmod.map.loc[buf_map]);

				g_dmod.map.save();
				draw_minimap();
				return 2;
			}

			g_dmod.map.load();

			cur_map = (((spr[1].y+1)*32) / 20)+(spr[1].x / 20);
			if (g_dmod.map.loc[cur_map] == 0) {
				//new map screen
				g_dmod.map.loc[cur_map] = add_new_map();
				g_dmod.map.save();
			}
			else {
				editor_load_screen(g_dmod.map.loc[cur_map]);
			}

			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			k[seq[3].frame[1]].xoffset = -playl;
			//SoundPlayEffect( SOUND_JUMP );

			log_debug("Y is %d X is %d", spr[h].y, spr[h].x);

			m1x = spr[h].x;
			m1y = spr[h].y;
			spr[h].x = m3x;
			spr[h].y = m3y;


			g_editorMode = EDITOR_MODE_SCREEN_TILES;

			spr[h].speed = 50;
			draw_screen_editor();
		}



		/* Cycle the current tile square (displayed at the bottom-right) */
		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES)
			&& (input_getscancodestate(SDL_SCANCODE_PAGEUP) || input_getscancodestate(SDL_SCANCODE_MINUS))) {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;
			cur_tile--;
			if (cur_tile < 0) cur_tile = 0;
		}
		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES)
			&& (input_getscancodestate(SDL_SCANCODE_PAGEDOWN) || input_getscancodestate(SDL_SCANCODE_EQUALS))) {
			spr[h].seq = 3;
			spr[h].seq_orig = 3;

			cur_tile++;
			//if (cur_tile > 127) cur_tile = 127;
		}


		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES) && (input_getcharjustpressed(SDLK_h))) {
			//start althard mode

			g_editorMode = EDITOR_MODE_SCREEN_HARDNESS_INIT;
			return 1; // goto skip_draw;
		}

		if (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS) {
			//mode for it
			if (input_getscancodejustpressed(SDL_SCANCODE_ESCAPE)) {
				//exit mode 8
				g_editorMode = EDITOR_MODE_SCREEN_TILES;
				spr[h].seq = 3;
				spr[h].seq_orig = 3;
				draw_screen_editor();
				return 0;
			}

			/* if (sjoy.keyjustpressed[/\* VK_OEM_4 *\/ 219]) // '[' for US */
			if (input_getscancodejustpressed(SDL_SCANCODE_PAGEUP) || input_getscancodejustpressed(SDL_SCANCODE_LEFTBRACKET)) {
				hard_tile--;
				if (hard_tile < 0) hard_tile = 799;
			}
			/* if (sjoy.keyjustpressed[/\* VK_OEM_6 *\/ 221]) // ']' for US */
			if (input_getscancodejustpressed(SDL_SCANCODE_PAGEDOWN) || input_getscancodejustpressed(SDL_SCANCODE_RIGHTBRACKET)) {
				hard_tile++;
				if (hard_tile > 799) hard_tile = 0;
			}

			if (input_getcharjustpressed(SDLK_c)) {
				//copy tile hardness from current block
				hard_tile = realhard(xy2screentile(spr[1].x, spr[1].y));
			}

			if (input_getcharjustpressed(SDLK_s)) {
				//stamp tile hardness to selected
				cur_ed_screen.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = hard_tile;
				draw_screen_editor();
				g_editorMode = EDITOR_MODE_SCREEN_HARDNESS_INIT;

				return 2;
			}

			if (input_getscancodejustpressed(SDL_SCANCODE_DELETE)) {
				//stamp tile hardness to selected
				cur_ed_screen.t[(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)].althard = 0;
				draw_screen_editor();
				g_editorMode = EDITOR_MODE_SCREEN_HARDNESS_INIT;

				return 2;
			}

			/* Display the hard tile in the clipboard */
			char crapa[20];
			sprintf(crapa, "Copy: %d",hard_tile);
			SaySmall(crapa, 580,400, 255,255,255);

			draw_hard_tile(spr[1].x,spr[1].y, hard_tile);

			if (input_getscancodejustpressed(SDL_SCANCODE_RETURN)) {
				//they want to edit this alt hardness, let's do it
				cur_tile = cur_ed_screen.t[xy2screentile(spr[1].x, spr[1].y)].square_full_idx0;

				xx = cur_tile - (cool * 128);
				Rect.left = spr[1].x+20;
				Rect.top = spr[1].y;
				Rect.right = Rect.left + 50;
				Rect.bottom = Rect.top + 50;

				/* crapRec.top = 0; */
				/* crapRec.left = 95; */
				/* crapRec.bottom = 450; */
				/* crapRec.right = 95+450; */

				/* ZeroMemory(&ddbltfx, sizeof(ddbltfx)); */
				/* ddbltfx.dwSize = sizeof( ddbltfx); */
				spr[1].seq = 0;
				spr[1].pseq = 10;
				spr[1].pframe = 1;


				/* lpDDSTwo->Blt(&crapRec , lpDDSBack, */
				/*               &Rect, DDBLT_DDFX | DDBLT_WAIT,&ddbltfx ); */
				// GFX
				/* In this particular case, we're scaling
				   the whole screen backbuffer by 900%
				   just to scale a single 50x50 square of
				   it... */
				{
					SDL_Rect src, dst;
					src.x = spr[1].x+20;
					src.y = spr[1].y;
					src.w = 50;
					src.h = 50;
					dst.x = 95;
					dst.y = 0;
					dst.w = 450;
					dst.h = 450;
					IOGFX_background->blitStretch(IOGFX_backbuffer, &src, &dst);
				}

				m4x = spr[h].x;
				m4y = spr[h].y;

				spr[1].x = 95;
				spr[1].y = 0;
				selx = 1;
				sely = 1;

				g_editorMode = EDITOR_MODE_TILE_HARDNESS;

				hmap.htile[hard_tile].used = /*true*/1;
				last_modereal = 8;
			}
		}

		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES)
			&& (SDL_GetModState()&KMOD_ALT)
			&& input_getcharjustpressed(SDLK_x)) {
			spr[h].seq = 2;
			spr[h].seq_orig = 2;
			m3x = spr[h].x;
			m3y = spr[h].y;
			spr[h].x = m1x;
			spr[h].y = m1y;
			g_editorMode = EDITOR_MODE_MINIMAP;
			spr[h].speed = 20;
			g_dmod.map.load();
			draw_minimap();
			lsm_kill_all_nonlive_sprites();
			return 2;
		}

		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES) && (sjoy.button[EDITOR_ACTION_ESCAPE])) {
			// jump to map selector selector from map mode
			editor_save_screen(g_dmod.map.loc[cur_map]);
			spr[h].seq = 2;
			spr[h].seq_orig = 2;
			//SoundPlayEffect( SOUND_JUMP );
			m3x = spr[h].x;
			m3y = spr[h].y;
			//Msg("m1y is %d, math is %d",m1y, (20 * (m1y / 20)) < m1y);
			spr[h].x = m1x;
			spr[h].y = m1y;
			g_editorMode = EDITOR_MODE_MINIMAP;
			spr[h].speed = 20;
			g_dmod.map.load();
			draw_minimap();
			lsm_kill_all_nonlive_sprites();
			return 2;
		}


		if (input_getscancodestate(SDL_SCANCODE_LEFT)) {
			spr[h].x -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		}

		if (input_getscancodestate(SDL_SCANCODE_DOWN)) {
			spr[h].y += spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		}

		if (input_getscancodestate(SDL_SCANCODE_UP)) {
			spr[h].y -= spr[h].speed;
			spr[h].seq = spr[h].seq_orig;
			EditorSoundPlayEffect(SOUND_STOP);
		}

		if (spr[h].speed < 1)
			spr[h].speed = 1;
		if (spr[h].y > (y - k[getpic(h)].box.bottom))
			spr[h].y = y - k[getpic(h)].box.bottom;
		if (spr[h].x > (x - k[getpic(h)].box.right))
			spr[h].x = x - k[getpic(h)].box.right;
		if (spr[h].x < 0)
			spr[h].x = 0;
		if (spr[h].y < 0)
			spr[h].y = 0;

		// end human brain (1)


		if ((g_editorMode == EDITOR_MODE_TILE_PICKER) || (g_editorMode == EDITOR_MODE_SCREEN_TILES)
			|| (g_editorMode == EDITOR_MODE_SPRITE_PICKER) || (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS)) {
			if ((selx * 50 + spr[1].x) > 600) {
				spr[1].x = 600 - (selx * 50);
			}
		}

		if ((g_editorMode == EDITOR_MODE_TILE_PICKER)) {
			if ((sely * 50 + spr[1].y) > 450) {
				spr[1].y = 450 - (sely * 50);
			}
		}
		if ((g_editorMode == EDITOR_MODE_SCREEN_TILES) || (g_editorMode == EDITOR_MODE_SPRITE_PICKER)
			|| (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS)) {
			if ((sely * 50 + spr[1].y) > 400) {
				spr[1].y = 400 - (sely * 50);
			}
		}
	} //end if seq is 0
	return 0;
}

/**
 * updateFrame
 *
 * Decide what needs to be blitted next, wait for flip to complete,
 * then flip the buffers.
 */
void AppFreeDinkedit::logic(void)
{
  //    static DWORD        lastTickCount[4] = {0,0,0,0};
  //    static int          currentFrame[3] = {0,0,0};
  //  char buffer[20];
  rect                rcRect;
/*   rect rcRectSrc; */
/*   rect rcRectDest; */
  rect box_crap,box_real;
/*   POINT p; */
  char msg[500];
  char buff[200];
  //	DWORD               delay[4] = {0, 0, 0, 20};
/*   HDC         hdc; */
  int in_crap2 = 0;
  //PALETTEENTRY        pe[256];
/*   HRESULT             ddrval; */
/*   DDBLTFX     ddbltfx; */
  /*BOOL*/int bs[MAX_SPRITES_AT_ONCE];

  int rank[MAX_SPRITES_AT_ONCE];
  int highest_sprite;

  int jj;

  SDL_framerateDelay(&framerate_manager);

  // Decide which frame will be blitted next
  Uint32 thisTickCount = SDL_GetTicks();
  strcpy(buff,"Nothing");
  check_joystick();
  //Scrawl_OnMouseInput();
  rcRect.left = 0;
  rcRect.top = 0;
  rcRect.right = x;
  rcRect.bottom = y;

  if (draw_screen_tiny != -1)
    {

    tiny_again:
      if (draw_screen_tiny  == 769)
	{
	  draw_screen_tiny = -1;
	  lsm_kill_all_nonlive_sprites();
	  //all done
	} else
	{


	  draw_screen_tiny++;

	  copy_front_to_two();


	  if (g_dmod.map.loc[draw_screen_tiny] != 0)
	    {
	      //a map exists here
	      editor_load_screen(g_dmod.map.loc[draw_screen_tiny]);
	      //map loaded, lets display it
	      draw_screen_editor();

	      goto pass_flip;
	    } else goto tiny_again;



	}

    }


/*   while( 1 ) */
/*     { */
/*       ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo, */
/* 				   &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
      // GFX
      IOGFX_backbuffer->blit(IOGFX_background, NULL, NULL);


/*       if( ddrval == DD_OK ) */
/*         { */
/* 	  break; */
/*         } */
/*       if( ddrval == DDERR_SURFACELOST ) */
/*         { */
/* 	  ddrval = restoreAll(); */
/* 	  if( ddrval != DD_OK ) */
/*             { */
/* 	      return; */
/*             } */
/*         } */
/*       if( ddrval != DDERR_WASSTILLDRAWING ) */
/*         { */
/* 	  return; */
/*         } */

/*     } */

 pass_flip:

  memset(&bs,0,sizeof(bs));

  int	max_s = 105;

  //max_sprites_at_once;

  /*for (int r2 = 1; r2 < max_sprites_at_once; r2++)
    {
    if (spr[r2].active) max_s = r2+1;
    }
  */
  int height;



  spr[1].que = 20000;
  if (g_editorMode == EDITOR_MODE_SCREEN_SPRITES) if (   ! ((spr[1].pseq == 10) && (spr[1].pframe == 8)) ) spr[1].que = sp_que;

  if (!in_enabled)
    {
      int r1;
    for (r1 = 1; r1 < max_s+1; r1++)
      {
	int h1;
	highest_sprite = 22024; //more than it could ever be

	rank[r1] = 0;

	for (h1 = 1; h1 < max_s+1; h1++)
	  {
	    if (spr[h1].active)
	      {
		if (bs[h1] == /*FALSE*/0)
		  {
		    //Msg( "Ok,  %d is %d", h1,(spr[h1].y + k[spr[h1].pic].yoffset) );
		    if (spr[h1].que != 0) height = spr[h1].que; else height = spr[h1].y;
		    if ( height < highest_sprite )
		      {
			highest_sprite = height;
			rank[r1] = h1;
		      }

		  }

	      }

	  }
	if (rank[r1] != 0)
	  bs[rank[r1]] = /*TRUE*/1;
      }
    }





  if (!in_enabled)

    for (jj = 1; jj < max_s; jj++)
      {

	int h = rank[jj];
	//Msg("Studying %d.,",h);

	if (spr[h].active)
	  {

	    //        Msg("Sprite %d is active.",h);

	    int greba = 0;

	    if (spr[h].brain == 1)
	      {
			  int state = gui_logic(h);
			  if (state == 1)
				  goto skip_draw;
			  else if (state == 2)
				  return;
	      } //real end of human brain


	    if (spr[h].brain == 2)
	      {
		if (spr[h].y > (y-k[getpic(h)].box.bottom))
		  {
		    spr[h].my -= (spr[h].my * 2);
		    // SoundPlayEffect( SOUND_JUMP );
		  }

		if (spr[h].x > (x-k[getpic(h)].box.right))
		  {
		    spr[h].mx -= (spr[h].mx * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }

		if (spr[h].y < 0)
		  {
		    spr[h].my -= (spr[h].my * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }


		if (spr[h].x < 0)
		  {
		    spr[h].mx -= (spr[h].mx * 2);
		    //SoundPlayEffect( SOUND_JUMP );
		  }


		spr[h].x += spr[h].mx;
		spr[h].y += spr[h].my;


	      }
	    // end robot(2)


	    // Cursor speed through game-compatible, 3-frames seq
	    if (spr[h].seq > 0)
	      {
		if (spr[h].frame < 1)
		  {
		    // Cursor change
		    spr[h].pseq = spr[h].seq;
		    spr[h].pframe = 1;

		    spr[h].frame = 1;
		    spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[1]);
		  }
		else
		  {
		    // not new anim
		    //is it time?
		    if (thisTickCount > spr[h].delay)
		      {
			spr[h].frame++;
			spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);

			spr[h].pseq = spr[h].seq;
			spr[h].pframe = spr[h].frame;

			if (seq[spr[h].seq].frame[spr[h].frame] == -1)
			  {
			    spr[h].frame = 1;
			    spr[h].pseq = spr[h].seq;
			    spr[h].pframe = spr[h].frame;
			    spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
			  }

			if (seq[spr[h].seq].frame[spr[h].frame] == 0)
			  {
			    spr[h].pseq = spr[h].seq;
			    spr[h].pframe = spr[h].frame-1;

			    spr[h].frame = 0;
			    spr[h].seq = 0;
			  }
		      }
		  }
	      }


	    if (g_editorMode == EDITOR_MODE_SCREEN_TILES)
	      {
		//need offset to look right
		k[seq[3].frame[1]].xoffset = -20;
		greba = 20;
	      }
	    if (g_editorMode == EDITOR_MODE_TILE_PICKER || g_editorMode == EDITOR_MODE_SPRITE_PICKER)
	      {
		//pick a tile, needs no offset
		k[seq[3].frame[1]].xoffset = 0;
		greba = 0;
	      }

	    //		if (  !(( h == 1) & (mode == 9)) )
	    if (!((h == 1) && (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS_INIT)))
	      {
		if (draw_screen_tiny == -1)
		  draw_sprite(IOGFX_backbuffer, h);
		else
		  draw_sprite(IOGFX_background, h);
	      }

	    //Msg("Drew %d.",h);

	  skip_draw:
	    if (spr[h].brain == 1)
	      {
		if (g_editorMode == EDITOR_MODE_SCREEN_TILES || g_editorMode == EDITOR_MODE_TILE_PICKER)
		  {
		    /* Draw the tile squares selector, an expandable
		       array of white non-filled squares */
		    int y;
		    for (y = 0; y < sely; y++)
		      {
			int x;
			for (x = 0; x < selx; x++)
			  {
/* 			    ddrval = lpDDSBack->BltFast( (spr[h].x+(50 *x))+greba,spr[h].y+(50 * y), k[getpic(h)].k, */
/* 							 &k[getpic(h)].box  , DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT ); */
			    // GFX
			    {
			      SDL_Rect dst;
			      dst.x = spr[h].x + 50*x + greba;
			      dst.y = spr[h].y + 50*y;
			      IOGFX_backbuffer->blit(GFX_k[getpic(h)].k, NULL, &dst);
			    }
			  }
		      }
		  }


		if ((g_editorMode == EDITOR_MODE_TILE_HARDNESS))
		  {
		    /* Display the current "pencil"/square to draw hardness with */
		    int yy;
		    for (yy = 0; yy < sely; yy++)
		      {
			int xx;
			for (xx = 0; xx < selx; xx++)
			  {
/* 			    ddrval = lpDDSBack->BltFast( spr[h].x+(9 * xx),spr[h].y+(9 * yy), k[getpic(h)].k, */
/* 							 &k[getpic(h)].box  , DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT ); */
			    // GFX
			    {
			      SDL_Rect dst;
			      dst.x = spr[h].x + 9*xx;
			      dst.y = spr[h].y + 9*yy;
			      IOGFX_backbuffer->blit(GFX_k[getpic(h)].k, NULL, &dst);
			    }
			  }
		      }
		  }
	      }
	  }
      }


  if (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS_INIT)
    {
      g_editorMode = EDITOR_MODE_SCREEN_HARDNESS;

      fill_whole_hard();
      lsm_kill_all_nonlive_sprites();
      place_sprites();

      /* draw_screen_editor();


		rcRect.top = 0;
		rcRect.right = x;
		rcRect.bottom = y;
		ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo,
		&rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
      */
      drawallhard();

      rcRect.left = 0;
      rcRect.right = 640;
      rcRect.top = 0;
      rcRect.bottom = 400;
/*       ddrval = lpDDSTwo->BltFast( 0, 0, lpDDSBack, */
/* 				  &rcRect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
/*       if (ddrval != DD_OK) dderror(ddrval); */
      // GFX
      IOGFX_background->blit(IOGFX_backbuffer, NULL, NULL);

      lsm_kill_all_nonlive_sprites();
    }


  //sprintf(msg, "k[1] top is %d",k[1].box.top);

  //prepare to display misc messages for debug purposes

/*   if (lpDDSBack->GetDC(&hdc) == DD_OK) */
/*     { */
/*       SetBkMode(hdc, TRANSPARENT);  */

/*       //   SetBkMode(hdc, OPAQUE); */
/*       SetBkColor(hdc, 1); */
/*       SetTextColor(hdc,RGB(200,200,200)); */
      FONTS_SetTextColor(200, 200, 200);

      //	TextOut(hdc,0,0, msg,lstrlen(msg));
      //	   if (mode == 0) strcpy(msg,"");
      if (g_editorMode == EDITOR_MODE_INIT)
	strcpy(msg,"");

      if (g_editorMode == EDITOR_MODE_MINIMAP)
	{
	  if (20 * (spr[1].y / 20) != spr[1].y)
	    {
	      spr[1].y += 10;
	    }
	  sprintf(msg,
		  "Map # %d - Press ENTER to edit, SPACE to detail map. (%d)  (Q) to quit and save. L to replace a "
		  "screen from another map file.  Z to toggle this help text. S to modify screentype. M to modify midi.",
		  g_editorMode,
		  (((spr[1].y+1)*32) / 20)+(spr[1].x / 20));
/* 		  g_dmod.map.loc[(((spr[1].y+1)*32) / 20)+(spr[1].x / 20) ]); */
	}
      if (g_editorMode == EDITOR_MODE_SCREEN_TILES)
	{
	  //((x-1) - (x / 12))*50, (x / 12) * 50,
	  sprintf(msg,
		  "Map # %d, (C)opy or (S)tamp tile. ESC to exit to map picker. ENTER to edit hardness. TAB for sprite edit mode. 1-10 for tilescreens. (hold alt, crtl or shift for more) SPACE to show hardness of screen. (I)nfo on sprites."
		  " V to change vision, B to change maps base .C file, H to edit tile hardness.",
		  g_editorMode);
/* 		  cur_map, */
/* 		  cur_tile, */
/* 		  cur_ed_screen.t[crap].num); */
	}
      if (g_editorMode == EDITOR_MODE_TILE_PICKER)
	sprintf(msg,
		"Map # %d - Current tile # %d - ENTER to choose, SPACE to edit hardness.",
		g_editorMode,
		cur_map);
/* 		(((spr[1].y+1)*12) / 50)+(spr[1].x / 50)); */
      if (g_editorMode == EDITOR_MODE_TILE_HARDNESS)
	{
	  sprintf(msg, "X:%d Y:%d: Density index %d  Z to harden, X to soften, A for low-hard, S for ???.\n"
		  "Alt+X/A/S to fill hardness. Shift+direction for larger brush. ENTER or ESC to exit.",
		  (spr[1].x / 9) -9,(spr[1].y / 9) +1, hard_tile);
	}

      if (g_editorMode == EDITOR_MODE_SPRITE_PICKER)
	{

	  if (sp_seq == 0)
	    {
	      sprintf(msg, "Choose sequence and press ENTER.  PageDown for next page, PageUp for previous. ESC or TAB to exit. (now on page %d)",
		      1+(sp_picker / 96));
	    } else
	    {
	      sprintf(msg, "Choose a sprite from sequence %d.  Enter to place sprite, TAB to exit or ESC to return to previous screen."
		      " E to edit depth dot and hardbox",
		      sp_seq);

	    }
	}

      if (g_editorMode == EDITOR_MODE_SCREEN_SPRITES)
	{

	  char crap7[80];
	  if (sp_screenmatch) strcpy(crap7, "ScreenMatch is ON."); else strcpy(crap7, "ScreenMatch is OFF");

	  sprintf(msg, "Press ENTER to pickup/putdown sprite. Space to show hardness.  E to edit/pick new sprite. SHIFT to move fast. (S)tamp sprite. PageUp&"
		  "PageDown to scale (now at %d). DEL to erase sprite.  Press 1 through 9 to change sprite attributes. (hold alt or shift for more)  Last sprite touched: %d  %s (M to toggle)"
		  "Hold Z or X + arrow keys to trim a sprite. V to change Vision mode. X: %d Y: %d",
		  spr[1].size,last_sprite_added,crap7, spr[1].x, spr[1].y);

	  //lets draw the depth dot




	}


      if (g_editorMode == EDITOR_MODE_SPRITE_HARDNESS)
	{


	  if (sp_mode == 0)
	    {
	      sprintf(msg, "Editing depth dot for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini.",
		      sp_seq,sp_frame);

	    }

	  if ( (sp_mode == 1) )

	    {
	      sprintf(msg, "Editing hardbox up left cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.left,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.top);

	    }

	  if ( (sp_mode == 2) )

	    {
	      sprintf(msg, "Editing hardbox down right cordinate for Seq %d, frame %d.  SHIFT to move fast. Control to move one"
		      " pixel.  TAB for next edit option. ESC to return to sprite picker. S to save to dink.ini. X: %d Y: %d",
		      sp_seq,sp_frame, k[seq[sp_seq].frame[sp_frame]].hardbox.right,
		      k[seq[sp_seq].frame[sp_frame]].hardbox.bottom);

	    }






	}


      if (g_editorMode == EDITOR_MODE_SCREEN_HARDNESS)
	{
	  int screentile = xy2screentile(spr[1].x, spr[1].y);
	  int sourcetile = cur_ed_screen.t[screentile].square_full_idx0;
	  int defaulthardness = hmap.btile_default[sourcetile];
	  int curhardness = realhard(screentile);
	  char str[100];
	  if (defaulthardness == curhardness)
	    sprintf(str, "Current %03d (default %03d tile %03d)", curhardness, defaulthardness, sourcetile);
	  else
	    sprintf(str, "Current %03d (default %03d tile %03d) (alternate)", curhardness, defaulthardness, sourcetile);
	    
	  sprintf(msg, "Alternative Tile Hardness Selector: Press S to stamp this tiles hardness."
		  "  DEL to remove alternate hardness."
		  "  C to copy from current block."
		  "  PageUp&PageDown to cycle.\n"
		  "ESCAPE to exit.\n"
		  "%s", str);
	}

      {
       int x;
       for (x = 0; x < 256; x++)
         {
           if (input_getscancodestate((SDL_Scancode)x))
             {
               sprintf(msg + strlen(msg), " (Scancode %i '%s')",
		       x,
		       //SDL_GetScancodeName(x),
		       SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)x)));
             }
         }
      }

      rcRect.left = 0;
      rcRect.top = 400;
      if (g_editorMode == EDITOR_MODE_TILE_HARDNESS) rcRect.top = 450;
      rcRect.right = 590;
      rcRect.bottom = 480;
      if (show_display)
	{
	  /* Display help message at the bottom of the screen */
/* 	  DrawText(hdc,msg,lstrlen(msg),&rcRect,DT_WORDBREAK); */
	  // FONTS
	  print_text_wrap(msg, &rcRect, 0, 0, FONT_SYSTEM);
	}

/*       lpDDSBack->ReleaseDC(hdc); */

/*     } /\* GetDC(&hdc) *\/ */


  if ((g_editorMode == EDITOR_MODE_MINIMAP))
    {
      if (input_getcharjustpressed(SDLK_z))
	{
	  if (show_display) show_display = /*false*/0;
	  else show_display = /*true*/1;
	}
    }


  if ( (g_editorMode == EDITOR_MODE_SCREEN_SPRITES) | (g_editorMode == EDITOR_MODE_SCREEN_TILES) )
    {
      /* Show sprites info */
      if (input_getcharstate(SDLK_i))
	{
	  int j;
	  for (j = 1; j < 100; j++)
	    {
	      if (cur_ed_screen.sprite[j].active == /*true*/1)
		{

/* 		  ddbltfx.dwSize = sizeof(ddbltfx); */
/* 		  ddbltfx.dwFillColor = 230; */

		  //info on the sprites  sprite info
/* 		  int temp = s_index[cur_ed_screen.sprite[j].seq].s + cur_ed_screen.sprite[j].frame; */


		  int sprite2 = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y,0,
						cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
						cur_ed_screen.sprite[j].size);
		  rect_copy(&spr[sprite2].alt , &cur_ed_screen.sprite[j].alt);

		  get_box(sprite2, &box_crap, &box_real, skip_screen_clipping());

		  lsm_remove_sprite(sprite2);

		  //box_crap.top = box_crap.sprite[j].y - 25;
		  box_crap.bottom = box_crap.top + 50;
		  box_crap.left = box_crap.left + ( (box_crap.right - box_crap.left) / 2);
		  box_crap.right = box_crap.left+ 50;

/* 		  ddrval = lpDDSBack->Blt(&box_crap ,k[seq[10].frame[5]].k,&k[seq[10].frame[5]].box, DDBLT_WAIT, &ddbltfx); */
		  //       	if (ddrval != DD_OK) dderror(ddrval);
		  // GFX
		  {
		    SDL_Rect dst;
		    dst.x = box_crap.left;
		    dst.y = box_crap.top;
		    /* Simplified blit, no scaling, the sprite is already 50x50 */
		    /* We need to avoid transparency though */
		    IOGFX_backbuffer->blitNoColorKey(GFX_k[seq[10].frame[5]].k, NULL, &dst);
		  }

		  char crap5[200];

                  char crap6[20];

		  strcpy(crap6,"");
		  if (cur_ed_screen.sprite[j].hard == 0) strcpy(crap6,"HARD");

		  sprintf(crap5, "B: %d %s",cur_ed_screen.sprite[j].brain,crap6);

		  if (cur_ed_screen.sprite[j].type == 0)
		    {

		      SaySmall(crap5,box_crap.left+3,box_crap.top+3,255,255,255);
		    }

		  if (cur_ed_screen.sprite[j].type > 0)
		    {
		      SaySmall(crap5,box_crap.left+3,box_crap.top+3,255,0,0);
		    }
		  if (strlen(cur_ed_screen.sprite[j].script) > 1)
		    SaySmall(cur_ed_screen.sprite[j].script,box_crap.left+3,box_crap.top+35,255,0,0);

		  sprintf(crap6,"%d",j);

		  SaySmall(crap6,box_crap.left+20,box_crap.top-15,0,255,0);


		}
	    }
	}

    }
  if (g_editorMode == EDITOR_MODE_SPRITE_HARDNESS)
    {
/*       ddbltfx.dwSize = sizeof(ddbltfx); */
/*       ddbltfx.dwFillColor = 230; */

      if (sp_mode == 0)
	{
	  //draw depth dot for sprite attribute edit

/* 	  box_crap.top = spr[2].y; */
/* 	  box_crap.bottom = spr[2].y+1; */
/* 	  box_crap.left = spr[2].x - 20; */
/* 	  box_crap.right = spr[2].x + 20; */


/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) dderror(ddrval); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = spr[2].x - 20;
	    dst.y = spr[2].y;
	    dst.w = 40;
	    dst.h = 1;
	    IOGFX_backbuffer->fillRect(&dst, 41, 0, 49);
	  }

/* 	  box_crap.top = spr[2].y-20; */
/* 	  box_crap.bottom = spr[2].y+20; */
/* 	  box_crap.left = spr[2].x; */
/* 	  box_crap.right = spr[2].x +1; */


/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) dderror(ddrval); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = spr[2].x;
	    dst.y = spr[2].y - 20;
	    dst.w = 1;
	    dst.h = 40;
	    IOGFX_backbuffer->fillRect(&dst, 41, 0, 49);
	  }
	}

      if  ((sp_mode == 1) | (sp_mode == 2) )
	{
	  //draw hardbox dot for sprite attribute edit

/* 	  box_crap = k[seq[sp_seq].frame[sp_frame]].hardbox; */

/* 	  OffsetRect(&box_crap,320,200); */


/* 	  ddrval = lpDDSBack->Blt(&box_crap ,NULL,NULL, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
/* 	  if (ddrval != DD_OK) Msg("Error with drawing hard block... you know why."); */
	  // GFX
	  {
	    SDL_Rect dst;
	    dst.x = k[seq[sp_seq].frame[sp_frame]].hardbox.left + 320;
	    dst.y = k[seq[sp_seq].frame[sp_frame]].hardbox.top + 200;
	    dst.w = k[seq[sp_seq].frame[sp_frame]].hardbox.right - k[seq[sp_seq].frame[sp_frame]].hardbox.left;
	    dst.h = k[seq[sp_seq].frame[sp_frame]].hardbox.bottom - k[seq[sp_seq].frame[sp_frame]].hardbox.top;
	    IOGFX_backbuffer->fillRect(&dst, 41, 0, 49);
	  }
	}
    }

  
  if (in_enabled)
    {
      //text box is open, lets act accordingly
      //check_joystick();
      if (input_getscancodestate(SDL_SCANCODE_RETURN) || input_getscancodestate(SDL_SCANCODE_KP_ENTER))
	{
	  //exit text mode
	  
	  if (in_command == 2)
	    {
	      if (in_string != NULL)
		strcpy(in_string, in_temp);
	      else
		log_error("Error, in_char pointer not set, can't issue a value.");
	    }

	  if (in_command == 1)
	    {
	      if (in_int != NULL)
		{
		  char *stop;
		  int in_crap = strtol(in_temp, &stop,10);
		  
		  in_crap2 = in_crap;
		  
		  if ((old_command == INPUT_SCREEN_MIDI) || (old_command == INPUT_SCREEN_TYPE))
		    g_dmod.map.load();
		  
		  *in_int = in_crap2;

		  if ((old_command == INPUT_SCREEN_MIDI) || (old_command == INPUT_SCREEN_TYPE))
		    g_dmod.map.save();
		}
	      else
		{
		  log_error("in_int pointer not set, can't issue a value.");
		}
	    }

	  in_command = 0;

	  if (in_huh == 3)
	    {
	      if (in_crap2 == 3)
		{
		  //default duck settings
		  sp_speed = 1;
		  sp_base_walk = 20;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 33;
		  sp_type = 1;
		  sp_que = 0;
		  sp_hard = 1;
		}
	      
	      if (in_crap2 == 4)
		{
		  //default pig settings
		  sp_speed = 1;
		  sp_base_walk = 40;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  sp_que = 0;
		  sp_hard = 1;
		}

	      if (in_crap2 == 9)
		{
		  //default diag settings
		  sp_speed = 1;
		  sp_base_walk =  (sp_seq / 10) * 10;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  strcpy(sp_script, "");
		  sp_que = 0;
		  sp_hard = 1;
		}

	      if (in_crap2 == 10)
		{
		  //default diag settings
		  sp_speed = 1;
		  sp_base_walk =  (sp_seq / 10) * 10;
		  sp_base_idle = -1;
		  sp_base_hit = -1;
		  sp_base_attack = -1;
		  sp_timer = 66;
		  sp_type = 1;
		  strcpy(sp_script, "");
		  sp_que = 0;
		  sp_hard = 1;
		}
	    }
	  
	  show_text_box(0);
	}
      else if (input_getscancodejustpressed(SDL_SCANCODE_ESCAPE))
	{
	  show_text_box(0);
	}
      else if (input_getscancodejustpressed(SDL_SCANCODE_BACKSPACE))
	//	if (getkey(8)) //this is a much faster backspace than the above
	{
	  if (strlen(in_temp) > 0)
	    in_temp[strlen(in_temp)-1] = 0;

	}

      if (in_enabled)
	{
	  Say(in_temp,260,200);
	}
      else
	{
	  /* Redraw last mode */
	  if (g_editorMode == EDITOR_MODE_MINIMAP)
	    {
	      if (old_command == INPUT_MINIMAP_LOAD)
		draw_minimap_buff();
	      else
		draw_minimap();
	    }
	  else
	    {
	      draw_screen_editor();
	    }
	}
    }


  if (in_master != 0) check_in();


  //open a text window?

  if (in_onflag)
    {
      //start it up

      //copy screen to Two
/*       SetRect(&rcRect, 0, 0, 640, 480); */
/*       ddrval = lpDDSTwo->Blt( &rcRect, lpDDSBack, &rcRect, DDBLT_WAIT, NULL); */
/*       if (ddrval != DD_OK) dderror(ddrval); */
      // GFX
      // TODO: use copy_front_to_two()
	  IOGFX_background->blit(IOGFX_backbuffer, NULL, NULL);


      strcpy(in_temp,in_default);
      in_x = 270;
      in_y = 190;
      in_onflag = /*false*/0;
      show_text_box(1);



    }



  //MAIN PAGE FLIP DONE HERE


  if (input_getscancodestate(SDL_SCANCODE_SPACE) && (g_editorMode != 1))
    {
      drawallhard();

    }




  if (draw_screen_tiny != -1)
    {

      int huh = 0;
      //if (draw_screen_tiny > 32) huh = 1;
      shrink_screen_to_these_cords(  (draw_screen_tiny-1) * 20 - ((((draw_screen_tiny-1) / 32) * 640) )   ,   ((((draw_screen_tiny-1) / 32)- huh) * 20));
      //Msg("Just flipped # %d", draw_screen_tiny);

    }



/*   if (!windowed) */
/*     { */

/*       while( 1 ) */
/* 	{ */
/* 	  ddrval = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT ); */
/* 	  if( ddrval == DD_OK ) */
/* 	    { */
/* 	      break; */
/* 	    } */
/* 	  if( ddrval == DDERR_SURFACELOST ) */
/* 	    { */
/* 	      ddrval = restoreAll(); */
/* 	      if( ddrval != DD_OK ) */
/* 		{ */
/* 		  break; */
/* 		} */
/* 	    } */
/* 	  if( ddrval != DDERR_WASSTILLDRAWING ) */
/* 	    { */

/* 	      dderror(ddrval); */
/* 	    } */
/* 	}  */

/*       if (draw_screen_tiny != -1)  */
/* 	{ */
	  //extra flip

/* 	  ddrval = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT ); */
/* 	} */


/*     } else */
/*     { */
      g_display->clear();
      g_display->flipStretch(IOGFX_backbuffer);
      /*
      //windowed mode, no flipping
      p.x = 0; p.y = 0;
      ClientToScreen(hWndMain, &p);
      GetClientRect(hWndMain, &rcRectDest);

      //rcRectDest.top += winoffset;
      rcRectDest.bottom = 480;
      rcRectDest.right = 640;

      OffsetRect(&rcRectDest, p.x, p.y);
      SetRect(&rcRectSrc, 0, 0, 640, 480);




      ddbltfx.dwSize = sizeof(ddbltfx);

      ddbltfx.dwDDFX = DDBLTFX_NOTEARING;
      ddrval = lpDDSPrimary->Blt( &rcRectDest, lpDDSBack, &rcRectSrc, DDBLT_DDFX | DDBLT_WAIT, &ddbltfx);
      */
/*     } */
} /* updateFrame */


int load_editor_sounds()
{
  int i;

  /*
   * Load all sounds
   */
  for(i = 0; i < NUM_SOUND_EFFECTS; i++)
    {
      char *filename = szSoundEffects[i];
      log_info("Loading sound %s [%d]", filename, i);
      
      /* Try from resources */
      SDL_RWops* rwops;
      rwops = find_resource_as_rwops(filename);
      if (rwops != NULL)
	CreateBufferFromWaveFile_RW(rwops, 1, i);
      else
	log_error("Cannot load sound effect %s, from resources or from %s",
		  filename, paths_getpkgdatadir());
    }
  return 1;
}


AppFreeDinkedit::AppFreeDinkedit() {
	description = _("Edit the Dink Smallwood game or one of its D-Mods.");
	splash_path = "Tiles/esplash.bmp";
}

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
void AppFreeDinkedit::init()
{
  /** SETUP **/
  /* Manually setup basic sequences */
  {
    int i = 1;
    for (; i <= 4; i++)
      {
	spr[i].active = false;
	spr[i].x = 10;
	spr[i].y = 10;
	spr[i].my = (rand() % 3)+1;
	spr[i].mx = (rand() % 3)+1;
	spr[i].seq = 1;
	spr[i].speed = (rand() % 40)+1;
	spr[i].brain = 2;
	spr[i].pseq = 10;
	spr[i].pframe = 3;
	spr[i].size = 100;
      }
  }

  spr[1].active = true;
  spr[1].x = 0;
  spr[1].y = 0;
  spr[1].speed = 20;
  spr[1].brain = 1;
  rect_set(&spr[1].alt,0,0,0,0);
  spr[1].pseq = 10;
  spr[1].pframe = 3;
  spr[1].seq = 0;
  spr[1].seq = 2;
  
  //sprite sequence setup
  seq[1].frame[1] = seq[10].frame[1];
  seq[1].frame[2] = seq[10].frame[2];
  seq[1].frame[3] = seq[10].frame[3];
  // FIX: end of sequence is 0, not -1. This made the editor crash.
  // seq[1].frame[4] = -1;
  seq[1].frame[4] = 0;

  seq[1].delay[1] = 50;
  seq[1].delay[2] = 50;
  seq[1].delay[3] = 50;
  seq[1].delay[4] = 50;

  seq[2].frame[1] = seq[10].frame[4];
  seq[2].frame[2] = seq[10].frame[4];
  seq[2].frame[3] = seq[10].frame[4];
  seq[2].frame[4] = 0;

  seq[2].delay[1] = 10;
  seq[2].delay[2] = 10;
  seq[2].delay[3] = 10;
  seq[2].delay[4] = 10;
  
  seq[3].frame[1] = seq[10].frame[5];
  seq[3].frame[2] = seq[10].frame[5];
  seq[3].frame[3] = seq[10].frame[5];
  seq[3].frame[4] = 0;
  
  seq[3].delay[1] = 5;
  seq[3].delay[2] = 5;
  seq[3].delay[3] = 5;
  seq[3].delay[4] = 5;
  
  seq[4].frame[1] = seq[10].frame[1];
  seq[4].frame[2] = seq[10].frame[1];
  seq[4].frame[3] = 0;
  seq[4].frame[4] = 0;
  
  seq[4].delay[1] = 2;
  seq[4].delay[2] = 2;
  seq[4].delay[3] = 2;
  seq[4].delay[4] = 2;
  
  if (sound_on)
    load_editor_sounds();

  g_editorMode = EDITOR_MODE_INIT;
  cur_tile = 1;
  
  playl = 20;
  playx = 620;
  playy = 480;
  sp_seq = 0;

  /* Relative mode with infinite out-of-screen scrolling */
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

/* Global game shortcuts - just full-screen toggle actually */
bool freedinkedit_input_global_shortcuts(SDL_Event* ev) {
  if (ev->type != SDL_KEYDOWN)
    return false;
  if (ev->key.repeat)
    return false;

  if (ev->key.keysym.scancode == SDL_SCANCODE_RETURN)
    {
      g_display->toggleFullScreen();
      return true;
    }
  return false;
}

void freedinkedit_input_window(SDL_Event* ev) {
	if (ev->type == SDL_WINDOWEVENT) {
		if (ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			int w = ev->window.data1;
			int h = ev->window.data2;
			g_display->onSizeChange(w, h);
		}
	}
}

void AppFreeDinkedit::input(SDL_Event* ev) {
	freedinkedit_input_window(ev);
	if (ev->type == SDL_KEYUP
			&& input_getscancodestate(ev->key.keysym.scancode) == SDL_PRESSED) {
		// always tell the game when the player releases a key
		input_update(ev);
	} else {
		int processed = false;
		if ((ev->type == SDL_KEYDOWN || ev->type == SDL_KEYUP)
				&& (ev->key.keysym.mod & KMOD_ALT)) {
			processed = freedinkedit_input_global_shortcuts(ev);
		}
		if (!processed)
			// forward all events to the editor
			input_update(ev);
	}

	if (ev->type == SDL_TEXTINPUT
			&& strlen(in_temp) < in_max
			&& isprint(ev->text.text[0]))
		sprintf(in_temp + strlen(in_temp), "%c", ev->text.text[0]);

	if (g_editorMode == EDITOR_MODE_SCREEN_SPRITES)
		freedinkedit_update_cursor_position(ev);
}


/**
 * Bootstrap
 */
int main(int argc, char *argv[])
{
	// Create app
	// Store it in the heap as Emscripten will trash the stack
	AppFreeDinkedit* freedinkedit = new AppFreeDinkedit();
	int ret = freedinkedit->main(argc, argv);
	delete freedinkedit;
	return ret;
}
