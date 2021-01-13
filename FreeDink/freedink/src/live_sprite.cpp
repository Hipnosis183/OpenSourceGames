/**
 * Displayed sprite

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

#include "live_sprite.h"
#include "live_sprites_manager.h"
#include "gfx_sprites.h"
#include "dinkini.h" /* check_seq_status */
#include "log.h"

/**
 * Get the current graphic (current sequence/current frame) for sprite
 * 'sprite_no'
 */
int getpic(int sprite_no)
{
  if (spr[sprite_no].pseq == 0)
    return 0;
  
  if (spr[sprite_no].pseq >= MAX_SEQUENCES)
    {
      log_error("Sequence %d?  But max is %d!", spr[sprite_no].pseq, MAX_SEQUENCES);
      return 0;
    }

  return seq[spr[sprite_no].pseq].frame[spr[sprite_no].pframe];
}

/**
 * Checks for all seq's used by the (base) commands
 * Game only
 */
void check_sprite_status_full(int sprite_no)
{
  //is sprite in memory?
  check_seq_status(spr[sprite_no].pseq);

  if (spr[sprite_no].base_walk > -1)
    check_base(spr[sprite_no].base_walk);
}


/* Editor only */
void check_sprite_status(int h)
{
        //is sprite in memory?
        if (spr[h].pseq > 0)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);
                if (seq[spr[h].pseq].frame[1] == 0)
                {
		  if (seq[spr[h].pseq].is_active)
		    figure_out(seq[spr[h].pseq].ini);
		  else
		    log_error("Error: sprite %d references non-existent sequence %d",
			      spr[h].sp_index, spr[h].pseq);
                }
                else
                {
                        //it's been loaded before.. is it lost or still there?
                }
        }
}

void live_sprite_set_kill_start(int h, Uint32 thisTickCount) {
	//inc delay, used by "skip" by all sprites
	/* 				box_crap = k[getpic(h)].box; */
	if (spr[h].kill_ttl > 0 && spr[h].kill_start == 0) {
		spr[h].kill_start = thisTickCount;
	}
}

bool live_sprite_is_expired(int h, Uint32 thisTickCount) {
	return spr[h].kill_ttl > 0 && (spr[h].kill_start + spr[h].kill_ttl < thisTickCount);
}

/**
 * Play live sprite sequence.
 * Set *p_isHitting to true if hit a 'special' frame
 */
void live_sprite_animate(int h, Uint32 thisTickCount, bool* p_isHitting) {
	if (spr[h].reverse) {
		//reverse instructions
		if (spr[h].seq > 0) {
			if (spr[h].frame < 1) {
				// new anim
				spr[h].pseq = spr[h].seq;
				spr[h].pframe = seq[spr[h].seq].len;
				spr[h].frame = seq[spr[h].seq].len;
				if (spr[h].frame_delay != 0)
					spr[h].delay = (thisTickCount+ spr[h].frame_delay);
				else
					spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[seq[spr[h].seq].len]);
			} else {
				// not new anim
				//is it time?
				if (thisTickCount > spr[h].delay) {
					spr[h].frame--;
					if (spr[h].frame_delay != 0)
						spr[h].delay = (thisTickCount + spr[h].frame_delay);
					else
						spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);

					spr[h].pseq = spr[h].seq;
					spr[h].pframe = spr[h].frame;

					if (seq[spr[h].seq].frame[spr[h].frame]  < 2) {
						spr[h].pseq = spr[h].seq;
						spr[h].pframe = spr[h].frame+1;

						spr[h].frame = 0;
						spr[h].seq_orig = spr[h].seq;
						spr[h].seq = 0;
						spr[h].nocontrol = /*false*/0;
					}

					if (spr[h].seq > 0 && seq[spr[h].seq].special[spr[h].frame] == 1)
						*p_isHitting = true;
				}
			}
		}
	} else {
		if (spr[h].seq > 0 && spr[h].picfreeze == 0) {
			if (spr[h].frame < 1) {
				// new anim
				spr[h].pseq = spr[h].seq;
				spr[h].pframe = 1;
				spr[h].frame = 1;
				if (spr[h].frame_delay != 0)
					spr[h].delay = thisTickCount + spr[h].frame_delay;
				else
					spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[1]);
			} else {
				// not new anim
				//is it time?
				if (thisTickCount > spr[h].delay) {
					spr[h].frame++;
					if (spr[h].frame_delay != 0)
						spr[h].delay = thisTickCount + spr[h].frame_delay;
					else
						spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);

					spr[h].pseq = spr[h].seq;
					spr[h].pframe = spr[h].frame;

					if (seq[spr[h].seq].frame[spr[h].frame] == -1) {
						spr[h].frame = 1;
						spr[h].pseq = spr[h].seq;
						spr[h].pframe = spr[h].frame;
						if (spr[h].frame_delay != 0)
							spr[h].delay = thisTickCount + spr[h].frame_delay;
						else
							spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
					}

					if (seq[spr[h].seq].frame[spr[h].frame] < 1) {
						spr[h].pseq = spr[h].seq;
						spr[h].pframe = spr[h].frame-1;

						spr[h].frame = 0;
						spr[h].seq_orig = spr[h].seq;
						spr[h].seq = 0;
						spr[h].nocontrol = /*false*/0;
					}

					if (spr[h].seq > 0 && seq[spr[h].seq].special[spr[h].frame] == 1)
						*p_isHitting = true;
				}
			}
		}
	}
}
