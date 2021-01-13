/**
 * Displayed screen

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2014, 2015  Sylvain Beucler

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

#include "game_engine.h"
#include "live_sprites_manager.h"
#include "live_screen.h"
#include "editor_screen.h"
#include "hardness_tiles.h"
#include "IOGfxPrimitives.h"
#include "gfx.h"
#include "gfx_sprites.h"
#include "log.h"
#include "dinkini.h"

/* base editor screen */
struct editor_screen cur_ed_screen;

/* hardness */
unsigned char screen_hitmap[600+1][400+1]; /* hit_map */

int playx = 620;
int playl = 20;
int playy = 400;

/* Screen transition */
static int move_screen = 0;
static int move_counter = 0;
/*bool*/int transition_in_progress = /*false*/0;


void live_screen_init() {
  memset(&screen_hitmap, 0, sizeof(screen_hitmap));
}

/**
 * Fills a int[MAX_SPRITES_AT_ONCE] with the index of the current
 * screen's sprites, sorted by ascending height/queue.
 */
void screen_rank_game_sprites(int* rank)
{
  memset(rank, 0, MAX_SPRITES_AT_ONCE * sizeof(int));

  int r1 = 0;
  int already_checked[MAX_SPRITES_AT_ONCE+1];
  memset(already_checked, 0, sizeof(already_checked));
  for (r1 = 0; r1 < last_sprite_created; r1++)
    {
      int highest_sprite = 22000; //more than it could ever be
      rank[r1] = 0;

      int h1;
      for (h1 = 1; h1 <= last_sprite_created; h1++)
	{
	  if (already_checked[h1] == 0 && spr[h1].active)
	    {
	      int height;
	      if (spr[h1].que != 0)
		height = spr[h1].que;
	      else
		height = spr[h1].y;
	      
	      if (height < highest_sprite)
		{
		  highest_sprite = height;
		  rank[r1] = h1;
		}
	    }
	}
      if (rank[r1] != 0)
	already_checked[rank[r1]] = 1;
    }
}

void fill_hard_sprites()
{
  int rank[MAX_SPRITES_AT_ONCE];
  screen_rank_game_sprites(rank);

  int r1 = 0;
  for (; r1 < last_sprite_created && rank[r1] > 0; r1++)
    {
      int h = rank[r1];
      if (spr[h].active)
	{
	  // Msg("proccesing sprite %d", h);
	  if (spr[h].sp_index != 0)
	    {
	      //Msg("has spindex of %d is_warp is %d",spr[h].sp_index,cur_ed_screen.sprite[spr[h].sp_index].is_warp);
	      if (cur_ed_screen.sprite[spr[h].sp_index].hard == 0)
		{
		  add_hardness(h,100+spr[h].sp_index);
		  //Msg("added warp hardness for %d", spr[h].sp_index);
		}
	    }
	  else
	    {
	      if (spr[h].hard == 0)
		{
		  //Msg("adding a new sprite hardness %d (not from editor)", h);
		  add_hardness(h, 1);
		}
	    }
	}
    }
}


void fill_whole_hard(void)
{
  int til;
  for (til=0; til < 96; til++)
    {
      int offx = (til * 50 - ((til / 12) * 600));
      int offy = (til / 12) * 50;
      int x, y;
      for (x = 0; x < 50; x++)
	for (y = 0; y < 50; y++)
	  screen_hitmap[offx +x][offy+y] = hmap.htile[  realhard(til)  ].hm[x][y];
    }
}


//add hardness from a sprite
void add_hardness (int sprite, int num)
{
  int xx;
  for (xx = spr[sprite].x + k[getpic(sprite)].hardbox.left; xx < spr[sprite].x + k[getpic(sprite)].hardbox.right; xx++)
    {
      int yy;
      for (yy = spr[sprite].y + k[getpic(sprite)].hardbox.top; yy < spr[sprite].y + k[getpic(sprite)].hardbox.bottom; yy++)
	{
	  if ( (xx-20 > 600) | (xx-20 < 0)| (yy > 400) | (yy < 0)) {
	    /* Nothing */
	  } else {
	    screen_hitmap[xx-20][yy] = num;
	  }
	}
    }
}



unsigned char get_hard_map(int x1, int y1)
{


        if ((x1 < 0) || (y1 < 0)) return(0);
        if ((x1 > 599) ) return(0);
        if (y1 > 399) return(0);


        int til = (x1 / 50) + ( ((y1 / 50)) * 12);
        //til++;

        int offx = x1 - ((x1 / 50) * 50);


        int offy = y1 - ((y1 / 50) * 50);

        //Msg("tile %d ",til);

        return( hmap.htile[ realhard(til )  ].hm[offx][offy]);

}



void fill_hardxy(rect box)
{
  int x1, y1;
  //Msg("filling hard of %d %d %d %d", box.top, box.left, box.right, box.bottom);

  if (box.right > 600)  box.right  = 600;
  if (box.top < 0)      box.top    = 0;
  if (box.bottom > 400) box.bottom = 400;
  if (box.left < 0)     box.left   = 0;

  for (x1 = box.left; x1 < box.right; x1++)
    for (y1 = box.top; y1 < box.bottom; y1++)
      screen_hitmap[x1][y1] = get_hard_map(x1,y1);
}



/**
 * Check whether planned new position (x1,y1) is solid
 * 
 * Only used in 'check_if_move_is_legal'
 *
 * Returns: 0 if not solid, !0 otherwise
 */
unsigned char get_hard(int x1, int y1, int screenlock)
{
  // Fix "going diag on screen border + screenlock avoids hardness"
  // TODO: same for screen with no adjacent screen
  if (screenlock)
    {
      if (x1 < 0)        x1 = 0;
      else if (x1 > 599) x1 = 599;

      if (y1 < 0)        y1 = 0;
      else if (y1 > 399) y1 = 399;
    }
  if ((x1 < 0) || (y1 < 0) || (x1 > 599) || (y1 > 399))
    return 0;
  
  int value = screen_hitmap[x1][y1];
  return(value);
}


/*bool*/int get_box (int h, rect * box_scaled, rect * box_real, bool skip_screen_clipping)
{
  int x_offset, y_offset;

  int mplayx = playx;
  int mplayl = playl;
  int mplayy = playy;

  if (spr[h].noclip)
    {
      mplayx = 640;
      mplayl = 0;
      mplayy = 480;
    }

  // added to fix frame-not-in-memory immediately
  if (getpic(h) < 1)
    {
      if (spr[h].pseq != 0)
	check_seq_status(spr[h].pseq);
    }

  // if frame is still not in memory:
  if (getpic(h) < 1)
    {
		log_warn("Yo, sprite %d has a bad pic. Seq %d, Frame %d",
		         h, spr[h].pseq, spr[h].pframe);
      goto nodraw;
    }

  *box_real = k[getpic(h)].box;

  /* This doesn't really make sense, but that's the way the game was
     released, so we keep it for compatibility */
  {
    rect krect;
    rect_copy(&krect, &k[getpic(h)].box);

    double size_ratio = spr[h].size / 100.0;
    int x_compat = krect.right  * (size_ratio - 1) / 2;
    int y_compat = krect.bottom * (size_ratio - 1) / 2;

    int center_x = k[getpic(h)].xoffset;
    int center_y = k[getpic(h)].yoffset;
    box_scaled->left   = spr[h].x - center_x - x_compat;
    box_scaled->top    = spr[h].y - center_y - y_compat;

    box_scaled->right  = box_scaled->left + krect.right  * size_ratio;
    box_scaled->bottom = box_scaled->top  + krect.bottom * size_ratio;
  }

  if (spr[h].alt.right != 0 || spr[h].alt.left != 0 || spr[h].alt.top != 0)
    {
      // checks for correct box stuff
      if (spr[h].alt.left < 0)
	spr[h].alt.left = 0;
      if (spr[h].alt.left > k[getpic(h)].box.right)
	spr[h].alt.left = k[getpic(h)].box.right;

      if (spr[h].alt.top < 0)
	spr[h].alt.top = 0;
      if (spr[h].alt.top > k[getpic(h)].box.bottom)
	spr[h].alt.top = k[getpic(h)].box.bottom;

      if (spr[h].alt.right < 0)
	spr[h].alt.right = 0;
      if (spr[h].alt.right > k[getpic(h)].box.right)
	spr[h].alt.right = k[getpic(h)].box.right;

      if (spr[h].alt.bottom < 0)
	spr[h].alt.bottom = 0;
      if (spr[h].alt.bottom > k[getpic(h)].box.bottom)
	spr[h].alt.bottom = k[getpic(h)].box.bottom;

      box_scaled->left += spr[h].alt.left;
      box_scaled->top  += spr[h].alt.top;
      box_scaled->right  = box_scaled->right  - (k[getpic(h)].box.right  - spr[h].alt.right);
      box_scaled->bottom = box_scaled->bottom - (k[getpic(h)].box.bottom - spr[h].alt.bottom);

      rect_copy(box_real, &spr[h].alt);
    }

  //********* Check to see if they need to be cut down and do clipping

  if (spr[h].size == 0)
    spr[h].size = 100;

  if (skip_screen_clipping)
    goto do_draw;

  if (box_scaled->left < mplayl)
    {
      x_offset = box_scaled->left * (-1) + mplayl;
      box_scaled->left = mplayl;

      if (spr[h].size == 100)
	box_real->left += x_offset;
      else
	box_real->left += (x_offset * 100) / spr[h].size;

      if (box_scaled->right - 1 < mplayl)
	goto nodraw;
    }

  if (box_scaled->top < 0)
    {
      y_offset = box_scaled->top * (-1);
      box_scaled->top = 0;

      if (spr[h].size == 100)
	box_real->top += y_offset;
      else
	box_real->top += (y_offset * 100) / spr[h].size;

      if (box_scaled->bottom-1 < 0)
	goto nodraw;
    }

  if (box_scaled->right > mplayx)
    {
      x_offset = (box_scaled->right) - mplayx;
      box_scaled->right = mplayx;

      if (spr[h].size == 100)
	box_real->right -= x_offset;
      else
	box_real->right -= (x_offset * 100) / spr[h].size;

      if (box_scaled->left+1 > mplayx)
	goto nodraw;
    }

  if (box_scaled->bottom > mplayy)
    {
      y_offset = (box_scaled->bottom) - mplayy;
      box_scaled->bottom = mplayy;

      if (spr[h].size == 100)
	box_real->bottom -= y_offset;
      else
	box_real->bottom -= (y_offset * 100) / spr[h].size;

      if (box_scaled->top+1 > mplayy)
	goto nodraw;
    }

 do_draw:
    return(/*true*/1);

 nodraw:
    return(/*false*/0);
}

void draw_sprite_game(IOGfxSurface *GFX_lpdest, int h)
{
  if (spr[h].brain == 8)
    return; // text
  if (spr[h].nodraw == 1)
    return; // invisible

  rect box_crap,box_real;

  if (get_box(h, &box_crap, &box_real, false))
    {
      /* Generic scaling */
      /* Not perfectly accurate yet: move a 200% sprite to the border
	 of the screen to it is clipped: it's scaled size will slighly
	 vary. Maybe we need to clip the source zone before scaling
	 it.. */
      // error checking for invalid rectangle
      if (box_crap.left >= box_crap.right || box_crap.top >= box_crap.bottom)
	return;
      
      SDL_Rect src, dst;
      int retval = 0;
      src.x = box_real.left;
      src.y = box_real.top;
      src.w = box_real.right - box_real.left;
      src.h = box_real.bottom - box_real.top;
      dst.x = box_crap.left;
      dst.y = box_crap.top;
      dst.w = box_crap.right - box_crap.left;
      dst.h = box_crap.bottom - box_crap.top;

      retval = GFX_lpdest->blitStretch(GFX_k[getpic(h)].k, &src, &dst);
      
      if (retval < 0) {
	log_error("Could not draw sprite %d: %s", getpic(h), SDL_GetError());
	/* If we failed, then maybe the sprite was actually loaded
	   yet, let's try now */
	if (spr[h].pseq != 0)
	  check_seq_status(spr[h].pseq);
    }
  }
}





/**
 * Screen transition with scrolling effect.
 * Returns 0 when transition is finished.
 */
bool transition(int fps_final)
{
	SDL_Rect src, dst;
	
	//we need to do our fancy screen transition
	int dumb = fps_final * 2;
	
	move_counter += dumb;
	
	
	if (move_screen == 4) {
		if (move_counter > 598)
			move_counter = 598;
		
		src.x = 0;
		src.y = 0;
		src.w = 600 - move_counter;
		src.h = 400;
		dst.x = 20 + move_counter;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp1, &src, &dst);
		
		src.x = 600 - move_counter;
		src.y = 0;
		src.w = move_counter;
		src.h = 400;
		dst.x = 20;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp2, &src, &dst);
		
		if (move_counter >= 595) {
			transition_in_progress = 0;
			move_screen = 0;
			move_counter = 0;
			//draw_map();
			return false;
		}
		
		return true;
    }
	
  
	if (move_screen == 6) {
		if (move_counter > 598)
			move_counter = 598;
		
		src.x = move_counter;
		src.y = 0;
		src.w = 600 - move_counter;
		src.h = 400;
		dst.x = 20;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp1, &src, &dst);
		
		src.x = 0;
		src.y = 0;
		src.w = move_counter;
		src.h = 400;
		dst.x = 620 - move_counter;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp2, &src, &dst);
		
		if (move_counter >= 595) {
			transition_in_progress = 0;
			move_screen = 0;
			move_counter = 0;
			//draw_map();
			return false;
		}
		
		return true;
    }
	
	
	if (move_screen == 8) {
		if (move_counter > 398)
			move_counter = 398;
		
		src.x = 0;
		src.y = 0;
		src.w = 600;
		src.h = 400 - move_counter;
		dst.x = 20;
		dst.y = move_counter;
		IOGFX_backbuffer->blit(IOGFX_tmp1, &src, &dst);
		
		src.x = 0;
		src.y = 400 - move_counter;
		src.w = 600;
		src.h = move_counter;
		dst.x = 20;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp2, &src, &dst);
		
		if (move_counter >= 398) {
			transition_in_progress = 0;
			move_screen = 0;
			move_counter = 0;
			//draw_map();
			return false;
		}
		
		return true;
    }
	
	
	if (move_screen == 2) {
		if (move_counter > 398)
			move_counter = 398;
		
		src.x = 0;
		src.y = move_counter;
		src.w = 600;
		src.h = 400 - move_counter;
		dst.x = 20;
		dst.y = 0;
		IOGFX_backbuffer->blit(IOGFX_tmp1, &src, &dst);
		
		src.x = 0;
		src.y = 0;
		src.w = 600;
		src.h = move_counter;
		dst.x = 20;
		dst.y = 400 - move_counter;
		IOGFX_backbuffer->blit(IOGFX_tmp2, &src, &dst);
		
		if (move_counter >= 398) {
			transition_in_progress = 0;
			move_screen = 0;
			move_counter = 0;
			//draw_map();
			return false;
		}
		
		return true;
    }
	
	return false;
}

/* Capture the current's backbuffer game zone for screen transition */
void grab_trick(int dir) {
	SDL_Rect src, dst;
	src.x = playl;
	src.y = 0;
	src.w = 620 - playl;
	src.h = 400;
	dst.x = dst.y = 0;
	IOGFX_tmp1->blit(IOGFX_backbuffer, &src, &dst);
	
	move_screen = dir;
	move_counter = 0;
}
