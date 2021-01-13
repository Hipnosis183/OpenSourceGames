/**
 * Game-specific processing

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

#include <stdlib.h>  /* srand */
#include <time.h>  /* time */
#include <string.h>  /* memset */
#include "game_engine.h"
#include "live_sprites_manager.h"
#include "live_screen.h"
#include "DMod.h" /* g_dmod.map */
#include "live_screen.h" /* screen_hitmap */
#include "dinkini.h" /* check_seq_status, hmap, cur_ed_screen */
#include "freedink.h"  /* add_time_to_saved_game */
#include "input.h"
#include "log.h"
#include "sfx.h"
#include "bgm.h"
#include "IOGfxPrimitives.h"
#include "IOGfxDisplay.h"
#include "gfx.h"
#include "gfx_sprites.h"
#include "hardness_tiles.h"
#include "savegame.h"
#include "meminfo.h"
#include "dinkc.h"
#include "dinkc_bindings.h"
#include "game_state.h"
#include "status.h"


int dversion = 108;

int walk_off_screen = /*false*/0;

int show_inventory = 0; // display inventory?
int warp_editor_sprite = -500;

int stop_entire_game;

int screenlock = 0;

struct wait_for_button wait4b;



char* dversion_string;

int last_saved_game = 0;
char save_game_info[200] = "Level &level";

time_t time_start;


int smooth_follow = 0;



/** Fadedown / fadeup **/
/* Tell the engine that we're warping */
/* (quick fadedown + black + fadeup) */
int process_warp = 0;
/* Tell the engine that we're fading down */
/* or fading up */
int process_downcycle = 0;
int process_upcycle = 0;
/* When the fadedown must finish: */
unsigned long cycle_clock = 0;
/* What script to resume after fade: */
int cycle_script = 0;

/* Engine is currently executing a screen's main() - display "Please
   Wait" when loading graphics and postpone other scripts */
/*bool*/int screen_main_is_running = /*false*/0;
static int please_wait_toggle_frame = 7;


static int high_speed = 0;
struct player_info play;

struct attackinfo_struct bow;

/* Number of ms since an arbitrarily fixed point */
Uint32 thisTickCount;
Uint32 lastTickCount;
int fps_final = 0;
int base_timing = 0;

int keep_mouse = 0;
int mode = 0;

void game_restart()
{
  int mainscript;
  lsm_kill_all_nonlive_sprites();
  kill_repeat_sounds_all();
  kill_all_scripts_for_real();
  set_mode(0);
  screenlock = 0;

  /* Reset all game state */
  memset(&play, 0, sizeof(play));

  memset(&screen_hitmap, 0, sizeof(screen_hitmap));
  input_set_default_buttons();

  mainscript = load_script("main", 0);

  locate(mainscript, "main");
  run_script(mainscript);
  //lets attach our vars to the scripts
  attach();
}


void game_compute_speed() {
	//Use to test at 30 fps
	//Sleep(66);
	
    lastTickCount = thisTickCount;
	thisTickCount = game_GetTicks();
	
	fps_final = thisTickCount - lastTickCount;
	
	//redink1 changed to 12-12 from 10-15... maybe work better on faster computers?
	if (fps_final < 12)
		fps_final = 12;
	if (fps_final > 68)
		fps_final = 68;


	base_timing = fps_final / 3;
	if (base_timing < 4)
		base_timing = 4;


	int new_dinkspeed;
	
	//redink1 added these changes to set Dink's speed correctly,
	//even on fast machines.
	//new_dinkspeed = (fps_average / dinkspeed) -  (fps_average / 8);
	if (dinkspeed <= 0)
		new_dinkspeed = 0;
	else if (dinkspeed == 1)
		new_dinkspeed = 12;
	else if (dinkspeed == 2)
		new_dinkspeed = 6;
	else if (dinkspeed == 3)
		new_dinkspeed = 3;
	else
		new_dinkspeed = 1;
	
	//redink1... weird bug, only do this for normal mode, as it
	//breaks mouse support
	//if (mode == 2)
	new_dinkspeed *= (base_timing / 4);
	
	spr[1].speed = new_dinkspeed;
}

/**
 * Fake SDL_GetTicks if the player is in high-speed mode.  Make sure
 * you call it once per frame.
 */
Uint32 game_GetTicks()
{
  static Uint32 last_sdl_ticks = 0;
  static Uint32 high_ticks = 0;

  Uint32 cur_sdl_ticks = SDL_GetTicks();
  /* Work-around incorrect initial value */
  if (last_sdl_ticks == 0)
    last_sdl_ticks = cur_sdl_ticks - 1;
    
  /* If high speed, then count every tick as triple (so add 2x) */
  if (high_speed)
    {
      high_ticks += 2 * (cur_sdl_ticks - last_sdl_ticks);
    }
  
  last_sdl_ticks = cur_sdl_ticks;
  return cur_sdl_ticks + high_ticks;
}

void game_set_high_speed()
{
  if (high_speed == 0)
    {
      SDL_setFramerate(&framerate_manager, 3*FPS);
      high_speed = 1;
    }
}

void game_set_normal_speed()
{
  if (high_speed == 1)
    {
      SDL_setFramerate(&framerate_manager, FPS);
      high_speed = 0;
    }
}


int next_raise(void)
{
  int crap = *plevel;
  int num = ((100 * crap) * crap);
  
  if (num > 99999) num = 99999;
  return(num);
}


/**
 * Add experience - no "did the player really kill this enemy?"
 * checks
 */
void add_exp_force(int num, int source_sprite)
{
  if (num > 0)
    {
      //add experience
      *pexp += num;

      int crap2 = add_sprite(spr[source_sprite].x, spr[source_sprite].y, 8, 0, 0);
      spr[crap2].y -= k[seq[spr[source_sprite].pseq].frame[spr[source_sprite].pframe]].yoffset;
      spr[crap2].x -= k[seq[spr[source_sprite].pseq].frame[spr[source_sprite].pframe]].xoffset;
      spr[crap2].y -= k[seq[spr[source_sprite].pseq].frame[spr[source_sprite].pframe]].box.bottom / 3;
      spr[crap2].x += k[seq[spr[source_sprite].pseq].frame[spr[source_sprite].pframe]].box.right / 5;
      spr[crap2].y -= 30;
      spr[crap2].speed = 1;
      spr[crap2].hard = 1;
      spr[crap2].brain_parm = 5000;
      spr[crap2].my = -1;
      spr[crap2].kill_ttl = 1000;
      spr[crap2].dir = 8;
      spr[crap2].damage = num;
      
      if (*pexp > 99999)
	*pexp = 99999;
    }
}

void add_exp(int num, int killed_sprite)
{
  if (spr[killed_sprite].last_hit != 1)
    return;
  
  add_exp_force(num, killed_sprite);
}


/**
 * Resurrect sprites that were temporarily disabled
 * (editor_type(6/7/8))
 */
void fix_dead_sprites()
{
  int i;

  for (i = 1; i < 100; i++)
    {
      int type = play.spmap[*pplayer_map].type[i];

      // Resurrect sprites after 5mn
      if (type == 6)
	{
	  if  ((thisTickCount > (play.spmap[*pplayer_map].last_time + 300000))
	       || (thisTickCount + 400000 < play.spmap[*pplayer_map].last_time + 300000))
	    {
	      //this sprite can come back online now
	      play.spmap[*pplayer_map].type[i] = 0;
	    }
	}

      // Resurrect sprites after 3mn
      if (type == 7)
	{
	  if (thisTickCount > (play.spmap[*pplayer_map].last_time + 180000))
	    {
	      //this sprite can come back online now
	      play.spmap[*pplayer_map].type[i] = 0;
	    }
	}

      // Resurrect sprites after 1mn
      if (type == 8)
	{
	  if (thisTickCount > (play.spmap[*pplayer_map].last_time + 60000))
	    {
	      //this sprite can come back online now
	      play.spmap[*pplayer_map].type[i] = 0;
	    }
	}
    }
}

/**
 * When entering a screen, play the appropriate MIDI
 */
void check_midi(void)
{
  // TODO: use a better constant (like max_file_path)
  char midi_filename[20];

  if ((!midi_active) || (g_dmod.map.music[*pplayer_map] == 0))
    return;

  /* There is music information associated with this screen */
  if (g_dmod.map.music[*pplayer_map] != -1) {
    if (g_dmod.map.music[*pplayer_map] > 1000)
      /* Try to play a CD track (unsupported) - fall back to MIDI */
      {
	sprintf(midi_filename, "%d.mid", g_dmod.map.music[*pplayer_map] - 1000);
	PlayMidi(midi_filename);
      }
    else
      {
	/* Just play the specified MIDI */
	sprintf(midi_filename, "%d.mid", g_dmod.map.music[*pplayer_map]);
	PlayMidi(midi_filename);
      }
  }
}

/**
 * Load 1 screen from map.dat, which contains all 768 game screens
 */
int game_load_screen(int mapdat_num)
{
  if (g_dmod.map.ts_loc_mem[mapdat_num] != NULL) {
    memcpy(&cur_ed_screen, g_dmod.map.ts_loc_mem[mapdat_num], sizeof(struct editor_screen));
  } else if (load_screen_to(g_dmod.map.map_dat.c_str(), mapdat_num, &cur_ed_screen) < 0) {
    return -1;
  }
  
  spr[1].move_active = 0;
  if (dversion >= 108)
    spr[1].move_nohard = 0;
  spr[1].freeze = 0;
  screenlock = 0;

  fill_whole_hard();

  fix_dead_sprites();
  check_midi();

  return 0;
}


/**
 * Remember last time we entered this screen (so we can disable
 * sprites for some minutes, e.g. monsters)
 */
void update_screen_time()
{
  //Msg("Cur time is %d", play.spmap[*pplayer_map].last_time);
  //Msg("Map is %d..", *pplayer_map);
  play.spmap[*pplayer_map].last_time = thisTickCount;
  //Msg("Time was saved as %d", play.spmap[*pplayer_map].last_time);
}


void update_play_changes( void )
{
  int j;
        for (j = 1; j < 100; j++)
        {
                if (cur_ed_screen.sprite[j].active)
                        if (play.spmap[*pplayer_map].type[j] != 0)
                        {
                                //lets make some changes, player has extra info
                                if (play.spmap[*pplayer_map].type[j] == 1)
                                {
                                        cur_ed_screen.sprite[j].active = 0;

                                }

                                if (play.spmap[*pplayer_map].type[j] == 2)
                                {
                                        cur_ed_screen.sprite[j].type = 1;
                    cur_ed_screen.sprite[j].hard = 1;
                                }
                                if (play.spmap[*pplayer_map].type[j] == 3)
                                {

                                        //              Msg("Changing sprite %d", j);
                                        cur_ed_screen.sprite[j].type = 0;
                                        cur_ed_screen.sprite[j].hard = 1;

                                }

                                if (play.spmap[*pplayer_map].type[j] == 4)
                                {
                                        cur_ed_screen.sprite[j].type = 1;
                    cur_ed_screen.sprite[j].hard = 0;
                                }

                                if (play.spmap[*pplayer_map].type[j] == 5)
                                {
                                        cur_ed_screen.sprite[j].type = 0;
                    cur_ed_screen.sprite[j].hard = 0;
                                }

                                if (play.spmap[*pplayer_map].type[j] == 6)
                                {
                                        cur_ed_screen.sprite[j].active = 0;

                                }
                                if (play.spmap[*pplayer_map].type[j] == 7)
                                {
                                        cur_ed_screen.sprite[j].active = 0;

                                }
                                if (play.spmap[*pplayer_map].type[j] == 8)
                                {
                                        cur_ed_screen.sprite[j].active = 0;

                                }

                                cur_ed_screen.sprite[j].seq = play.spmap[*pplayer_map].seq[j];
                                cur_ed_screen.sprite[j].frame = play.spmap[*pplayer_map].frame[j];
                                strcpy(cur_ed_screen.sprite[j].script, "");


                        }


        }
}


/**
 * Load screen sprites: draw background sprites, ordered by queue and
 * configure the others (sounds, scripts, etc.).
 *
 * Also cf. game_place_sprites_background(...) and editor's
 * place_sprites(...).
 */
void game_place_sprites()
{
  update_play_changes();
  
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_editor_sprites(rank);
  
  int r1 = 0;  
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (cur_ed_screen.sprite[j].active == 1
	  && (cur_ed_screen.sprite[j].vision == 0 || cur_ed_screen.sprite[j].vision == *pvision))
	{
	  check_seq_status(cur_ed_screen.sprite[j].seq);
	  
	  //we have instructions to make a sprite
	  if (cur_ed_screen.sprite[j].type == 0 || cur_ed_screen.sprite[j].type == 2)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);

	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[j].alt);
	      
	      check_sprite_status_full(sprite);

	      if (cur_ed_screen.sprite[j].type == 0)
		draw_sprite_game(IOGFX_background, sprite);
	      
	      if (spr[sprite].hard == 0)
		{
		  /*if (cur_ed_screen.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else */
		  add_hardness(sprite, 100 + j);
		}
	      lsm_remove_sprite(sprite);
	    }

	  if (cur_ed_screen.sprite[j].type == 1)
	    {
	      //make it a living sprite
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);
	      
	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      
	      //assign addition parms to the new sprite
	      spr[sprite].sp_index = j;
	      
	      spr[sprite].brain = cur_ed_screen.sprite[j].brain;
	      spr[sprite].speed = cur_ed_screen.sprite[j].speed;
	      spr[sprite].base_walk = cur_ed_screen.sprite[j].base_walk;
	      spr[sprite].base_idle = cur_ed_screen.sprite[j].base_idle;
	      spr[sprite].base_attack = cur_ed_screen.sprite[j].base_attack;
	      spr[sprite].base_hit = cur_ed_screen.sprite[j].base_hit;
	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      spr[sprite].timing = cur_ed_screen.sprite[j].timing;
	      spr[sprite].que = cur_ed_screen.sprite[j].que;
	      
	      
	      spr[sprite].sp_index = j;
	      
	      rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[j].alt);
	      
	      spr[sprite].base_die = cur_ed_screen.sprite[j].base_die;
	      spr[sprite].strength = cur_ed_screen.sprite[j].strength;
	      spr[sprite].defense = cur_ed_screen.sprite[j].defense;
	      spr[sprite].gold = cur_ed_screen.sprite[j].gold;
	      spr[sprite].exp = cur_ed_screen.sprite[j].exp;
	      spr[sprite].nohit = cur_ed_screen.sprite[j].nohit;
	      spr[sprite].touch_damage = cur_ed_screen.sprite[j].touch_damage;
	      spr[sprite].hitpoints = cur_ed_screen.sprite[j].hitpoints;
	      spr[sprite].sound = cur_ed_screen.sprite[j].sound;
	      check_sprite_status_full(sprite);
	      if (cur_ed_screen.sprite[j].is_warp == 0 && spr[sprite].sound != 0)
		{
		  //make looping sound
		  log_debug("making sound with sprite %d..", sprite);
		  SoundPlayEffect( spr[sprite].sound,22050, 0,sprite, 1);
		}
	      if (spr[sprite].brain == 3)
		{
		  // Duck
		  check_seq_status(21);
		  check_seq_status(23);
		  check_seq_status(24);
		  check_seq_status(26);
		  check_seq_status(27);
		  check_seq_status(29);
		  // Headless duck
		  check_seq_status(111);
		  check_seq_status(113);
		  check_seq_status(117);
		  check_seq_status(119);
		  // Duck head
		  check_seq_status(121);
		  check_seq_status(123);
		  check_seq_status(127);
		  check_seq_status(129);
		}
	      
	      if (spr[sprite].hard == 0)
		{
		  /*  if (cur_ed_screen.sprite[j].is_warp == 0)
			add_hardness(sprite, 1);
		      else */
		  add_hardness(sprite, 100+j);
		}
	      
	      //does it need a script loaded?
	      if (strlen(cur_ed_screen.sprite[j].script) > 1)
		{
		  spr[sprite].script = load_script(cur_ed_screen.sprite[j].script, sprite);
		}
	    }
	  //Msg("I just made sprite %d because rank[%d] told me to..",sprite,j);
					       }
    }
}

/**
 * Draw background sprites and background (not looking at
 * non-background sprites), ordered by queue.
 * 
 * Also cf. game_place_sprites(...) and editor's place_sprites(...).
 */
void game_place_sprites_background()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_editor_sprites(rank);

  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];
      
      if (cur_ed_screen.sprite[j].active == 1
	  && (cur_ed_screen.sprite[j].vision == 0 || cur_ed_screen.sprite[j].vision == *pvision))
	{
	  if (cur_ed_screen.sprite[j].type == 0)
	    {
	      //we have instructions to make a sprite
	      check_seq_status(cur_ed_screen.sprite[j].seq);
	      
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);

	      check_sprite_status_full(sprite);
	      draw_sprite_game(IOGFX_background, sprite);
	      lsm_remove_sprite(sprite);
	    }
	}
    }
}

void fill_back_sprites()
{
  int rank[MAX_SPRITES_EDITOR];
  screen_rank_editor_sprites(rank);

  int r1 = 0;
  for (; r1 < MAX_SPRITES_EDITOR && rank[r1] > 0; r1++)
    {
      //Msg("Ok, rank[%d] is %d.",oo,rank[oo]);
      int j = rank[r1];

      if (cur_ed_screen.sprite[j].active == 1
	  && (cur_ed_screen.sprite[j].vision == 0 || cur_ed_screen.sprite[j].vision == *pvision))
	{



	  if (cur_ed_screen.sprite[j].type != 1 && cur_ed_screen.sprite[j].hard == 0)
	    {
	      //make it part of the background (much faster)
	      int sprite = add_sprite_dumb(cur_ed_screen.sprite[j].x,cur_ed_screen.sprite[j].y, 0,
					   cur_ed_screen.sprite[j].seq,cur_ed_screen.sprite[j].frame,
					   cur_ed_screen.sprite[j].size);

	      spr[sprite].hard = cur_ed_screen.sprite[j].hard;
	      spr[sprite].sp_index = j;
	      rect_copy(&spr[sprite].alt , &cur_ed_screen.sprite[j].alt);

	      check_sprite_status_full(sprite);




	      if (spr[sprite].hard == 0)
		{
		  /*if (cur_ed_screen.sprite[j].is_warp == 0)
		    add_hardness(sprite, 1); else */
		  add_hardness(sprite,100+j);
		}
	      lsm_remove_sprite(sprite);
	    }
	}
    }
}


/**
 * Run main() for all active sprites on screen
 */
void game_screen_init_scripts()
{
  int k = 1;
  for (; k < MAX_SCRIPTS; k++)
    {
      if (rinfo[k] != NULL && rinfo[k]->sprite != 0
	  /* don't go out of bounds in spr[300], e.g. when sprite == 1000: */
	  && rinfo[k]->sprite < MAX_SPRITES_AT_ONCE
	  && spr[rinfo[k]->sprite].active)
	{
	  if (locate(k, "main"))
	    {
	      log_debug("Screendraw: running main of script %s..", rinfo[k]->name);
	      run_script(k);
	    }
	}
    }
}

/**
 * Activates a screen: draw it and run scripts
 */
void draw_screen_game(void)
{
  *pvision = 0;
                
  lsm_kill_all_nonlive_sprites();
  kill_repeat_sounds();
  kill_all_scripts();

  gfx_tiles_draw_screen(g_dmod.bgTilesets.slots, cur_ed_screen.t);

  int script_id = 0;
  if (cur_ed_screen.ts_script_id > 0)
    script_id = cur_ed_screen.ts_script_id;
  else if (strlen(cur_ed_screen.script) > 1)
    script_id = load_script(cur_ed_screen.script, 0);
                        
  if (script_id > 0)
    {
      locate(script_id, "main");
      screen_main_is_running = /*true*/1;
      run_script(script_id);
      screen_main_is_running = /*false*/0;
    }

  // lets add the sprites hardness to the real hardness, adding it's
  // own uniqueness to our collective.
  game_place_sprites();
  
  thisTickCount = game_GetTicks();
                
  // Run active sprites' scripts
  game_screen_init_scripts();

  // Display some memory stats after loading a screen
  if (debug_mode) {
    meminfo_log_mallinfo();
    gfx_log_meminfo();
    sfx_log_meminfo();
  }
}
        
/* It's used at: freedink.cpp:restoreAll(), DinkC's draw_background(),
   stop_entire_game(). What's the difference with draw_screen_game()?? */
void draw_screen_game_background(void)
{
  gfx_tiles_draw_screen(g_dmod.bgTilesets.slots, cur_ed_screen.t);
  game_place_sprites_background();
}


/**
 * Display a flashing "Please Wait" anim directly on the screen, just
 * before switching to a screen that requires loading new graphics
 * from the disk.
 */
static void draw_wait() {
	if (screen_main_is_running) {
		if (seq[423].frame[please_wait_toggle_frame] != 0) {
			SDL_Rect dst = { 232, 0, -1, -1 };
			IOGFX_backbuffer->blit(GFX_k[seq[423].frame[please_wait_toggle_frame]].k, NULL, &dst);
			g_display->flipStretch(IOGFX_backbuffer);
		}
		if (please_wait_toggle_frame == 7)
			please_wait_toggle_frame = 8;
		else
			please_wait_toggle_frame = 7;
	}
}


void set_mode(int new_mode) {
  mode = new_mode;
  if (mode == 3 && !keep_mouse) {
    SDL_SetWindowGrab(g_display->window, SDL_FALSE);
  } else {
    /* Jail window cursor (Alt+Tab still works) */
    SDL_SetWindowGrab(g_display->window, SDL_TRUE);
  }
}


/**
 * Prepare mode 1 (mouse interaction / title screen / START.c)
 */
void apply_mode0() {
	memset(&spr[1], 0, sizeof(spr[1]));
	spr[1].speed = 3;
	spr[1].timing = 0;
	spr[1].brain = 1;
	spr[1].hard = 1;
	spr[1].pseq = 2;
	spr[1].pframe = 1;
	spr[1].seq = 2;
	spr[1].dir = 2;
	spr[1].damage = 0;
	spr[1].strength = 10;
	spr[1].defense = 0;
	spr[1].skip = 0;
	rect_set(&spr[1].alt, 0, 0, 0, 0);
	spr[1].base_idle = 10;
	spr[1].base_walk = -1;
	spr[1].size = 100;
	spr[1].base_hit = 100;
	spr[1].active = true;
	spr[1].custom = new std::map<std::string, int>;

	SDL_WarpMouseInWindow(g_display->window, spr[1].x, spr[1].y);

	int version_text = add_sprite(0, 450, 8, 0, 0);
	spr[version_text].hard = 1;
	spr[version_text].noclip = 1;
	strcpy(spr[version_text].text, dversion_string);
	spr[version_text].damage = -1;
	spr[version_text].text_owner = 1000;

	int scr = load_script("START", 1000);
	if (locate(scr, "MAIN") == /*false*/0) {
		log_error("Can't locate MAIN in script START!");
	}

	run_script(scr);
}

void apply_mode() {
	if (mode == 0) {
		apply_mode0();
		set_mode(1);
	}

	if (mode == 2) {
		set_mode(3);
		game_load_screen(g_dmod.map.loc[*pplayer_map]);
		draw_screen_game();
		flife = *plife;
	}
}


void set_keep_mouse(int on) {
  keep_mouse = on;
  if (!on)
    SDL_SetWindowGrab(g_display->window, SDL_FALSE);
}


void game_init(int version)
{
  dversion = version;
  /* Clean the game state structure - done by C++ but not
     automatically done by C, and this causes errors. TODO: fix the
     errors properly instead of using this dirty trick. */
  memset(&play, 0, sizeof(play));
  live_sprites_manager_init();

  gfx_sprites_loading_listener = draw_wait;
  
  if (dversion >= 108)
    dversion_string = "v1.08 FreeDink";
  else
    dversion_string = "v1.07 FreeDink";

  srand((unsigned)time(NULL));

  dinkc_init();  
  dinkc_bindings_init();
}

void game_quit()
{
  kill_all_scripts_for_real();

  int i = 0;
  for (i = 1; i < MAX_SPRITES_AT_ONCE; i++)
    {
      if (spr[i].custom != NULL)
	delete(spr[i].custom);
      spr[i].custom = NULL;
    }

  
  dinkc_bindings_quit();
  dinkc_quit();

  if (last_saved_game > 0)
    {
      log_info("Modifying saved game.");

      if (!add_time_to_saved_game(last_saved_game))
	log_error("Error modifying saved game.");
      last_saved_game = 0;
    }
}
