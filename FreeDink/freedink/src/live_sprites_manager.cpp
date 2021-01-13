/**
 * Live sprites manager

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2008, 2009, 2010, 2014, 2015  Sylvain Beucler

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
#include "gfx_sprites.h"
#include "dinkc.h"

struct sp spr[MAX_SPRITES_AT_ONCE]; //max sprite control systems at once
int last_sprite_created;

void live_sprites_manager_init() {
  memset(&spr, 0, sizeof(spr));
  last_sprite_created = 0;
}

bool lsm_isValidSprite(int sprite) {
	return (sprite > 0 && sprite < MAX_SPRITES_AT_ONCE);
}

void lsm_remove_sprite(int sprite) {
	if (!lsm_isValidSprite(sprite))
		return;

	spr[sprite].active = false;
	if (spr[sprite].custom != NULL) {
		delete spr[sprite].custom;
		spr[sprite].custom = NULL;
	}
	if (spr[sprite].text_cache != NULL) {
		delete spr[sprite].text_cache;
		spr[sprite].text_cache = NULL;
	}

	// if cached text sprite -> free
}

int add_sprite(int x1, int y, int brain,int pseq, int pframe )
{
  int x;
    for (x = 1; x < MAX_SPRITES_AT_ONCE; x++)
        {
                if (!spr[x].active)
                {
                        memset(&spr[x], 0, sizeof(spr[x]));

                        spr[x].active = true;
                        spr[x].x = x1;
                        spr[x].y = y;
                        spr[x].my = 0;
                        spr[x].mx = 0;
                        spr[x].speed = 1;
                        spr[x].brain = brain;
                        spr[x].frame = 0;
                        spr[x].pseq = pseq;
                        spr[x].pframe = pframe;
                        spr[x].seq = 0;
                        if (x > last_sprite_created)
                                last_sprite_created = x;
                        spr[x].timing = 33;
                        spr[x].wait = 0;
                        spr[x].lpx[0] = 0;
                        spr[x].lpy[0] = 0;
                        spr[x].moveman = 0;
                        spr[x].size = 100;
                        spr[x].que = 0;
                        spr[x].strength = 0;
                        spr[x].damage = 0;
                        spr[x].defense = 0;
                        spr[x].hard = 1;

						if (spr[x].custom == NULL)
						  spr[x].custom = new std::map<std::string, int>;
						else
						  spr[x].custom->clear();

                        return(x);
                }

        }

        return(0);
}

/* Like add_sprit_dumb, except:
 * speed      :   1 ->  0
 * size       : 100 -> size
 * timer      :  33 ->  0
 * que        :   0 ->  ?
 * seq_orig   :   ? ->  0
 * base_hit   :   ? -> -1
 * base_walk  :   ? -> -1
 * base_die   :   ? -> -1
 * base_idle  :   ? -> -1
 * base_attack:   ? -> -1
 * last_sound :   ? ->  0
 * hard       :   ? ->  1
 * althard    :   ? ->  0
 * sp_index   :   ? ->  0
 * nocontrol  :   ? ->  0
 * idle       :   ? ->  0
 * hard       :   1 ->  ?
 * alt        :   ? -> {0,0,0,0}
 */
int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size )
{
  int x;
    for (x = 1; x < MAX_SPRITES_AT_ONCE; x++)
        {
                if (!spr[x].active)
                {
                        memset(&spr[x], 0, sizeof(spr[x]));

                        //Msg("Making sprite %d.",x);
                        spr[x].active = true;
                        spr[x].x = x1;
                        spr[x].y = y;
                        spr[x].my = 0;
                        spr[x].mx = 0;
                        spr[x].speed = 0;
                        spr[x].brain = brain;
                        spr[x].frame = 0;
                        spr[x].pseq = pseq;
                        spr[x].pframe = pframe;
                        spr[x].size = size;
                        spr[x].seq = 0;
                        if (x > last_sprite_created)
                                last_sprite_created = x;

                        spr[x].timing = 0;
                        spr[x].wait = 0;
                        spr[x].lpx[0] = 0;
                        spr[x].lpy[0] = 0;
                        spr[x].moveman = 0;
                        spr[x].seq_orig = 0;


            spr[x].base_hit = -1;
                        spr[x].base_walk = -1;
                        spr[x].base_die = -1;
                        spr[x].base_idle = -1;
                        spr[x].base_attack = -1;
                        spr[x].last_sound = 0;
                        spr[x].hard = 1;

                        rect_set(&spr[x].alt, 0,0,0,0);
                        spr[x].althard = 0;
                        spr[x].sp_index = 0;
                        spr[x].nocontrol = 0;
                        spr[x].idle = 0;
                        spr[x].strength = 0;
                        spr[x].damage = 0;
                        spr[x].defense = 0;

						if (spr[x].custom == NULL)
						  spr[x].custom = new std::map<std::string, int>;
						else
						  spr[x].custom->clear();

                        return(x);
                }

        }

        return(0);
}


void random_blood(int mx, int my, int sprite)
        {
                int myseq;
                /* v1.08 introduces custom blood sequence, as well as
                   a slightly different default (select blood in range
                   187-189 included, instead of 187-188 included) */
                int randy;
                if (spr[sprite].bloodseq > 0 && spr[sprite].bloodnum > 0)
                  {
                    myseq = spr[sprite].bloodseq;
                    randy = spr[sprite].bloodnum;
                  }
                else
                  {
                    myseq = 187;
					// TODO: dropped dversion dependency for testsuite
					// purposes, see how to reintroduce it cleanly
					// if (dversion >= 108)
					// 	randy = 3;
                    // else
					// 	randy = 2;
					randy = 3;
                  }
                myseq += (rand () % randy);
                
                int crap2 = add_sprite(mx,my,5,myseq,1);
                /* TODO: add_sprite might return 0, and the following
                   would trash spr[0] - cf. bugs.debian.org/688934 */
                spr[crap2].speed = 0;
                spr[crap2].base_walk = -1;
                spr[crap2].nohit = 1;
                spr[crap2].seq = myseq;
                if (sprite > 0)
                        spr[crap2].que = spr[sprite].y+1;

        }

static bool kill_highest_nonlive_sprite() {
	int highest_sprite = 0;
	bool setlast = true;
	
	for (int k = 1; k < MAX_SPRITES_AT_ONCE; k++) {
		if (spr[k].active) {
			if (spr[k].live)
				setlast = false;
			else
				highest_sprite = k;
		}
	}
	
	if (highest_sprite > 1) {
		lsm_remove_sprite(highest_sprite);
		if (setlast)
			last_sprite_created = highest_sprite - 1;
		return true;
	}
	
	//we didn't kill any sprites, only 1 remains
	return false;
}

/**
 * Mark all sprites as inactive except for 'live' sprites nor spr#1
 */
void lsm_kill_all_nonlive_sprites() {
	while (kill_highest_nonlive_sprite());
}

void get_last_sprite() {
	for (int i = MAX_SPRITES_AT_ONCE - 1; i > 2; i--) {
		if (spr[i].active) {
			last_sprite_created = i;
			return;
		}
    }
}


void kill_text_owned_by(int sprite)
{
  int i;
  for (i = 1; i < MAX_SPRITES_AT_ONCE; i++)
    {
      if (spr[i].active && spr[i].brain == 8 && spr[i].text_owner == sprite)
    	  lsm_remove_sprite(i);
    }
}




int does_sprite_have_text(int sprite) {
	for (int i = 1; i < MAX_SPRITES_AT_ONCE; i++)
		if (spr[i].active && spr[i].text_owner == sprite && spr[i].brain == 8)
			return i;
	return 0;
}


/**
 * Is 'sprite' currently talking?
 * Returns 1 if a text sprite is owned by sprite number 'sprite'.
 */
/*bool*/int text_owned_by(int sprite)
{
  int i = 1;
  for (; i < MAX_SPRITES_AT_ONCE; i++)
    if (spr[i].active && spr[i].brain == 8 && spr[i].text_owner == sprite)
      return /*true*/1;
  return /*false*/0;
}


/**
 * Find an editor sprite in active sprites
 */
int find_sprite(int editor_sprite)
{
  int k;
  for (k = 1; k <= last_sprite_created; k++)
    if (spr[k].sp_index == editor_sprite)
      return k;
  return 0;
}

