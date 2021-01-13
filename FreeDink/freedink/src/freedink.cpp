/**
 * FreeDink game-specific code

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2003, 2004, 2005, 2007, 2008, 2009, 2010, 2012, 2014, 2015  Sylvain Beucler

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

#include "freedink.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fastfile.h"

#include "IOGfxPrimitives.h"
#include "IOGfxDisplay.h"
#include "ImageLoader.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_palette.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "bgm.h"
#include "sfx.h"
#include "update_frame.h"
#include "io_util.h"
#include "paths.h"
#include "input.h"
#include "log.h"

#include "game_engine.h"
#include "live_sprites_manager.h"
#include "live_screen.h"
#include "DMod.h"
#include "meminfo.h"
#include "dinkc_console.h"
#include "game_choice.h"
#include "text.h"


Uint32 but_timer = 0;

/* Fadedown/fadeup counter */
static int process_count = 0;

struct show_bmp showb;


/**
 * Check if the sprite can pass or should be blocked
 *
 * Returns 0 = can pass, <>0 = should be blocked
 */
int check_if_move_is_legal(int u)
{
  if ((dversion >= 108) /* move_nohard is active for all movements, not just active moves */
      || (/* dversion == 107 && */ spr[u].move_active))
    if (spr[u].move_nohard == 1)
      return(0);

  if (u == 1 && in_this_base(spr[1].seq, dink_base_push))
    return(0);
	
  int hardness = 0;
	if (spr[u].moveman > 0)
	{
	  int i;
	  for (i = 1; i <= spr[u].moveman; i++)
	    {
	      hardness = get_hard(spr[u].lpx[i]-20, spr[u].lpy[i], screenlock);
	      if (hardness == 2 && spr[u].flying) 
		{
		  spr[u].moveman = 0;
		  if (dversion >= 108)
		    return 0;
		  else
		    return 2;
		}
	      if (hardness > 0)
		{
		  spr[u].x = spr[u].lpx[i-1];
		  spr[u].y = spr[u].lpy[i-1];
		  spr[u].moveman = 0;			
		  
		  if (push_active)
		    {
		      if (u == 1 && hardness != 2 && play.push_active == false)
			{
			  if ((spr[u].dir == 2) | (spr[u].dir == 4) | (spr[u].dir == 6) | (spr[u].dir == 8))
			    {
			      //he  (dink)  is definatly pushing on something
			      play.push_active = true;
			      play.push_dir = spr[u].dir;
			      play.push_timer = thisTickCount;
			    }
			}
		      else
			{
			  if (play.push_dir != spr[1].dir) play.push_active = false;
			}
		    }
		  return(hardness);
		}
	    }
	}
	
	if (u == 1)  play.push_active = false;
	return(0);
}

int did_player_cross_screen()
{
  if (walk_off_screen == 1)
    return 0;
	
  int ret = 0;
  // DO MATH TO SEE IF THEY HAVE CROSSED THE SCREEN, IF SO LOAD NEW ONE
  if (spr[1].x < playl)
    {
      if ((*pplayer_map-1) >= 1 && g_dmod.map.loc[*pplayer_map-1] > 0 && screenlock == 0)
	{
	  //move one map to the left
	  update_screen_time();
	  grab_trick(4);
	  *pplayer_map -= 1;
	  game_load_screen(g_dmod.map.loc[*pplayer_map]);
	  if (g_dmod.map.indoor[*pplayer_map] == 0)
	    play.last_map = *pplayer_map;
	  
	  draw_screen_game();
	  // compatibility: update Dink position *after* screen change
	  spr[1].x = 619;
	  spr[1].y = spr[1].lpy[0];
	  ret = 1;
	}
      else
	{
	  spr[1].x = playl;
	}
    }
  
  else if (spr[1].x > 619)
    {
      if ((*pplayer_map+1) <= 24*32 && g_dmod.map.loc[*pplayer_map+1] > 0 && screenlock == 0)
	{
	  //move one map to the right
	  update_screen_time();
	  grab_trick(6);
	  *pplayer_map += 1;
	  game_load_screen(g_dmod.map.loc[*pplayer_map]);
	  if (g_dmod.map.indoor[*pplayer_map] == 0)
	    play.last_map = *pplayer_map;
	  
	  draw_screen_game();
	  // compatibility: update Dink position *after* screen change
	  spr[1].x = playl;
	  spr[1].y = spr[1].lpy[0];
	  ret = 1;
	}
      else
	{
	  spr[1].x = 619;
	}
    }
  
  else if (spr[1].y < 0)
    {
      if ((*pplayer_map-32) >= 1 && g_dmod.map.loc[*pplayer_map-32] > 0 && screenlock == 0)
	{
	  //move one map up
	  update_screen_time();
	  grab_trick(8);
	  *pplayer_map -= 32;
	  game_load_screen(g_dmod.map.loc[*pplayer_map]);
	  if (g_dmod.map.indoor[*pplayer_map] == 0)
	    play.last_map = *pplayer_map;
	  
	  // compatibility: update Dink X position *before* screen change
	  // (shouldn't matter when though, since it's an Y motion)
	  spr[1].x = spr[1].lpx[0];
	  draw_screen_game();
	  // compatibility: update Dink Y position *after* screen change
	  spr[1].y = 399;
	  ret = 1;
	}
      else
	{
	  spr[1].y = 0;
	}
    }
  
  
  else if (spr[1].y > 399)
    {
      if ((*pplayer_map+32) <= 24*32 && g_dmod.map.loc[*pplayer_map+32] > 0 && screenlock == 0)
	{
	  //move one map down
	  update_screen_time();
	  grab_trick(2);
	  *pplayer_map += 32;
	  game_load_screen(g_dmod.map.loc[*pplayer_map]);
	  if (g_dmod.map.indoor[*pplayer_map] == 0)
	    play.last_map = *pplayer_map;
	  
	  draw_screen_game();
	  // compatibility: update Dink position *after* screen change
	  spr[1].y = 0;
	  spr[1].x = spr[1].lpx[0];
	  ret = 1;
	}
      else
	{
	  spr[1].y = 399;
	}
    }
  return ret;
}

/*bool*/int run_through_tag_list_talk(int h)
{
	rect box;
	int amount, amounty;
	int i;

	for (i = 1; i <= last_sprite_created; i++)
	{
		
		if (spr[i].active) if (i != h) if (spr[i].brain != 8)
		{
			
			
			rect_copy(&box, &k[getpic(i)].hardbox);
			rect_offset(&box, spr[i].x, spr[i].y);
			
			rect_inflate(&box, 10,10);
			
			amount = 50;		
			amounty = 35;
			if (spr[h].dir == 6)
			{
				box.left -= amount;
			}
			
			if (spr[h].dir == 4)
			{
				box.right += amount;
			}
			
			
			if (spr[h].dir == 2)
			{
				box.top -= amounty;
			}
			
			if (spr[h].dir == 8)
			{
				box.bottom += amounty;
			}
			
			//		draw_box(box, 33);
			
			if (inside_box(spr[h].x, spr[h].y, box))
			{	
				//Msg("Talking to sprite %d", i);
				if (spr[i].script > 0)
				{
					//if (  (spr[i].brain == 0) | (spr[i].brain == 5) | (spr[i].brain == 6) | (spr[i].brain == 7))
					//Msg("trying to find TALK in script %d", spr[i].script);
					if (locate(spr[i].script, "TALK")) 
					{
						kill_returning_stuff(spr[i].script);
						
						run_script(spr[i].script);
						return(/*true*/1);	
					}
					
					
				}
				
			}
			
			
		}
		
		
	}
	
	
	return(/*false*/0);
}


/**
 * Trigger a warp (teleport)
 * block: the warp editor sprite
 */
int special_block(int block)
{
  if (cur_ed_screen.sprite[block].is_warp == 1)
    {
      //they touched a warp
      if (cur_ed_screen.sprite[block].sound == 0)
        SoundPlayEffect(7, 12000, 0, 0, 0);
      else
        SoundPlayEffect(cur_ed_screen.sprite[block].sound, 22050, 0, 0, 0);
      
      if (cur_ed_screen.sprite[block].parm_seq != 0)
        {
          // we'll also play an animation here
          int sprite = find_sprite(block);
          if (sprite > 0)
            {
              spr[sprite].seq = cur_ed_screen.sprite[block].parm_seq;
              process_warp = block;
            }
          return 1;
        }
      process_warp = block;
      return 1; // redraw screen with fade
    }
  return 0;
}
	
/* fade_down() - fade to black */
void CyclePalette()
{
  if (!truecolor)
    {
      SDL_Color palette[256];
      int kk;

      gfx_palette_get_phys(palette);

      for (kk = 1; kk < 256; kk++) /* skipping index 0 because it's
				      already (and always) black ;) */
	{
	  if (dversion >= 108)
	    {
	      /* Use time-based rather than absolute increments;
		 avoids incomplete fades on slow computers */
	      /* dt / 2 == dt * 256 / 512 == complete fade in 512ms */
	      int lValue = (thisTickCount - lastTickCount) / 2;
	      if (palette[kk].b != 0)
		{
		  if (palette[kk].b > lValue)
		    palette[kk].b -= lValue;
		  else
		    palette[kk].b = 0;
		}
	      if (palette[kk].g != 0)
		{
		  if (palette[kk].g > lValue)
		    palette[kk].g -= lValue;
		  else
		    palette[kk].g = 0;
		}
	      if (palette[kk].r != 0)
		{
		  if (palette[kk].r > lValue)
		    palette[kk].r -= lValue;
		  else
		    palette[kk].r = 0;
		}
	    }
	  else
	    {
	      if (palette[kk].b != 0)
		{
		  if (palette[kk].b > 10)
		    palette[kk].b -= 10;
		  else
		    palette[kk].b--;
		}
	      if (palette[kk].g != 0)
		{
		  if (palette[kk].g > 10)
		    palette[kk].g -= 10;
		  else
		    palette[kk].g--;
		}
	      if (palette[kk].r != 0)
		{
		  if (palette[kk].r > 10)
		    palette[kk].r -= 10;
		  else
		    palette[kk].r--;
		}
	    }
	}
  
      gfx_palette_set_phys(palette);
    }
  else
    {
      /* truecolor */
      if (truecolor_fade_lasttick == 0)
	{
	  truecolor_fade_lasttick = game_GetTicks();
	  //truecolor_fade_brightness -= 256*.3;
	}
      else
	{
	  int delta = game_GetTicks() - truecolor_fade_lasttick;
	  /* Complete fade in 400ms */
	  truecolor_fade_lasttick = game_GetTicks();
	  g_display->brightness -= delta * 256 / 400.0;
	}
      if (g_display->brightness <= 0)
    	  g_display->brightness = 0;
      
    }

  if (process_downcycle) 
    {
      if (thisTickCount > cycle_clock)
	{
	  process_downcycle = /*false*/0;
	  truecolor_fade_lasttick = 0;
				
	  if (cycle_script != 0)
	    {
	      int junk = cycle_script;
	      cycle_script = 0;	
	      run_script(junk);
	    }
	}
    }
}
	
/* fade_up() */	
void up_cycle(void)
{
  int donethistime = 1;

  if (!truecolor)
    {
      SDL_Color palette[256];
      int kk;

      gfx_palette_get_phys(palette);

      for (kk = 1; kk < 256; kk++)
	{
	  int tmp = -1;

	  tmp = palette[kk].b; // 'int' to avoid 'char' overflow
	  if (tmp != GFX_ref_pal[kk].b)
	    {
	      donethistime = 0;
	      if (tmp > 246)
		tmp++;
	      else
		tmp += 10;
	    }
	  if (tmp > GFX_ref_pal[kk].b)
	    tmp = GFX_ref_pal[kk].b;
	  palette[kk].b = tmp;
      
	  tmp = palette[kk].g; // 'int' to avoid 'char' overflow
	  if (tmp != GFX_ref_pal[kk].g)
	    {
	      donethistime = 0;
	      if (tmp > 246)
		tmp++;
	      else
		tmp += 10;
	    }
	  if (tmp > GFX_ref_pal[kk].g)
	    tmp = GFX_ref_pal[kk].g;
	  palette[kk].g = tmp;
      
	  tmp = palette[kk].r; // 'int' to avoid 'char' overflow
	  if (tmp != GFX_ref_pal[kk].r)
	    {
	      donethistime = 0;
	      if (tmp > 246)
		tmp++;
	      else
		tmp += 10;
	    }
	  if (tmp > GFX_ref_pal[kk].r)
	    tmp = GFX_ref_pal[kk].r;
	  palette[kk].r = tmp;
	}
  
      gfx_palette_set_phys(palette);
    }
  else
    {
      /* truecolor */
      donethistime = 0;
      if (truecolor_fade_lasttick == 0)
	{
	  truecolor_fade_lasttick = game_GetTicks();
	  //truecolor_fade_brightness += 256*.3;
	}
      else
	{
	  int delta = game_GetTicks() - truecolor_fade_lasttick;
	  /* Complete fade in 400ms */
	  truecolor_fade_lasttick = game_GetTicks();
	  g_display->brightness += delta * 256 / 400.0;
	}
      if (g_display->brightness >= 256)
	{
    	  g_display->brightness = 256;
	  donethistime = 1;
	}
    }
		
  if (process_upcycle)
    if (donethistime == 1)
      {
	process_upcycle = 0;
	truecolor_fade_lasttick = 0;
	
	if (cycle_script != 0)
	  {
	    int junk = cycle_script;
	    cycle_script = 0;	
	    run_script(junk);
	  }
      }
}


        int hurt_thing(int h, int damage, int special)
        {
                //lets hurt this sprite but good
                if (damage < 1) return(0);
                int num = damage - spr[h].defense;

                //      Msg("num is %d.. defense was %d.of sprite %d", num, spr[h].defense, h);
                if (num < 1) num = 0;

                if (num == 0)
                {
                        if ((rand() % 2)+1 == 1) num = 1;
                }

                spr[h].damage += num;
                return(num);
                //draw blood here
        }

/**
 * Check which sprites are affected by an attack from 'h', the
 * attacker.
 */
	void run_through_tag_list(int h, int strength)
	{
		rect box;
		int amount, amounty;
		int i;

		for (i = 1; i <= last_sprite_created; i++)
		{
			if (spr[i].active) if (i != h) if
				(! ( (spr[i].nohit == 1) && (spr[i].script == 0)) )
			{
				
				rect_copy(&box, &k[getpic(i)].hardbox);
				rect_offset(&box, spr[i].x, spr[i].y);
				
				//InflateRect(&box, 10,10);
				
				box.right += 5;
				box.left -= 5;
				box.top -= 5;
				box.bottom += 10;
				if (spr[h].range == 0)		
					amount = 28; else amount = spr[h].range;
				
				if (spr[h].range == 0)		
					
					amounty = 36; else amounty = (spr[h].range + (spr[h].range / 6));
				
				int range_amount = spr[h].range / 8;
				
				if (spr[h].dir == 6)
				{
					box.top -= 10;
					box.bottom += 10;
					if (spr[h].range != 0) box.top -= range_amount;
					if (spr[h].range != 0) box.bottom += range_amount;
					
					
					box.left -= amount;
				}
				
				if (spr[h].dir == 4)
				{
					box.right += amount;
					
					box.top -= 10;
					box.bottom += 10;
					if (spr[h].range != 0) box.top -= range_amount;
					if (spr[h].range != 0) box.bottom += range_amount;
					
				}
				
				
				if (spr[h].dir == 2)
				{
					box.right += 10;
					box.left -= 10;
					box.top -= amounty;
					
					if (spr[h].range != 0) box.right += range_amount;
					if (spr[h].range != 0) box.left -= range_amount;
					
				}
				
				if (spr[h].dir == 8)
				{
					box.right += 10;
					box.left -= 10;
					box.bottom += amounty;
					
					if (spr[h].range != 0) box.right += range_amount;
					if (spr[h].range != 0) box.right -= range_amount;
					
				}
				
				if (debug_mode) {
					SDL_Rect r = {box.left, box.top, box.right-box.left, box.bottom-box.top};
					IOGFX_backbuffer->fillRect(&r, 222, 173, 255);
				}
				
				if (inside_box(spr[h].x, spr[h].y, box))
				{	
					// addition for fixing missle_source problems
					if (dversion >= 108)
					  *pmissle_source = h;

					if (spr[i].nohit == 1)
					{
						if (spr[i].script > 0)
						{
							//if (  (spr[i].brain == 0) | (spr[i].brain == 5) | (spr[i].brain == 6) | (spr[i].brain == 7))
							*penemy_sprite = h;
							
							if (  (spr[i].base_attack != -1) || (spr[i].touch_damage > 0))
								spr[i].target = h;   
							
							if (locate(spr[i].script, "HIT"))
							{
								kill_returning_stuff(spr[i].script);
								run_script(spr[i].script);
							}
							
							
						}
						
						
					} else
					{
						//hit this personb/thing
						if (spr[h].attack_hit_sound == 0)
						{
							SoundPlayEffect( 9,22050, 0 ,0,0);
						} else
						{
							SoundPlayEffect( spr[h].attack_hit_sound,spr[h].attack_hit_sound_speed, 0 ,0,0);
						}
						if (  (spr[i].base_attack != -1) || (spr[i].touch_damage > 0))
							spr[i].target = h;   
						if (spr[h].strength == 0)
						{
							
						} else
						{
							if (  (spr[i].hitpoints > 0) || (i == 1) )
							{
								
								spr[i].last_hit = h; 
								if ( hurt_thing(i, (spr[h].strength / 2) + ((rand() % ((spr[h].strength+1) / 2))+1), 0) > 0)
									random_blood(spr[i].x, spr[i].y-40, i);
							}
							
						}
						if (spr[i].script > 0)
						{
							//if (  (spr[i].brain == 0) | (spr[i].brain == 5) | (spr[i].brain == 6) | (spr[i].brain == 7))
							spr[i].last_hit = h;    
							*penemy_sprite = h;
							if (  (spr[i].base_attack != -1) || (spr[i].touch_damage > 0))
								spr[i].target = h;   
							
							if (locate(spr[i].script, "HIT"))
							{
								kill_returning_stuff(spr[i].script);
								run_script(spr[i].script);
							}
							
						}
						
					}
					
				}
				
		}
		
	}
	
}



void run_through_touch_damage_list(int h)
{
	rect box;
	int i;

	for (i = 1; i <= last_sprite_created; i++)
	{
		if (spr[i].active) if (i != h) if
			((spr[i].touch_damage != 0) )
		{
			
			if (spr[i].touch_damage != -1) if (spr[h].notouch) return;
			rect_copy(&box, &k[getpic(i)].hardbox);
			rect_offset(&box, spr[i].x, spr[i].y);
			
			//InflateRect(&box, 10,10);
			
			box.right += 2;
			box.left -= 2;
			box.top -= 2;
			box.bottom += 2;
			if (debug_mode) {
				SDL_Rect r = {box.left, box.top, box.right-box.left, box.bottom-box.top};
				IOGFX_backbuffer->fillRect(&r, 222, 173, 255);
			}
			
			
			if (inside_box(spr[h].x, spr[h].y, box))
			{	
				
				if ((spr[i].touch_damage == -1) && (spr[i].script != 0))
				{
					if (locate(spr[i].script, "TOUCH")) run_script(spr[i].script);
				} else
				{
					if (spr[i].touch_damage == -1)
					{
						log_error("Sprites touch damage is set to -1 but there is no script set!");
					} else
					{
						//lets hurt the guy
						
						spr[h].notouch = /*true*/1;
						spr[h].notouch_timer = thisTickCount+400;
						spr[h].last_hit = i;
						if (spr[i].script != 0)
							if (locate(spr[i].script, "TOUCH")) run_script(spr[i].script);
							if (hurt_thing(h, spr[i].touch_damage, 0) > 0)
								random_blood(spr[h].x, spr[h].y-40, h);
							
							
					}
					
				}
				
				
				
			}
			
			
		}
		
	}
	
}




void process_warp_man(void)
{
  int sprite = find_sprite(process_warp);
  
  /* warp sprite doesn't exist (e.g. merged background sprite), or
     warp anim is finished */
  /* Cf. http://www.dinknetwork.com/forum.cgi?MID=168476 */
  if (sprite == 0 || spr[sprite].seq == 0)
    {
      process_count++;
      CyclePalette();
      /* Wait 5 CyclePalette before blanking the screen and
	 warp. Truecolor more: the fade algorithm is a bit different
	 and requires a few more cycles to get the same effect; unlike
	 v1.08, we don't wait for a full fadedown, which is long and
	 can be not very smooth on old computers. */
      if ((!truecolor && process_count > 5)
	  || (truecolor && g_display->brightness <= 180))
	{
	  IOGFX_backbuffer->fillRect(NULL, 0,0,0);
	  g_display->flipStretch(IOGFX_backbuffer);
	  
	  process_count = 0;
	  int block = process_warp;
	  update_screen_time();
	  spr[1].x = cur_ed_screen.sprite[block].warp_x;
	  spr[1].y = cur_ed_screen.sprite[block].warp_y;
	  *pplayer_map = cur_ed_screen.sprite[block].warp_map;

	  // update map indicator
	  if (g_dmod.map.indoor[cur_ed_screen.sprite[block].warp_map] == 0)
	    play.last_map = cur_ed_screen.sprite[block].warp_map;
	  
	  game_load_screen(g_dmod.map.loc[cur_ed_screen.sprite[block].warp_map]);
	  draw_screen_game();
	  
	  process_upcycle = 1;
	  process_warp = 0;
	}
    }
  else /* warp anim didn't finish yet */
    {
      process_count = 0;		
    }
}

void process_show_bmp( void )
{
  IOGFX_backbuffer->blit(IOGFX_tmp1, NULL, NULL);
  
  if (showb.showdot)
    {
      //let's display a nice dot to mark where they are on the map
      int x = play.last_map - 1;
      int mseq = 165;
      
      showb.picframe++;
      if (showb.picframe > seq[mseq].len) showb.picframe = 1;
      int mframe = showb.picframe;
      
      SDL_Rect dst;
      // convert map# to a (x,y) position on a FreeDinkedit minimap
      dst.x = (x % 32) * 20;
      dst.y = (x / 32) * 20;
      IOGFX_backbuffer->blit(GFX_k[seq[mseq].frame[mframe]].k, NULL, &dst);
    }
  
  
  if ((sjoy.button[ACTION_ATTACK])
      || (sjoy.button[ACTION_TALK])
      || (sjoy.button[ACTION_MAGIC])
      || (sjoy.button[ACTION_INVENTORY])
      || (sjoy.button[ACTION_MENU])
      || (sjoy.button[ACTION_MAP]))
    {
      showb.active = /*false*/0;
      if (showb.script != 0)
	run_script(showb.script);
      showb.stime = thisTickCount+2000;
      but_timer = thisTickCount + 200;
      
      int sprite = say_text_xy("", 1, 440, 0);								
      spr[sprite].noclip = 1;
      
      
      // Return to canonical game palette
      gfx_palette_set_phys(GFX_ref_pal);
      // The main flip_it() will be called, skip it - lpDDSBack is
      // not matching the palette anymore, it needs to be redrawn
      // first.
      abort_this_flip = /*true*/1;
    }
}
