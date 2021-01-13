/**
 * Game status bar

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

#include "status.h"
#define SEQ_EXPLEVEL_NUMS 442

#include "SDL.h"
#include <string.h> /* strlen */
#include "IOGfxPrimitives.h" /* gfx_blit_nocolorkey */
#include "gfx.h"
#include "gfx_sprites.h" /* GFX_k */
#include "dinkini.h" /* check_seq_status */
#include "live_sprites_manager.h" /* spr */
#include "sfx.h"
#include "game_choice.h" /* game_choice.active */
#include "game_engine.h" /* show_inventory */

/* Status animations */
int flife;
static int fstrength, fdefense, fmagic,
	last_magic_draw,
	/* flife, */ flifemax,
	fgold,
	fexp, fraise;

static void draw_num(int mseq, char* nums, int mx, int my) {
	int length = 0;
	int rnum = 0;

	for (unsigned int i = 0; i < strlen(nums); i++) {
		if (nums[i] == '0') rnum = 10;
		else if (nums[i] == '1') rnum = 1;
		else if (nums[i] == '2') rnum = 2;
		else if (nums[i] == '3') rnum = 3;
		else if (nums[i] == '4') rnum = 4;
		else if (nums[i] == '5') rnum = 5;
		else if (nums[i] == '6') rnum = 6;
		else if (nums[i] == '7') rnum = 7;
		else if (nums[i] == '8') rnum = 8;
		else if (nums[i] == '9') rnum = 9;
		else if (nums[i] == '/') rnum = 11;
		if (rnum == 11 || mseq == SEQ_EXPLEVEL_NUMS) {
			SDL_Rect dst = {mx+length, my};
			IOGFX_background->blit(GFX_k[seq[mseq].frame[rnum]].k, NULL, &dst);
		} else {
			SDL_Rect dst = {mx+length, my};
			IOGFX_background->blitNoColorKey(GFX_k[seq[mseq].frame[rnum]].k, NULL, &dst);
		}

		length += k[seq[mseq].frame[rnum]].box.right;
	}
}


void draw_exp()
{
  char buffer[5+1];
  char final[5+1+5+1];
  
  strcpy(final, "");
  snprintf(buffer, sizeof(buffer), "%05d", fexp);
  strcat(final, buffer);
  strcat(final,"/");

  snprintf(buffer, sizeof(buffer), "%05d", fraise);
  strcat(final, buffer);
  draw_num(181, final, 404, 459);
}

void draw_strength()
{
  char final[3+1];
  snprintf(final, sizeof(final), "%03d", fstrength);
  draw_num(182, final, 81, 415);
}

void draw_defense()
{
  char final[3+1];
  snprintf(final, sizeof(final), "%03d", fdefense);
  draw_num(183, final, 81, 437);
}

void draw_magic()
{
  char final[3+1];
  snprintf(final, sizeof(final), "%03d", fmagic);
  draw_num(184, final, 81, 459);
}

void draw_level()
{
  char final[2+1];
  snprintf(final, sizeof(final), "%d", *plevel);
  
  if (strlen(final) == 1)
    draw_num(SEQ_EXPLEVEL_NUMS, final, 528, 456);
  else
    draw_num(SEQ_EXPLEVEL_NUMS, final, 523, 456);

}

void draw_gold()
{
  char final[6+1];
  snprintf(final, sizeof(final),
		   (fgold < 100000) ? "%05d" : "%06d",
		   fgold);
  draw_num(185, final, 298, 457);
}


void draw_bar(int life, int seqman)
{
  int cur = 0;
  int curx = 284;
  int cury = 412;
  int rnum = 3;
  int curx_start = curx;

  rect box;
  while(1)
    {
      cur++;
      if (cur > life)
	{
	  cur--;
	  int rem = (cur) - (cur / 10) * 10;
	  if (rem != 0)
	    {
	      rect_copy(&box, &k[seq[seqman].frame[rnum]].box);
	      box.right = (box.right * ((rem) * 10)/100);
	      //woah, there is part of a bar remaining.  Lets do it.
		SDL_Rect src, dst;
		src.x = 0; src.y = 0;
		src.w = k[seq[seqman].frame[rnum]].box.right * (rem * 10) / 100;
		src.h = k[seq[seqman].frame[rnum]].box.bottom;
		dst.x = curx; dst.y = cury;
		IOGFX_background->blitNoColorKey(GFX_k[seq[seqman].frame[rnum]].k, &src, &dst);
	    }
	  //are we done?
	  return;
	}

      rnum = 2;
      if (cur < 11) rnum = 1;
      if (cur == *plifemax) rnum = 3;

      if ((cur / 10) * 10 == cur)
	{
	    SDL_Rect dst;
	    dst.x = curx;
	    dst.y = cury;
	    IOGFX_background->blitNoColorKey(GFX_k[seq[seqman].frame[rnum]].k, NULL, &dst);

	  curx += k[seq[seqman].frame[rnum]].box.right;
	  if (cur == 110)
	    {cury += k[seq[seqman].frame[rnum]].box.bottom+5;
	      curx = curx_start;

	    }

	  if (cur == 220) return;
	}
    }
}


void draw_health( void )
{
        flifemax = *plifemax;
        draw_bar(flifemax, 190);
        flife = *plife;
        draw_bar(flife, 451);
}

void draw_icons()
{
  if (*pcur_weapon >= 1 && *pcur_weapon <= NB_ITEMS && play.item[*pcur_weapon - 1].active)
    {
      check_seq_status(play.item[*pcur_weapon - 1].seq);
      SDL_Rect dst = {557, 413};
      IOGFX_background->blit(GFX_k[seq[play.item[*pcur_weapon - 1].seq].frame[play.item[*pcur_weapon - 1].frame]].k,
        NULL, &dst);
    }

  if (*pcur_magic >= 1 && *pcur_magic <= NB_MITEMS && play.mitem[*pcur_magic - 1].active)
    {
      check_seq_status(play.mitem[*pcur_magic - 1].seq);
      SDL_Rect dst = {153, 413};
      IOGFX_background->blit(GFX_k[seq[play.mitem[*pcur_magic - 1].seq].frame[play.mitem[*pcur_magic - 1].frame]].k,
        NULL, &dst);
    }
}


/** draw_virtical, draw_hor, draw_virt2, draw_hor2: used to draw the
    magic jauge (in that order) (dinkvar.cpp:draw_mlevel() only) **/

void draw_mgauge_ver1(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.bottom;
  cut = (full * percent) / 100;
  myrect.bottom = cut;

  my += (full - cut);

  SDL_Rect src, dst;
  src.x = src.y = 0;
  src.w = k[seq[mseq].frame[mframe]].box.right;
  src.h = k[seq[mseq].frame[mframe]].box.bottom * percent / 100;
  dst.x = mx;
  dst.y = my;
  IOGFX_background->blitNoColorKey(GFX_k[seq[mseq].frame[mframe]].k, &src, &dst);
}

void draw_mgauge_ver2(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.bottom;
  cut = (full * percent) / 100;
  myrect.bottom = cut;

  SDL_Rect src, dst;
  src.x = src.y = 0;
  src.w = k[seq[mseq].frame[mframe]].box.right;
  src.h = k[seq[mseq].frame[mframe]].box.bottom * percent / 100;
  dst.x = mx; dst.y = my;
  IOGFX_background->blitNoColorKey(GFX_k[seq[mseq].frame[mframe]].k, &src, &dst);
}

void draw_mgauge_hor1(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.right;
  cut = (full * percent) / 100;
  full = cut;
  myrect.right = full;

  SDL_Rect src, dst;
  src.x = src.y = 0;
  src.w = k[seq[mseq].frame[mframe]].box.right * percent / 100;
  src.h = k[seq[mseq].frame[mframe]].box.bottom;
  dst.x = mx; dst.y = my;
  IOGFX_background->blitNoColorKey(GFX_k[seq[mseq].frame[mframe]].k, &src, &dst);
}

void draw_mgauge_hor2(int percent, int mx, int my, int mseq, int mframe)
{
  int cut;
  if (percent > 25) percent = 25;
  percent = (percent * 4);
  rect myrect;
  rect_copy(&myrect, &k[seq[mseq].frame[mframe]].box);
  int full = myrect.right;
  cut = (full * percent) / 100;

  myrect.right = cut;
  mx += (full - cut);

  SDL_Rect src, dst;
  src.x = src.y = 0;
  src.w = k[seq[mseq].frame[mframe]].box.right * percent / 100;
  src.h = k[seq[mseq].frame[mframe]].box.bottom;
  dst.x = mx;
  dst.y = my;
  IOGFX_background->blitNoColorKey(GFX_k[seq[mseq].frame[mframe]].k, &src, &dst);
}

void draw_mgauge(int percent)
{
  //if (*pmagic_level < 1) return;

  int mseq = 180;
  int bary = 6;
  int barx = 7;

  if (percent > 0) draw_mgauge_ver1(percent, 149, 411, mseq, bary);
  percent -= 25;
  if (percent > 0) draw_mgauge_hor1(percent, 149, 409, mseq, barx);
  percent -= 25;
  if (percent > 0) draw_mgauge_ver2(percent, 215, 411, mseq, bary);
  percent -= 25;
  if (percent > 0) draw_mgauge_hor2(percent, 149, 466, mseq, barx);
}


/**
 * Sync status with player stats, and redraw full status
 */
void draw_status_all(void)
{
  {
    SDL_Rect src = {0, 0, 640, 80}, dst = {0, 400};
    IOGFX_background->blitNoColorKey(GFX_k[seq[180].frame[3]].k, &src, &dst);
  }
  {
    SDL_Rect src = {0, 0, 20, 400}, dst1 = {0, 0}, dst2 = {620, 0};
    IOGFX_background->blitNoColorKey(GFX_k[seq[180].frame[1]].k, &src, &dst1);
    IOGFX_background->blitNoColorKey(GFX_k[seq[180].frame[2]].k, &src, &dst2);
  }

  fraise = next_raise();
  if (*pexp < fraise)
    fexp = *pexp;
  else
    fexp = fraise - 1;
  fstrength = *pstrength;
  fmagic = *pmagic;
  fgold = *pgold;
  fdefense = *pdefense;
  last_magic_draw = 0;
  draw_exp();
  draw_health();
  draw_strength();
  draw_defense();
  draw_magic();
  draw_gold();
  draw_level();
  draw_icons();
  if (*pmagic_cost > 0 && *pmagic_level > 0)
    draw_mgauge(*pmagic_level * 100 / *pmagic_cost);
}

/**
 * Draw screen lateral bar, with screenlock skin
 */
void drawscreenlock() {
	IOGFX_backbuffer->blitNoColorKey(GFX_k[seq[423].frame[9]].k, NULL, NULL);
    SDL_Rect dst = {620, 0};
    IOGFX_backbuffer->blitNoColorKey(GFX_k[seq[423].frame[10]].k, NULL, &dst);
}

/**
 * Update player stats, and render status bar only as needed
 */
void update_status_all() {
	bool drawexp = false;
	int next = next_raise();

	if (next != fraise) {
		fraise += next / 40;

		if (fraise > next)
			fraise = next;

		drawexp = true;
		SoundPlayEffect(13,15050, 0,0 ,0);
	}

	if (*pexp != fexp
	    && ((game_choice.active == 0 && show_inventory == 0 && spr[1].freeze == 0)
	    || fexp + 10 < fraise)) {
		//update screen experience
		fexp += 10;
		//make noise here

		if (fexp > *pexp)
			fexp = *pexp;

		drawexp = true;
		SoundPlayEffect(13,29050, 0,0 ,0);

		if (fexp >= fraise) {
			*pexp -= next;
			fexp = 0;

			int script = load_script("lraise", 1);
			if (locate(script, "raise")) run_script(script);
		}
	}

	if (drawexp) {
		draw_exp();
	}

	if (flifemax != *plifemax || flife != *plife) {
		if (flifemax < *plifemax) flifemax++;
		if (flifemax > *plifemax) flifemax--;
		if (flife > *plife) flife--;
		if (flife < *plife) flife++;
		if (flife > *plife) flife--;
		if (flife < *plife) flife++;
		draw_bar(flifemax, 190);
		draw_bar(flife, 451);
	}

	if (fstrength != *pstrength) {
		if (fstrength < *pstrength) fstrength++;
		if (fstrength > *pstrength) fstrength--;
		SoundPlayEffect( 22,22050, 0,0 ,0);
		draw_strength();
	}

	if (fdefense != *pdefense) {
		if (fdefense < *pdefense) fdefense++;
		if (fdefense > *pdefense) fdefense--;
		SoundPlayEffect( 22,22050, 0,0 ,0);
		draw_defense();
	}

	if (fmagic != *pmagic) {
		if (fmagic < *pmagic) fmagic++;
		if (fmagic > *pmagic) fmagic--;
		SoundPlayEffect( 22,22050, 0,0 ,0);
		draw_magic();
	}

	if (fgold != *pgold) {
		if (fgold < *pgold) {
			fgold += 20;
			if (fgold > *pgold) fgold = *pgold;
		}

		if (fgold > *pgold) {
			fgold -= 20;
			if (fgold < *pgold) fgold = *pgold;
		}
		SoundPlayEffect( 14,22050, 0,0 ,0);
		draw_gold();
	}

	if (*pmagic_level < *pmagic_cost) {
		if (show_inventory == 0)
			*pmagic_level += *pmagic;
		if (*pmagic_level > *pmagic_cost) *pmagic_level = *pmagic_cost;
	}

	if (*pmagic_cost > 0) if (*pmagic_level > 0) {
		int mnum = *pmagic_level * 100 / *pmagic_cost;
		if (mnum != last_magic_draw) {
			draw_mgauge(mnum);
			last_magic_draw = mnum;
		}
	}

	spr[1].strength = fstrength;
	spr[1].defense = fdefense;

	if (flife < 1) {
		int script = load_script("dinfo", 1000);
		if (locate(script, "die"))
			run_script(script);
	}
}
