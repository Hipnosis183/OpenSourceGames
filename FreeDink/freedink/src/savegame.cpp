/**
 * Load and save game progress

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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <string.h>

#include "game_engine.h"
#include "live_sprites_manager.h"
#include "DMod.h"
#include "dinkc.h"
#include "sfx.h"
#include "bgm.h"
#include "log.h"
#include "paths.h"
#include "input.h"
#include "gfx.h"
#include "gfx_palette.h"
#include "ImageLoader.h"

/*bool*/int add_time_to_saved_game(int num)
{
  FILE *f = NULL;

  f = paths_savegame_fopen(num, "rb");
  if (!f)
    {
      log_error("Couldn't load save game %d", num);
      return /*false*/0;
    }

  int minutes = 0;
  int minutes_offset = 200;
  fseek(f, minutes_offset, SEEK_SET);
  minutes = read_lsb_int(f);
  fclose(f);
  
  //great, now let's resave it with added time
  log_info("Ok, adding time.");
  time_t ct;
  
  time(&ct);
  minutes += (int) (difftime(ct,time_start) / 60);
  
  f = paths_savegame_fopen(num, "rb+");
  if (f)
    {
      fseek(f, minutes_offset, SEEK_SET);
      write_lsb_int(minutes, f);
      fclose(f);
    }
  log_info("Wrote it.(%d of time)", minutes);
  
  return /*true*/1;
}


/*bool*/int load_game(int num)
{
  char skipbuf[10000]; // more than any fseek we do
  char dinkdat[50] = "";
  char mapdat[50] = "";
  
  FILE *f = NULL;
  
  //lets get rid of our magic and weapon scripts
  if (weapon_script != 0)
    {
      if (locate(weapon_script, "DISARM"))
	{
	  run_script(weapon_script);
	}
    }

  if (magic_script != 0 && locate(magic_script, "DISARM"))
    run_script(magic_script);

  bow.active = /*false*/0;
  weapon_script = 0;
  magic_script = 0;
  midi_active = /*true*/1;
  
  if (last_saved_game > 0)
    {
      log_info("Modifying saved game.");
      if (!add_time_to_saved_game(last_saved_game))
	log_error("Error modifying saved game.");
    }
  StopMidi();
  
  f = paths_savegame_fopen(num, "rb");
  if (!f)
    {
      log_error("Couldn't load save game %d", num);
      return /*false*/0;
    }


  /* Portably load struct player_info play from disk */
  int i = 0;
  // TODO: check 'version' field and warn/upgrade/downgrade if
  // savegame version != dversion
  fread(skipbuf, 4, 1, f);
  fread(skipbuf, 77+1, 1, f); // skip save_game_info, cf. save_game_small(...)
  fread(skipbuf, 118, 1, f); // unused
  // offset 200
  play.minutes = read_lsb_int(f);
  spr[1].x = read_lsb_int(f);
  spr[1].y = read_lsb_int(f);
  fread(skipbuf, 4, 1, f); // unused 'die' field
  spr[1].size = read_lsb_int(f);
  spr[1].defense = read_lsb_int(f);
  spr[1].dir = read_lsb_int(f);
  spr[1].pframe = read_lsb_int(f);
  spr[1].pseq = read_lsb_int(f);
  spr[1].seq = read_lsb_int(f);
  spr[1].frame = read_lsb_int(f);
  spr[1].strength = read_lsb_int(f);
  spr[1].base_walk = read_lsb_int(f);
  spr[1].base_idle = read_lsb_int(f);
  spr[1].base_hit = read_lsb_int(f);
  spr[1].que = read_lsb_int(f);
  // offset 264

  // skip first originally unused mitem entry
  fread(skipbuf, 20, 1, f);
  for (i = 0; i < NB_MITEMS; i++)
    {
      play.mitem[i].active = fgetc(f);
      fread(play.mitem[i].name, 11, 1, f);
      /* The item script could overflow, overwriting 'seq'; if */
      play.mitem[i].name[11-1] = '\0'; // safety
      play.mitem[i].seq = read_lsb_int(f);
      play.mitem[i].frame = read_lsb_int(f);
    }
  // skip first originally unused item entry
  fread(skipbuf, 20, 1, f);
  for (i = 0; i < NB_ITEMS; i++)
    {
      play.item[i].active = fgetc(f);
      fread(play.item[i].name, 11, 1, f);
      play.item[i].name[11-1] = '\0'; // safety
      play.item[i].seq = read_lsb_int(f);
      play.item[i].frame = read_lsb_int(f);
    }
  // offset 784

  play.curitem = read_lsb_int(f) - 1;
  fread(skipbuf, 4, 1, f); // reproduce unused 'unused' field
  fread(skipbuf, 4, 1, f); // reproduce unused 'counter' field
  fread(skipbuf, 1, 1, f); // reproduce unused 'idle' field
  fread(skipbuf, 3, 1, f); // reproduce memory alignment
  // offset 796

  for (i = 0; i < 769; i++)
    {
      /* Thoses are char arrays, not null-terminated strings */
      int j = 0;
      fread(play.spmap[i].type, 100, 1, f);
      for (j = 0; j < 100; j++)
	play.spmap[i].seq[j] = read_lsb_short(f);
      fread(play.spmap[i].frame, 100, 1, f);
      play.spmap[i].last_time = read_lsb_uint(f);
    }

  /* Here's we'll perform a few tricks to respect a misconception in
     the original savegame format */
  // skip first originally unused play.button entry
  fread(skipbuf, 4, 1, f);
  // first play.var entry (cf. below) was overwritten by
  // play.button[10], writing 10 play.button entries:
  for (i = 0; i < 10; i++) // use fixed 10 rather than NB_BUTTONS
    input_set_button_action(i, read_lsb_int(f));
  // skip the rest of first unused play.var entry
  fread(skipbuf, 32-4, 1, f);

  // reading the rest of play.var
  for (i = 1; i < MAX_VARS; i++)
    {
      play.var[i].var = read_lsb_int(f);
      fread(play.var[i].name, 20, 1, f);
      play.var[i].name[20-1] = '\0'; // safety
      play.var[i].scope = read_lsb_int(f);
      play.var[i].active = fgetc(f);
      fread(skipbuf, 3, 1, f); // reproduce memory alignment
    }

  play.push_active = fgetc(f);
  fread(skipbuf, 3, 1, f); // reproduce memory alignment
  play.push_dir = read_lsb_int(f);

  play.push_timer = read_lsb_int(f);

  play.last_talk = read_lsb_int(f);
  play.mouse = read_lsb_int(f);
  play.item_type = (enum item_type)fgetc(f);
  fread(skipbuf, 3, 1, f); // reproduce memory alignment
  play.last_map = read_lsb_int(f);
  fread(skipbuf, 4, 1, f); // reproduce unused 'crap' field
  fread(skipbuf, 95 * 4, 1, f); // reproduce unused 'buff' field
  fread(skipbuf, 20 * 4, 1, f); // reproduce unused 'dbuff' field
  fread(skipbuf, 10 * 4, 1, f); // reproduce unused 'lbuff' field
  
  /* v1.08: use wasted space for storing file location of map.dat,
     dink.dat, palette, and tiles */
  /* char cbuff[6000]; */
  /* Thoses are char arrays, not null-terminated strings */
  fread(mapdat, 50, 1, f);
  fread(dinkdat, 50, 1, f);
  fread(play.palette, 50, 1, f);

  for (i = 0; i < GFX_TILES_NB_SETS+1; i++)
    {
      fread(play.tile[i].file, 50, 1, f);
      play.tile[i].file[50-1] = '\0'; // safety
    }
  for (i = 0; i < 100; i++)
    {
      fread(play.func[i].file, 10, 1, f);
      play.func[i].file[10-1] = '\0'; // safety
      fread(play.func[i].func, 20, 1, f);
      play.func[i].func[20-1] = '\0'; // safety
    }
  /* Remains 750 unused chars at the end of the file. */
  /* fread(play.cbuff, 750, 1, f); */

  fclose(f);


  if (dversion >= 108)
    {
      // new map, if exist
      if (strlen (mapdat) > 0 && strlen (dinkdat) > 0)
	{
	  g_dmod.map = EditorMap(dinkdat, mapdat);
	  g_dmod.map.load();
	}
      
      // load palette
      if (strlen(play.palette) > 0)
	{
	  if (gfx_palette_set_from_bmp(play.palette) < 0)
	    log_error("Couldn't load palette from '%s': %s", play.palette, SDL_GetError());
	  gfx_palette_get_phys(GFX_ref_pal);
	}
      
      /* Reload tiles */
      g_dmod.bgTilesets.loadDefault();
      
      /* Replace with custom tiles if needed */
      for (i = 1; i <= GFX_TILES_NB_SETS; i++)
	if (strlen(play.tile[i].file) > 0)
	  g_dmod.bgTilesets.loadSlot(i, play.tile[i].file);
    }
  
  
  spr[1].damage = 0;
  walk_off_screen = 0;
  spr[1].nodraw = 0;
  push_active = 1;
  
  time(&time_start);
  
  int script = load_script("main", 0);
  locate(script, "main");
  run_script(script);
  //lets attach our vars to the scripts
  
  attach();
  log_debug("Attached vars.");
  dinkspeed = 3;
  
  if (*pcur_weapon >= 1 && *pcur_weapon <= NB_ITEMS)
    {
      if (play.item[*pcur_weapon - 1].active == 0)
	{
	  *pcur_weapon = 1;
	  weapon_script = 0;
	  log_error("Loadgame error: Player doesn't have armed weapon - changed to 1.");
	}
      else
	{
	  weapon_script = load_script(play.item[*pcur_weapon - 1].name, 1000);
	  if (locate(weapon_script, "DISARM"))
	    run_script(weapon_script);
	  weapon_script = load_script(play.item[*pcur_weapon - 1].name, 1000);
	  if (locate(weapon_script, "ARM"))
	    run_script(weapon_script);
	}
    }
  if (*pcur_magic >= 1 && *pcur_magic <= NB_MITEMS)
    {
      if (play.item[*pcur_magic - 1].active == /*false*/0)
	{
	  *pcur_magic = 0;
	  magic_script = 0;
	  log_error("Loadgame error: Player doesn't have armed magic - changed to 0.");
	}
      else
	{
	  
	  magic_script = load_script(play.mitem[*pcur_magic - 1].name, 1000);
	  if (locate(magic_script, "DISARM"))
	    run_script(magic_script);
	  magic_script = load_script(play.mitem[*pcur_magic - 1].name, 1000);
	  if (locate(magic_script, "ARM"))
	    run_script(magic_script);
	}
    }
  kill_repeat_sounds_all();
  game_load_screen(g_dmod.map.loc[*pplayer_map]);
  log_info("Loaded map.");
  draw_screen_game();
  log_info("Map drawn.");
  
  last_saved_game = num;
  
  return /*true*/1;
}


void save_game(int num)
{
  char skipbuf[10000]; // more than any fseek we do
  memset(skipbuf, 0, 10000);

  FILE *f;

  //lets set some vars first
  time_t ct;
  time(&ct);
  play.minutes += (int) (difftime(ct,time_start) / 60);
        //reset timer
  time(&time_start);

  last_saved_game = num;
  f = paths_savegame_fopen(num, "wb");
  if (f == NULL)
    {
      perror("Cannot save game");
      return;
    }

  /* Portably dump struct player_info play to disk */
  int i = 0;
  write_lsb_int(dversion, f);
  // set_save_game_info() support:
  {
    char* info_temp = strdup(save_game_info);
    decipher_string(&info_temp, 0);
    fwrite(info_temp, 77+1, 1, f);
    free(info_temp);
  }
  fwrite(skipbuf, 118, 1, f); // unused
  // offset 200
  write_lsb_int(play.minutes, f);
  write_lsb_int(spr[1].x, f);
  write_lsb_int(spr[1].y, f);
  fwrite(skipbuf, 4, 1, f); // unused 'die' field
  write_lsb_int(spr[1].size, f);
  write_lsb_int(spr[1].defense, f);
  write_lsb_int(spr[1].dir, f);
  write_lsb_int(spr[1].pframe, f);
  write_lsb_int(spr[1].pseq, f);
  write_lsb_int(spr[1].seq, f);
  write_lsb_int(spr[1].frame, f);
  write_lsb_int(spr[1].strength, f);
  write_lsb_int(spr[1].base_walk, f);
  write_lsb_int(spr[1].base_idle, f);
  write_lsb_int(spr[1].base_hit, f);
  write_lsb_int(spr[1].que, f);
  // offset 264

  // skip first originally unused mitem entry
  fwrite(skipbuf, 20, 1, f);
  for (i = 0; i < NB_MITEMS; i++)
    {
      fputc(play.mitem[i].active, f);
      fwrite(play.mitem[i].name, 11, 1, f);
      write_lsb_int(play.mitem[i].seq, f);
      write_lsb_int(play.mitem[i].frame, f);
    }
  // skip first originally unused item entry
  fwrite(skipbuf, 20, 1, f);
  for (i = 0; i < NB_ITEMS; i++)
    {
      fputc(play.item[i].active, f);
      fwrite(play.item[i].name, 11, 1, f);
      write_lsb_int(play.item[i].seq, f);
      write_lsb_int(play.item[i].frame, f);
    }
  // offset 784

  write_lsb_int(play.curitem + 1, f);
  fwrite(skipbuf, 4, 1, f); // reproduce unused 'unused' field
  fwrite(skipbuf, 4, 1, f); // reproduce unused 'counter' field
  fwrite(skipbuf, 1, 1, f); // reproduce unused 'idle' field
  fwrite(skipbuf, 3, 1, f); // reproduce memory alignment
  // offset 796

  for (i = 0; i < 769; i++)
    {
      int j = 0;
      fwrite(play.spmap[i].type, 100, 1, f);
      for (j = 0; j < 100; j++)
	write_lsb_short(play.spmap[i].seq[j], f);
      fwrite(play.spmap[i].frame, 100, 1, f);
      write_lsb_uint(play.spmap[i].last_time, f);
    }

  /* Here's we'll perform a few tricks to respect a misconception in
     the original savegame format */
  // skip first originally unused play.button entry
  fwrite(skipbuf, 4, 1, f);
  // first play.var entry (cf. below) was overwritten by
  // play.button[10], writing 10 play.button entries:
  for (i = 0; i < 10; i++) // use fixed 10 rather than NB_BUTTONS
    write_lsb_int(input_get_button_action(i), f);
  // skip the rest of first unused play.var entry
  fwrite(skipbuf, 32-4, 1, f);

  // writing the rest of play.var
  for (i = 1; i < MAX_VARS; i++)
    {
      write_lsb_int(play.var[i].var, f);
      fwrite(play.var[i].name, 20, 1, f);
      write_lsb_int(play.var[i].scope, f);
      fputc(play.var[i].active, f);
      fwrite(skipbuf, 3, 1, f); // reproduce memory alignment
    }

  fputc(play.push_active, f);
  fwrite(skipbuf, 3, 1, f); // reproduce memory alignment
  write_lsb_int(play.push_dir, f);

  write_lsb_int(play.push_timer, f);

  write_lsb_int(play.last_talk, f);
  write_lsb_int(play.mouse, f);
  fputc(play.item_type, f);
  fwrite(skipbuf, 3, 1, f); // reproduce memory alignment
  write_lsb_int(play.last_map, f);
  fwrite(skipbuf, 4, 1, f); // reproduce unused 'crap' field
  fwrite(skipbuf, 95 * 4, 1, f); // reproduce unused 'buff' field
  fwrite(skipbuf, 20 * 4, 1, f); // reproduce unused 'dbuff' field
  fwrite(skipbuf, 10 * 4, 1, f); // reproduce unused 'lbuff' field
  
  /* v1.08: use wasted space for storing file location of map.dat,
     dink.dat, palette, and tiles */
  /* char cbuff[6000];*/
  fwrite(g_dmod.map.map_dat.c_str(), 50, 1, f);
  fwrite(g_dmod.map.dink_dat.c_str(), 50, 1, f);
  fwrite(play.palette, 50, 1, f);

  for (i = 0; i < GFX_TILES_NB_SETS+1; i++)
    fwrite(play.tile[i].file, 50, 1, f);
  for (i = 0; i < 100; i++)
    {
      fwrite(play.func[i].file, 10, 1, f);
      fwrite(play.func[i].func, 20, 1, f);
    }
  fwrite(skipbuf, 750, 1, f);

  fclose(f);

#ifdef __EMSCRIPTEN__
  // Flush changes to IDBFS
  EM_ASM(
	FS.syncfs(false, function(err) { if (err) { console.trace(); console.log(err); } })
  );
#endif
}
