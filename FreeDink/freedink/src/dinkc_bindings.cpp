/**
 * Link game engine and DinkC script engine

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015  Sylvain Beucler

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

#include "dinkc_bindings.h"

#include <stdio.h>
#include <stdlib.h> /* atol */
#include <time.h>
#include <math.h>
#include <alloca.h>
#include <string.h>
#include <ctype.h> /* tolower */
#include <xalloc.h>

#include "game_engine.h"
#include "live_sprites_manager.h"
#include "live_screen.h"
#include "DMod.h"
#include "editor_screen.h"
#include "dinkc.h"
#include "freedink.h"
#include "IOGfxPrimitives.h"
#include "ImageLoader.h"
#include "gfx.h"
#include "gfx_fonts.h"
#include "gfx_palette.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "dinkini.h"
#include "bgm.h"
#include "sfx.h"
#include "input.h"
#include "str_util.h"
#include "paths.h"
#include "log.h"
#include "dinkc_console.h"
#include "i18n.h"
#include "inventory.h" /* add_item, ITEM_*, kill_item_script,
			  kill_mitem_script, kill_cur_magic */
#include "game_choice.h" /* talk, talk_clear */
#include "status.h"
#include "text.h"
#include "savegame.h"
#include "brain.h"


int change_sprite(int h, int val, int *change)
{
  //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
  if (h < 1 || h >= MAX_SPRITES_AT_ONCE)
    {
      log_error("Error with an SP command - Sprite %d is invalid.", h);
      return -1;
    }

  if (!spr[h].active)
    return -1;

  if (val != -1)
    *change = val;
  
  return *change;
  
}

/**
 * Change sprite value even if value is -1
 */
int change_sprite_noreturn(int h, int val, int* change)
{
  //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
  if (h < 0
      || h >= MAX_SPRITES_AT_ONCE
      || !spr[h].active)
    return -1;

  *change = val;

  return(*change);
}


int change_edit(int h, int val, unsigned short* change)
{
  //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
  
  if (h < 1 || h > 99)
    return -1;

  if (val != -1)
    *change = val;
  
  return *change;
}

/**
 * Sanity-check and set an editor variable (editor_type(),
 * editor_seq() and editor_frame())
 */
int change_edit_char(int h, int val, unsigned char* change)
{
  //Msg("Searching sprite %s with val %d.  Cur is %d", h, val, *change);
  //  Msg("h is %d..",val);
  if (h < 1 || h > 99)
    return -1;

  if (val != -1)
    *change = val;
  
  return *change;
}


/***************/
/*  DinkC API  */
/*             */
/***************/

/**
 * Short-hand to check for invalid sprites and avoid segfaults.
 * Also warn the D-Mod author about it.
 */
#define STOP_IF_BAD_SPRITE(sprite)                                             \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)                            \
    {                                                                          \
      log_error("[DinkC] %s:%d:%s: invalid sprite %d (offset %ld)",             \
                rinfo[script]->name, rinfo[script]->debug_line,                \
                cur_funcname, sprite, rinfo[script]->current);                 \
      return;                                                                  \
    }

/**
 * sp_* functions used to call 'change_sprite' on spr[sprite] without
 * checking if 'sprite' was in [1; MAX_SPRITES_AT_ONCE-1]. Since
 * 'change_sprite' returns -1 when 'sprite' is inactive, that's also
 * what we return when the sprite is out of range.
 */
#define RETURN_NEG_IF_BAD_SPRITE(sprite)                                       \
  if (sprite <= 0 || sprite >= MAX_SPRITES_AT_ONCE)                            \
    {                                                                          \
      log_error("[DinkC] %s:%d:%s: invalid sprite %d (offset %ld)",             \
                rinfo[script]->name, rinfo[script]->debug_line,                \
                cur_funcname, sprite, rinfo[script]->current);                 \
      *preturnint = -1;                                                        \
      return;                                                                  \
    }


void dc_sp_active(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  int val = (int)spr[sprite].active;
  *preturnint = change_sprite(sprite, sparg, &val);
  if (!val)
    lsm_remove_sprite(sprite);
}

void dc_sp_attack_hit_sound(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].attack_hit_sound);
}

void dc_sp_attack_hit_sound_speed(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].attack_hit_sound_speed);
}

void dc_sp_attack_wait(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg+thisTickCount, &spr[sprite].attack_wait);
}

void dc_sp_base_attack(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_attack);
}

void dc_sp_base_die(int script, int* yield, int* preturnint, int sprite, int base_sequence)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, base_sequence, &spr[sprite].base_die);
}

void dc_sp_base_hit(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_hit);
}

void dc_sp_base_idle(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_idle);
}

void dc_sp_base_walk(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].base_walk);
}

void dc_sp_brain(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain);
}

void dc_sp_brain_parm(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain_parm);
}

void dc_sp_brain_parm2(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].brain_parm2);
}

void dc_sp_defense(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].defense);
}

void dc_sp_dir(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].dir);
  if (sparg != -1)
    changedir(spr[sprite].dir, sprite, spr[sprite].base_walk);
}

void dc_sp_disabled(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].disabled);
}

void dc_sp_distance(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].distance);
}

void dc_sp_exp(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].exp);
}

void dc_sp_flying(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].flying);
}

void dc_sp_follow(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].follow);
}

void dc_sp_frame(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].frame);
}

void dc_sp_frame_delay(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].frame_delay);
}

void dc_sp_gold(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].gold);
}

void dc_sp_hard(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].hard);
  if (spr[sprite].sp_index != 0 && sparg != -1)
    cur_ed_screen.sprite[spr[sprite].sp_index].hard = *preturnint;
}

void dc_sp_hitpoints(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].hitpoints);
}

void dc_sp_move_nohard(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].move_nohard);
}

void dc_sp_mx(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].mx);
}

void dc_sp_my(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].my);
}

void dc_sp_noclip(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].noclip);
}

void dc_sp_nocontrol(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nocontrol);
}

void dc_sp_nodraw(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nodraw);
}

void dc_sp_nohit(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].nohit);
}

void dc_sp_notouch(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].notouch);
}

void dc_sp_pframe(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].pframe);
}

void dc_sp_picfreeze(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].picfreeze);
}

void dc_sp_pseq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].pseq);
}

void dc_sp_que(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].que);
}

void dc_sp_range(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].range);
}

void dc_sp_reverse(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].reverse);
}

void dc_sp_seq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  if ((sparg < 0 || sparg >= MAX_SEQUENCES) && sparg != -1)
    {
      log_error("[DinkC] %s:%d:%s: invalid sequence %d, ignoring (offset %ld)",
                rinfo[script]->name, rinfo[script]->debug_line,
                cur_funcname, sparg, rinfo[script]->current);
      *preturnint = -1;
      return;
    }
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].seq);
}

void dc_sp_size(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].size);
}

void dc_sp_sound(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].sound);
  if (sparg > 0)
    SoundPlayEffect(spr[sprite].sound,22050, 0, sprite, 1);
}

void dc_sp_speed(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].speed);
  if (sparg != -1)
    changedir(spr[sprite].dir, sprite, spr[sprite].base_walk);
}

void dc_sp_strength(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].strength);
}

void dc_sp_target(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].target);
}

void dc_sp_timing(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].timing);
}

void dc_sp_touch_damage(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite_noreturn(sprite, sparg, &spr[sprite].touch_damage);
}

void dc_sp_x(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].x);
}

void dc_sp_y(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  *preturnint = change_sprite(sprite, sparg, &spr[sprite].y);
}



void dc_sp_kill(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].kill_ttl = sparg;
}

void dc_sp_editor_num(int script, int* yield, int* preturnint, int sprite)
{
  *preturnint = 0;
  if (sprite > 0 && sprite < MAX_SPRITES_AT_ONCE)
    *preturnint = spr[sprite].sp_index;
  else
    log_error("[DinkC] sp_editor_num: invalid sprite %d", sprite);
}


void dc_sp_kill_wait(int script, int* yield, int* preturnint, int sprite)
{
  if (sprite > 0 && sprite < MAX_SPRITES_AT_ONCE)
    spr[sprite].wait = 0;
  else
    log_error("[DinkC] sp_kill_wait: invalid sprite %d", sprite);
}

void dc_sp_script(int script, int* yield, int* preturnint, int sprite, char* dcscript)
{
  if (sprite <= 0 || (sprite >= MAX_SPRITES_AT_ONCE && sprite != 1000))
    {
      log_error("[DinkC] %s:%d:%s: cannot process sprite %d??",
                rinfo[script]->name, rinfo[script]->debug_line, cur_funcname,
		sprite);
      return;
    }
  kill_scripts_owned_by(sprite);
  int new_script = load_script(dcscript, sprite);
  if (new_script == 0)
    {
      *preturnint = 0;
      return;
    }
  spr[sprite].script = new_script;

  int tempreturn = 0;
  if (sprite != 1000)
    {
      tempreturn = spr[sprite].script;

      if (screen_main_is_running) {
	log_info("Not running %s until later..", rinfo[spr[sprite].script]->name);
      } else {
	locate(spr[sprite].script, "MAIN");
	run_script(spr[sprite].script);
      }
    }
    
  *preturnint = tempreturn;
}


void dc_unfreeze(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (spr[sprite].active)
    spr[sprite].freeze = 0;
  else
    log_error("[DinkC] Couldn't unfreeze sprite %d in script %d, it doesn't exist.", sprite, script);
}

void dc_freeze(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (spr[sprite].active)
    spr[sprite].freeze = script;
  else
    log_error("[DinkC] Couldn't freeze sprite %d in script %d, it doesn't exist.", sprite, script);
}

void dc_set_callback_random(int script, int* yield, int* preturnint, char* procedure, int base, int range)
{
  int retval = add_callback(procedure, base, range, script);
  if (dversion >= 108)
    *preturnint = retval;
}

void dc_set_dink_speed(int script, int* yield, int* preturnint, int speed)
{
  if (dversion < 108)
    dinkspeed = speed;
  else if (speed != 0)
    dinkspeed = speed;
}

void dc_reset_timer(int script, int* yield, int* preturnint)
{
  time(&time_start);
  play.minutes = 0;
}

void dc_set_keep_mouse(int script, int* yield, int* preturnint, int keep_mouse_p)
{
  set_keep_mouse(keep_mouse_p);
}

void dc_add_item(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame)
{
  add_item(dcscript, sequence, frame, ITEM_REGULAR);
}

void dc_add_magic(int script, int* yield, int* preturnint, char* dcscript, int sequence, int frame)
{
  add_item(dcscript, sequence, frame, ITEM_MAGIC);
}

void dc_add_exp(int script, int* yield, int* preturnint, int amount, int active_sprite)
{
  STOP_IF_BAD_SPRITE(active_sprite);

  if (dversion >= 108)
    // fix - made work with all sprites when
    // using add_exp DinkC command
    add_exp_force(amount, active_sprite);
  else
    add_exp(amount, active_sprite);
}

void dc_kill_this_item(int script, int* yield, int* preturnint, char* dcscript)
{
  kill_item_script(dcscript);
}

void dc_kill_this_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  kill_mitem_script(dcscript);
}

void dc_show_bmp(int script, int* yield, int* preturnint, char* bmp_file, int show_map_dot, int unused)
{
  log_info("showing BMP");
  wait4b.active = /*false*/0;
  show_bmp(bmp_file, script);
  
  showb.active = /*true*/1;
  showb.showdot = show_map_dot;
  showb.script = script;

  *yield = 1;
}

void dc_copy_bmp_to_screen(int script, int* yield, int* preturnint, char* bmp_file)
{
  log_info("copying BMP");
  copy_bmp(bmp_file);
}

void dc_wait_for_button(int script, int* yield, int* preturnint)
{
  log_info("waiting for button with script %d", script);
  wait4b.script = script;
  wait4b.active = /*true*/1;
  wait4b.button = 0;
  *yield = 1;
}

void dc_stop_wait_for_button(int script, int* yield, int* preturnint)
{
  wait4b.active = /*false*/0;
}

void dc_load_screen(int script, int* yield, int* preturnint)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  //Msg("Loading map %d..",*pplayer_map);
  update_screen_time();

  if (*pplayer_map > 768) {
    log_error("[DinkC] %s:%d:%s(): invalid screen %d", rinfo[script]->name,
			  rinfo[script]->debug_line, cur_funcname,
			  *pplayer_map);
	return;
  }

  game_load_screen(g_dmod.map.loc[*pplayer_map]);

  // update indicator on mini-map
  if (g_dmod.map.indoor[*pplayer_map] == 0)
    play.last_map = *pplayer_map;
    
  return;
}

/**
 * Decipher a copy of 'text' (to avoid potentially realloc'ing it) and
 * call 'say_text(...)'
 */
static int say_text_from_dc(char* text, int active_sprite, int script)
{
  log_debug("[DinkC] %s:%d:%s(\"%s\", %d)", rinfo[script]->name,
	    rinfo[script]->debug_line, cur_funcname,
	    text, active_sprite);
  if (active_sprite != 1000 && !lsm_isValidSprite(active_sprite))
	  log_error("[DinkC] %s:%d:%s(\"%s\", %d): %d is not a valid sprite", rinfo[script]->name,
				rinfo[script]->debug_line, cur_funcname,
				text, active_sprite, active_sprite);

  /* Translate text (before variable substitution) */
  char* translation = NULL;
  if (strlen(text) >= 2 && text[0] == '`')
    {
      char* temp = i18n_translate(rinfo[script]->name, rinfo[script]->debug_line, text+2);
      translation = (char*)xmalloc(strlen(temp) + 2 + 1);
      sprintf(translation, "%c%c%s", text[0], text[1], temp);
      free(temp);
    }
  else
    {
      translation = i18n_translate(rinfo[script]->name, rinfo[script]->debug_line, text);
    }

  /* Substitute variables */
  char* expanded = strdup(translation);
  free(translation);
  decipher_string(&expanded, script);

  int text_sprite = say_text(expanded, active_sprite, script);
  free(expanded);
  return text_sprite;
}

/**
 * Decipher a copy of 'text' (to avoid potentially realloc'ing it) and
 * call 'say_text_xy(...)'
 */
static int say_text_xy_from_dc(char* text, int x, int y, int script)
{
  log_debug("[DinkC] %s:%d:%s(\"%s\", %d, %d)", rinfo[script]->name,
	    rinfo[script]->debug_line, cur_funcname,
	    text, x, y);

  /* Translate text (before variable substitution) */
  char* translation = NULL;
  if (strlen(text) >= 2 && text[0] == '`')
    {
      char* temp = i18n_translate(rinfo[script]->name, rinfo[script]->debug_line, text+2);
      translation = (char*)xmalloc(strlen(temp) + 2 + 1);
      sprintf(translation, "%c%c%s", text[0], text[1], temp);
      free(temp);
    }
  else
    {
      translation = i18n_translate(rinfo[script]->name, rinfo[script]->debug_line, text);
    }

  /* Substitute variables */
  char* expanded = strdup(translation);
  free(translation);
  decipher_string(&expanded, script);

  int text_sprite = say_text_xy(expanded, x, y, script);
  free(expanded);
  return text_sprite;
}

void dc_say(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* 1000 is a valid value, and bad values don't trigger segfaults
     in this particular function; so don't validate active_sprite */
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  if (active_sprite == 0)
    {
      log_error("[DinkC] say_stop: Sprite 0 can talk? Yeah, didn't think so.");
      return;
    }

  if (active_sprite != 1000)
    kill_text_owned_by(active_sprite);

  *preturnint = say_text_from_dc(text, active_sprite, script);
}

void dc_say_stop(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  if (active_sprite == 0)
    {
      log_error("[DinkC] say_stop: Sprite 0 can talk? Yeah, didn't think so.");
      return;
    }
    
  kill_text_owned_by(active_sprite);
  kill_text_owned_by(1);
  kill_returning_stuff(script);

  int sprite = say_text_from_dc(text, active_sprite, script);
  *preturnint = sprite;

  spr[sprite].say_stop_callback = script;
  play.last_talk = script;
  //Msg("Sprite %d marked callback true.", sprite);
    
  *yield = 1;
}

void dc_say_stop_npc(int script, int* yield, int* preturnint, char* text, int active_sprite)
{
  /* STOP_IF_BAD_SPRITE(active_sprite); */

  /* no-op if already talking */
  if (text_owned_by(active_sprite))
    {
      *preturnint = 0;
      return;
    }
    
  kill_returning_stuff(script);

  int sprite = say_text_from_dc(text, active_sprite, script);
  spr[sprite].say_stop_callback = script;
    
  *yield = 1;
}

void dc_say_stop_xy(int script, int* yield, int* preturnint, char* text, int x, int y)
{
  kill_returning_stuff(script);

  int sprite = say_text_xy_from_dc(text, x, y, script);
  spr[sprite].say_stop_callback = script;
  spr[sprite].live = /*true*/1;
  play.last_talk = script;
  *yield = 1;
}

void dc_say_xy(int script, int* yield, int* preturnint, char* text, int x, int y)
{
  kill_returning_stuff(script);
  *preturnint = say_text_xy_from_dc(text, x, y, script);
}

void dc_draw_screen(int script, int* yield, int* preturnint)
{
  /* only refresh screen if not in a cut-scene */
  /* do it before draw_screen_game() because that one calls
     kill_all_scripts(), which NULLifies rinfo[script] */
  if (rinfo[script]->sprite != 1000)
    *yield = 1;
  draw_screen_game();
}

void dc_free_items(int script, int* yield, int* preturnint)
{
  *preturnint = 0;
  int i = 0;
  for (; i < NB_ITEMS; i++)
    {
      if (play.item[i].active == 0)
	*preturnint += 1;
    }
}

void dc_free_magic(int script, int* yield, int* preturnint)
{
  *preturnint = 0;

  int i = 0;
  for (; i < NB_MITEMS; i ++)
    {
      if (play.mitem[i-1].active == 0)
	*preturnint += 1;
    }
}

void dc_kill_cur_item(int script, int* yield, int* preturnint)
{
  *preturnint = 0;
  kill_cur_item();
  *yield = 1;
  /* unlike documented, this stops script execution, even in 1.08 */
}

void dc_kill_cur_magic(int script, int* yield, int* preturnint)
{
  *preturnint = 0;
  kill_cur_magic();
  *yield = 1;
  /* unlike documented, this stops script execution, even in 1.08 */
}

void dc_draw_status(int script, int* yield, int* preturnint)
{
  draw_status_all();
}

void dc_arm_weapon(int script, int* yield, int* preturnint)
{
  if (weapon_script != 0 && locate(weapon_script, "DISARM"))
    run_script(weapon_script);

  weapon_script = load_script(play.item[*pcur_weapon - 1].name, 1000);
  if (locate(weapon_script, "ARM"))
    run_script(weapon_script);
}

void dc_arm_magic(int script, int* yield, int* preturnint)
{
  if (magic_script != 0 && locate(magic_script, "DISARM"))
    run_script(magic_script);
    
  magic_script = load_script(play.mitem[*pcur_magic - 1].name, 1000);
  if (locate(magic_script, "ARM"))
    run_script(magic_script);
}

void dc_restart_game(int script, int* yield, int* preturnint)
{
  game_restart();
  *yield = 1;
}

void dc_wait(int script, int* yield, int* preturnint, int delayms)
{
  kill_returning_stuff(script);
  add_callback("", delayms, 0, script);
  *yield = 1;
}

void dc_preload_seq(int script, int* yield, int* preturnint, int sequence)
{
  check_seq_status(sequence);
}

void dc_script_attach(int script, int* yield, int* preturnint, int sprite)
{
  /* STOP_IF_BAD_SPRITE(sprite); */
  rinfo[script]->sprite = sprite;
}

void dc_draw_hard_sprite(int script, int* yield, int* preturnint, int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);

  update_play_changes();
  int l = sprite;
  rect mhard;
  rect_copy(&mhard, &k[seq[spr[l].pseq].frame[spr[l].pframe]].hardbox);
  rect_offset(&mhard, (spr[l].x- 20), spr[l].y);

  fill_hardxy(mhard);
  fill_back_sprites();
  fill_hard_sprites();
}


void dc_activate_bow(int script, int* yield, int* preturnint)
{
  spr[1].seq = 0;
  spr[1].pseq = 100+spr[1].dir;
  spr[1].pframe = 1;
  bow.active = /*true*/1;
  bow.script = script;
  bow.hitme = /*false*/0;
  bow.time = 0;
    
  /*      bowsound->Release();
    
  //lpDS->DuplicateSoundBuffer(ssound[42].sound,&bowsound);
  //bowsound->Play(0, 0, DSBPLAY_LOOPING);
  */
    
  *yield = 1;
}

void dc_disable_all_sprites(int script, int* yield, int* preturnint)
{
  int jj;
  for (jj = 1; jj < last_sprite_created; jj++)
    if (spr[jj].active) spr[jj].disabled = /*true*/1;
}

void dc_draw_background(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  draw_screen_game_background();
}

void dc_draw_hard_map(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  log_info("Drawing hard map..");
  update_play_changes();
  fill_whole_hard();
  fill_hard_sprites();
  fill_back_sprites();
}

void dc_enable_all_sprites(int script, int* yield, int* preturnint)
{
  int jj;
  for (jj = 1; jj < last_sprite_created; jj++)
    if (spr[jj].active) spr[jj].disabled = /*false*/0;
}

void dc_fade_down(int script, int* yield, int* preturnint)
{
  if (!process_upcycle)
    {
      process_downcycle = /*true*/1;
    }
  else
    {
      log_warn("[DinkC] %s:%d: fade_down() called during fade_up(), ignoring fade_down()",
                rinfo[script]->name, rinfo[script]->debug_line);
    }
  cycle_clock = thisTickCount+1000;
  cycle_script = script;
  *yield = 1;
}

void dc_fade_up(int script, int* yield, int* preturnint)
{
  if (process_downcycle)
    {
      log_warn("[DinkC] %s:%d: fade_up() called during fade_down(), forcing fade_up()",
                rinfo[script]->name, rinfo[script]->debug_line);
      process_downcycle = 0;
    }
  process_upcycle = /*true*/1;
  cycle_script = script;
  *yield = 1;
}

void dc_get_burn(int script, int* yield, int* preturnint)
{
  *preturnint = 1;
}

void dc_get_last_bow_power(int script, int* yield, int* preturnint)
{
  *preturnint = bow.last_power;
}

void dc_get_version(int script, int* yield, int* preturnint)
{
  *preturnint = dversion;
}

void dc_kill_all_sounds(int script, int* yield, int* preturnint)
{
  kill_repeat_sounds_all();
}

void dc_kill_game(int script, int* yield, int* preturnint)
{
  log_info("Was told to kill game, so doing it like a good boy.");
  /* Send QUIT event to the main game loop,
     which will cleanly exit */
  SDL_Event ev;
  ev.type = SDL_QUIT;
  SDL_PushEvent(&ev);
  *yield = 1;
}

void dc_kill_this_task(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  if (rinfo[script]->proc_return != 0)
    {
      run_script(rinfo[script]->proc_return);
    }
  kill_script(script);
  *yield = 1;
}

void dc_scripts_used(int script, int* yield, int* preturnint)
{
  int m = 0;
  int i;
  for (i = 1; i < MAX_SCRIPTS; i++)
    if (rinfo[i] != NULL) m++;
  *preturnint = m;
}

void dc_stopcd(int script, int* yield, int* preturnint)
{
  /* No-op */
}

void dc_stopmidi(int script, int* yield, int* preturnint)
{
  // (sprite, direction, until, nohard);
  StopMidi();
}

void dc_turn_midi_off(int script, int* yield, int* preturnint)
{
  midi_active = /*false*/0;
}

void dc_turn_midi_on(int script, int* yield, int* preturnint)
{
  midi_active = /*true*/1;
}

void dc_count_item(int script, int* yield, int* preturnint, char* dcscript)
{
  int i;
  *preturnint = 0;
  for (i = 0; i < NB_ITEMS; i++)
    {
      if (play.item[i].active
	  && compare(play.item[i].name, dcscript))
	returnint++;
    }
}

void dc_count_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  int i;
  *preturnint = 0;
  for (i = 0; i < NB_MITEMS; i++)
    {
      if (play.mitem[i].active
	  && compare(play.mitem[i].name, dcscript))
	returnint++;
    }
}

void dc_compare_sprite_script(int script, int* yield, int* preturnint, int sprite, char* dcscript)
{
  *preturnint = 0;
  STOP_IF_BAD_SPRITE(sprite);
 
  if (spr[sprite].active)
    {
      if (spr[sprite].script == 0)
	{
	  log_error("[DinkC] compare_sprite_script: Sprite %d has no script.", sprite);
	  return;
	}
      if (rinfo[spr[sprite].script] == NULL)
	{
	  log_error("[DinkC] compare_sprite_script: script %d for sprite %d was already killed!.",
		    sprite, spr[sprite].script);
	  return;
	}
      if (compare(dcscript, rinfo[spr[sprite].script]->name))
	{
	  *preturnint = 1;
	  return;
	}
    }
  else
    {
      log_error("[DinkC] compare_sprite_script: Can't compare sprite script, sprite not active.");
    }
}



void dc_compare_weapon(int script, int* yield, int* preturnint, char* dcscript)
{
  *preturnint = 0;
  if (*pcur_weapon >= 1 && *pcur_weapon <= NB_ITEMS)
    {
      if (compare(play.item[*pcur_weapon - 1].name, dcscript))
	*preturnint = 1;
    }
}

void dc_compare_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  *preturnint = 0;

  if (*pcur_magic >= 1 && *pcur_magic <= NB_MITEMS)
    {
      if (dversion >= 108)
	{
	  if (compare(play.mitem[*pcur_magic - 1].name, dcscript))
	    *preturnint = 1;
	}
      else
	{
	  /* reproduce v1.07 bug: compare with regular item rather than
	     magic item */
	  if (compare(play.item[*pcur_magic - 1].name, dcscript))
	    *preturnint = 1;
	}
    }
}

void dc_init(int script, int* yield, int* preturnint, char* dink_ini_line)
{
  figure_out(dink_ini_line);
}

void dc_dink_can_walk_off_screen(int script, int* yield, int* preturnint, int can_walk_off_screen_p)
{
  walk_off_screen = can_walk_off_screen_p;
}

void dc_push_active(int script, int* yield, int* preturnint, int dink_can_push_p)
{
  push_active = dink_can_push_p;
}

void dc_stop_entire_game(int script, int* yield, int* preturnint, int stop_p)
{
  stop_entire_game = stop_p;
  IOGFX_background->blit(IOGFX_backbuffer, NULL, NULL);
}


void dc_editor_type(int script, int* yield, int* preturnint, int editor_sprite, int type)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit_char(editor_sprite, type,
				 &play.spmap[*pplayer_map].type[editor_sprite]);
}
void dc_editor_seq(int script, int* yield, int* preturnint, int editor_sprite, int seq)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit(editor_sprite, seq,
			    &play.spmap[*pplayer_map].seq[editor_sprite]);
}

void dc_editor_frame(int script, int* yield, int* preturnint, int editor_sprite, int frame)
{
  if (editor_sprite < 0 || editor_sprite >= 100)
    return;
  *preturnint = change_edit_char(editor_sprite, frame,
				 &play.spmap[*pplayer_map].frame[editor_sprite]);
}



void dc_move(int script, int* yield, int* preturnint,
	     int sprite, int direction, int destination_limit, int ignore_hardness_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].move_active = /*true*/1;
  spr[sprite].move_dir = direction;
  spr[sprite].move_num = destination_limit;
  spr[sprite].move_nohard = ignore_hardness_p;
  spr[sprite].move_script = 0;
  log_debug("Moving: Sprite %d, dir %d, num %d", sprite, direction, destination_limit);
}

void dc_spawn(int script, int* yield, int* preturnint,
	     char* dcscript)
{
  int mysc = load_script(dcscript, 1000);
  if (mysc == 0)
    {
      *preturnint = 0;
      return;
    }
  locate(mysc, "MAIN");
  int tempreturn = mysc;
  run_script(mysc);
  *preturnint = tempreturn;
}

void dc_run_script_by_number(int script, int* yield, int* preturnint,
			     int script_index, char* funcname)
{
  if (locate(script_index, funcname))
    run_script(script_index);
}

void dc_playmidi(int script, int* yield, int* preturnint,
		 char* midi_file)
{
  //StopMidi();
  int regm = atol(midi_file);
  log_debug("Processing playmidi command.");
  if (regm > 1000)
    //cd directive
    {
      int cd_track = regm - 1000;
      log_info("playmidi - cd play command detected.");
      
      char buf[10+4+1];
      sprintf(buf, "%d.mid", cd_track);
      log_info("Playing midi %s.", buf);
      PlayMidi(buf);
      // then try to play 'midi_file' as well:
      // (necessary for START.c:playmidi("1003.mid"))
    }
  log_info("Playing midi %s.", midi_file);
  PlayMidi(midi_file);
}

void dc_playsound(int script, int* yield, int* preturnint,
		  int sound_number, int min_speed, int rand_speed_to_add, int sprite, int repeat_p)
{
  if (sprite < 0 || sprite >= MAX_SPRITES_AT_ONCE)
    sprite = 0; // no "3d" volume effect... and no segfault :p

  if (sound_on)
    *preturnint = playsound(sound_number, min_speed, rand_speed_to_add, sprite, repeat_p);
  else
    *preturnint = 0;
}

void dc_sound_set_survive(int script, int* yield, int* preturnint,
			  int sound_bank, int survive_p)
{
  //let's set one sound to survive
  if (sound_on && sound_bank > 0)
    sound_set_survive(sound_bank, survive_p);
}

void dc_sound_set_vol(int script, int* yield, int* preturnint,
		      int sound_bank, int vol)
{
  if (sound_on && sound_bank > 0)
    sound_set_vol(sound_bank, vol);
}

void dc_sound_set_kill(int script, int* yield, int* preturnint,
		       int sound_bank)
{
  if (sound_on && sound_bank > 0)
    sound_set_kill(sound_bank);
}


void dc_save_game(int script, int* yield, int* preturnint, int game_slot)
{
  save_game(game_slot);
}

void dc_force_vision(int script, int* yield, int* preturnint, int vision)
{
  *pvision = vision;
  rinfo[script]->sprite = 1000;
  fill_whole_hard();
  draw_screen_game();
}

void dc_fill_screen(int script, int* yield, int* preturnint, int palette_index)
{
	if (palette_index < 0 || palette_index > 255)
		palette_index = 0;
	IOGFX_background->fill_screen(palette_index, GFX_ref_pal);
}

void dc_load_game(int script, int* yield, int* preturnint, int game_slot)
{
  kill_all_scripts_for_real();
  *preturnint = load_game(game_slot);
  log_info("load completed.");
  if (rinfo[script] == NULL)
    log_error("[DinkC] Script %d is suddenly null!", script);
  *pupdate_status = 1;
  draw_status_all();
  *yield = 1;
}

void dc_game_exist(int script, int* yield, int* preturnint, int game_slot)
{
  FILE *fp;
  if ((fp = paths_savegame_fopen(game_slot, "rb")) != NULL)
    {
      fclose(fp);
      *preturnint = 1;
    }
  else
    {
      *preturnint = 0;
    }
}

void dc_move_stop(int script, int* yield, int* preturnint,
		  int sprite, int direction, int destination_limit, int ignore_hardness_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  spr[sprite].move_active = /*true*/1;
  spr[sprite].move_dir = direction;
  spr[sprite].move_num = destination_limit;
  spr[sprite].move_nohard = ignore_hardness_p;
  spr[sprite].move_script = script;
  log_debug("Move_stop: Sprite %d, dir %d, num %d", sprite, direction, destination_limit);
  *yield = 1;
}

void dc_load_sound(int script, int* yield, int* preturnint,
		   char* wav_file, int sound_index)
{
  if (sound_on)
    {
      log_info("getting %s..", wav_file);
      CreateBufferFromWaveFile(wav_file, sound_index);
    }
}

void dc_debug(int script, int* yield, int* preturnint,
	      char* text)
{
  /* Convert from Latin-1 (.c) to UTF-8 (SDL) since the message is
     shown on the screen in debug mode */
  char* buf = latin1_to_utf8(text);
  decipher_string(&buf, script);
  log_debug("%s", buf);
  free(buf);
}

void dc_busy(int script, int* yield, int* preturnint,
	     int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);
  *preturnint = does_sprite_have_text(sprite);
  log_debug("Busy: Return int is %d and %d.  Nlist got %d.",
	    *preturnint, does_sprite_have_text(sprite), sprite);
}


void dc_make_global_int(int script, int* yield, int* preturnint,
			char* varname, int default_val)
{
  make_int(varname, default_val, DINKC_GLOBAL_SCOPE, script);
}

void dc_inside_box(int script, int* yield, int* preturnint,
		   int x, int y, int left, int right, int top, int bottom)
{
  rect myrect;
  rect_set(&myrect, left, right, top, bottom);
  *preturnint = inside_box(x, y, myrect);
  log_debug("Inbox is int is %d and %d.  Nlist got %d.", *preturnint, x, y);
}

void dc_random(int script, int* yield, int* preturnint,
	       int range, int base)
{
  *preturnint = (rand() % range) + base;
}

void dc_initfont(int script, int* yield, int* preturnint,
		 char* fontname)
{
  initfont(fontname);
  log_info("Initted font %s", fontname);
}

void dc_set_mode(int script, int* yield, int* preturnint,
		 int new_mode)
{
  set_mode(new_mode);
  *preturnint = mode;
}

void dc_kill_shadow(int script, int* yield, int* preturnint,
		    int sprite)
{
  /* STOP_IF_BAD_SPRITE(sprite); */
  int jj;
  for (jj = 1; jj <= last_sprite_created; jj++)
    {
      if (spr[jj].brain == 15 && spr[jj].brain_parm == sprite)
	{
      lsm_remove_sprite(jj);
	}
    }
}

void dc_create_sprite(int script, int* yield, int* preturnint,
		      int x, int y, int brain, int sequence, int frame)
{
  *preturnint = add_sprite_dumb(x, y, brain, sequence, frame, 100/*size*/);
}

void dc_sp(int script, int* yield, int* preturnint,
	   int editor_sprite)
{
  int i = find_sprite(editor_sprite);
  if (i != 0) {
    log_debug("Sp returned %d.", i);
    *preturnint = i;
    return;
  }
  if (last_sprite_created == 1)
    log_warn("you can't call SP() from a screen-ref,"
	     " no sprites have been created yet.");
  *preturnint = 0; /* not found */
}

void dc_is_script_attached(int script, int* yield, int* preturnint,
			   int sprite)
{
  STOP_IF_BAD_SPRITE(sprite);
  *preturnint = spr[sprite].script;
}

void dc_get_sprite_with_this_brain(int script, int* yield, int* preturnint,
				   int brain, int sprite_ignore)
{
  int i;
  for (i = 1; i <= last_sprite_created; i++)
    {
      if (spr[i].brain == brain && i != sprite_ignore && spr[i].active)
	{
	  log_debug("Ok, sprite with brain %d is %d", brain, i);
	  *preturnint = i;
	  return;
	}
    }
  *preturnint = 0; /* not found */
}

void dc_get_rand_sprite_with_this_brain(int script, int* yield, int* preturnint,
					int brain, int sprite_ignore)
{
  int i;
  int nb_matches = 0;
  for (i = 1; i <= last_sprite_created; i++)
    {
      if (spr[i].brain == brain && i != sprite_ignore && spr[i].active)
	nb_matches++;
    }
  if (nb_matches == 0)
    {
      log_debug("Get rand brain can't find any brains with %d.", brain);
      *preturnint = 0;
      return;
    }
  
  int mypick = (rand() % nb_matches) + 1;
  int ii;
  int cur_match = 0;
  for (ii = 1; ii <= last_sprite_created; ii++)
    {
      if (spr[ii].brain == brain && ii != sprite_ignore && spr[ii].active)
	{
	  cur_match++;
	  if (cur_match == mypick)
	    {
	      *preturnint = ii;
	      return;
	    }
	}
    }
  *preturnint = 0; /* not found */
}

/* BIG FAT WARNING: in DinkC, buttons are in [1, 10] (not [0, 9]) */
void dc_set_button(int script, int* yield, int* preturnint,
		   int button, int function)
{
  input_set_button_action(button-1, function);
}

void dc_hurt(int script, int* yield, int* preturnint,
	     int sprite, int damage)
{
  STOP_IF_BAD_SPRITE(sprite);

  if (dversion >= 108)
    {
      // With v1.07 hurt(&sthing, -1) would run hit(), with v1.08 it
      // doesn't (after redink1 tried to fix a game freeze bug that I
      // can't reproduce)
      if (damage < 0)
	return;
    }

  if (hurt_thing(sprite, damage, 0) > 0)
    random_blood(spr[sprite].x, spr[sprite].y-40, sprite);

  if (spr[sprite].nohit != 1
      && spr[sprite].script != 0
      && locate(spr[sprite].script, "HIT"))
    {
      if (rinfo[script]->sprite != 1000)
	{
	  *penemy_sprite = rinfo[script]->sprite;
	  //redink1 addition of missle_source stuff
	  if (dversion >= 108)
	    *pmissle_source = rinfo[script]->sprite;
	}
      kill_returning_stuff(spr[sprite].script);
      run_script(spr[sprite].script);
    }
}

void dc_screenlock(int script, int* yield, int* preturnint,
		   int param)
{
  if (dversion >= 108)
    {
      // returns the screenlock value to DinkC
      if (param == 0 || param == 1)
	screenlock = param;
      *preturnint = screenlock;
      /* Note: redink1's v1.08 always set returnint, even if too many
	 parameters were passed. Since this breaks the logic of DinkC
	 interpreter clarification (return a variable value when bad
	 parameters), we won't reproduce this particular bug
	 here. AFAICS no D-Mod abused 'screenlock' this way. */
    }
  else
    {
      screenlock = param;
    }
}


/****************/
/*  v1.08-only  */
/*              */
/****************/

void dc_sp_blood_num(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].bloodnum);
  *preturnint = spr[sprite].bloodseq;
}

void dc_sp_blood_seq(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].bloodseq);
  *preturnint = spr[sprite].bloodseq;
}

void dc_sp_clip_bottom(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.bottom);
  *preturnint = spr[sprite].alt.bottom;
}

void dc_sp_clip_left(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.left);
  *preturnint = spr[sprite].alt.left;
}

void dc_sp_clip_right(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.right);
  *preturnint = spr[sprite].alt.right;
}

void dc_sp_clip_top(int script, int* yield, int* preturnint, int sprite, int sparg)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  change_sprite (sprite, sparg, &spr[sprite].alt.top);
  *preturnint = spr[sprite].alt.top;
}

void dc_sp_custom(int script, int* yield, int* preturnint, char* key, int sprite, int val)
{
  RETURN_NEG_IF_BAD_SPRITE(sprite);
  if (!spr[sprite].active)
    {
      *preturnint = -1;
    }
  else
    {
      // If key doesn't exist, create it.
      if (spr[sprite].custom->find(key) == spr[sprite].custom->end())
	spr[sprite].custom->insert(std::make_pair(key, 0));

      // Set the value
      if (val != -1) {
	spr[sprite].custom->erase(key);
	spr[sprite].custom->insert(std::make_pair(key, val));
      }

      *preturnint = spr[sprite].custom->find(key)->second;
    }
}


/**
 * Like sp_mx but use change_sprite_noreturn, so allow setting the
 * value to -1.
 */
void dc_sp_move_x(int script, int* yield, int* preturnint, int sprite, int dx)
  {
    STOP_IF_BAD_SPRITE(sprite);
    change_sprite_noreturn (sprite, dx, &spr[sprite].mx);
  }

/**
 * Like sp_my but use change_sprite_noreturn, so allow setting the
 * value to -1.
 */
void dc_sp_move_y(int script, int* yield, int* preturnint, int sprite, int dy)
  {
    STOP_IF_BAD_SPRITE(sprite);
    change_sprite_noreturn (sprite, dy, &spr[sprite].my);
  }

void dc_sp_freeze(int script, int* yield, int* preturnint, int sprite, int frozen_p)
{
  STOP_IF_BAD_SPRITE(sprite);
  // Set the value
  if (frozen_p == 0)
    spr[sprite].freeze = 0;
  else if (frozen_p == 1)
    spr[sprite].freeze = script;
  /* else -> invalid value */

  // Return the (normalized) value
  *preturnint = (spr[sprite].freeze > 0);
}


void dc_clear_editor_info(int script, int* yield, int* preturnint)
{
    int i;
    for (i = 0; i < 769; i++)
      {
	int j;
	for (j = 0; j < 100; j++)
	  {
	    play.spmap[i].seq[j] = 0;
	    play.spmap[i].frame[j] = 0;
	    play.spmap[i].type[j] = 0;
	    play.spmap[i].last_time = 0;
	  }
      }
    *preturnint = 1;
}

void dc_get_date_day(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%d", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_date_month(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%m", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_date_year(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%Y", time_now);
    *preturnint = atoi (mytime);
}

void dc_get_time_game(int script, int* yield, int* preturnint)
{
    time_t ct;
    time (&ct);
    *preturnint = play.minutes + (difftime (ct, time_start) / 60);
}

void dc_get_time_real(int script, int* yield, int* preturnint)
{
    char mytime[5];
    time_t ct;
    struct tm* time_now;
    time (&ct);
    time_now = localtime (&ct);
    strftime (mytime, 5, "%M", time_now);
    *preturnint = atoi (mytime);
    strftime (mytime, 5, "%H", time_now);
    *preturnint += 60 * atoi (mytime);
}

void dc_get_truecolor(int script, int* yield, int* preturnint)
{
    *preturnint = truecolor;
}

void dc_show_console(int script, int* yield, int* preturnint)
{
  dinkc_console_show();
}

void dc_show_inventory(int script, int* yield, int* preturnint)
{
    show_inventory = 1;
}

void dc_var_used(int script, int* yield, int* preturnint)
{
    int m = 0;
    int i;
    for (i = 1; i < MAX_VARS; i++)
      if (play.var[i].active == 1)
	m++;
    *preturnint = m;
}


void dc_loopmidi(int script, int* yield, int* preturnint, int loop_midi)
{
  loopmidi(loop_midi);
}


void dc_math_abs(int script, int* yield, int* preturnint, int val)
{
  *preturnint = abs(val);
}

void dc_math_sqrt(int script, int* yield, int* preturnint, int val)
{
  *preturnint = sqrt(abs(val));
}

void dc_math_mod(int script, int* yield, int* preturnint, int val, int div)
{
  *preturnint = (val % div);
}

void dc_make_global_function(int script, int* yield, int* preturnint, char* dcscript, char* procname)
{
  make_function(dcscript, procname);
}

void dc_set_save_game_info(int script, int* yield, int* preturnint, char* info)
{
  strncpy(save_game_info, info, LEN_SAVE_GAME_INFO);
  save_game_info[LEN_SAVE_GAME_INFO - 1] = '\0';
}

void dc_load_map(int script, int* yield, int* preturnint, char* mapdat_file, char* dinkdat_file)
{
  // load a new map/dink.dat
  g_dmod.map = EditorMap(dinkdat_file, mapdat_file);
  g_dmod.map.load();
}

void dc_load_tile(int script, int* yield, int* preturnint, char* tileset_file, int tileset_index)
{
  // load new tiles
  if (tileset_index >= 1 && tileset_index <= GFX_TILES_NB_SETS)
    {
      //Load in the new tiles...
      g_dmod.bgTilesets.loadSlot(tileset_index, tileset_file);
      
      //Store in save game
      strncpy(play.tile[tileset_index].file, tileset_file, 50);
    }
  else
    {
      log_error("[DinkC] %s:%d:%s: dc_load_tile: invalid tileset index '%d'",
		rinfo[script]->name, rinfo[script]->debug_line, cur_funcname,
		tileset_index);
    }
}

void dc_map_tile(int script, int* yield, int* preturnint, int tile_position, int tile_index)
{
  // developers can change or see what tile is at any given position
  // Yeah... they can only modify valid tiles
  if (tile_position >= 1 && tile_position <= 96)
    {
      int max = GFX_TILES_NB_SQUARES - 1;

      if (tile_index >= 0 && tile_index <= max)
	cur_ed_screen.t[tile_position - 1].square_full_idx0 = tile_index;
      else
	log_error("[DinkC] %s:%d:%s: dc_map_tile: invalid tile index '%d'",
		  rinfo[script]->name, rinfo[script]->debug_line, cur_funcname,
		  tile_index);

      *preturnint = cur_ed_screen.t[tile_position - 1].square_full_idx0;
    }
}

void dc_map_hard_tile(int script, int* yield, int* preturnint, int tile_position, int hard_tile_index)
{
  // developers can retrieve/modify a hard tile
  // Yeah... they can only modify valid tiles
  if (tile_position >= 1 && tile_position <= 96)
    {
      //Only change the value if it is greater than 0...
      if (hard_tile_index > 0)
	cur_ed_screen.t[tile_position - 1].althard = hard_tile_index;
      *preturnint = cur_ed_screen.t[tile_position - 1].althard;
    }
}


void dc_load_palette(int script, int* yield, int* preturnint, char* bmp_file)
{
  // load a palette from any bmp
  if (gfx_palette_set_from_bmp(bmp_file) < 0)
    log_error("[DinkC] Couldn't load palette from '%s': %s", bmp_file, SDL_GetError());
  gfx_palette_get_phys(GFX_ref_pal);
  
  //Store in save game
  strncpy(play.palette, bmp_file, 50);
}

void dc_get_item(int script, int* yield, int* preturnint, char* dcscript)
{
  // get index of specified item
  *preturnint = 0;
  {
    int i = 0;
    for (; i < NB_ITEMS; i++)
      {
	if (play.item[i].active
	    && compare(play.item[i].name, dcscript))
	  {
	    *preturnint = i + 1;
	    break;
	  }
      }
  }
}

void dc_get_magic(int script, int* yield, int* preturnint, char* dcscript)
{
  // get index of specified magic spell
  *preturnint = 0;
  {
    int i = 0;
    for (; i < NB_MITEMS; i++)
      {
	if (play.mitem[i].active
	    && compare(play.mitem[i].name, dcscript))
	  {
	    *preturnint = i + 1;
	    break;
	  }
      }
  }
}

void dc_set_font_color(int script, int* yield, int* preturnint, int index, int r, int g, int b)
{
  // sets font color
  set_font_color(index, r, g, b);
}

void dc_get_next_sprite_with_this_brain(int script, int* yield, int* preturnint,
					int brain, int sprite_ignore, int sprite_start_with)
{
  // make Paul Pliska's life more fulfilling
  {
    int i = sprite_start_with;
    for (; i <= last_sprite_created; i++)
      {
	if ((spr[i].brain == brain) && (i != sprite_ignore))
	  if (spr[i].active)
	    {
	      log_debug("Ok, sprite with brain %d is %d", brain, i);
	      *preturnint = i;
	      return;
	    }
      }
  }
  log_debug("Ok, sprite with brain %d is 0", brain);
  *preturnint = 0; /* not found */
}

void dc_set_smooth_follow(int script, int* yield, int* preturnint, int smooth_p)
{
  if (smooth_p == 0)
    smooth_follow = 0;
  else if (smooth_p == 1)
    smooth_follow = 1;
}
void dc_set_dink_base_push(int script, int* yield, int* preturnint, int base_sequence)
{
  dink_base_push = base_sequence;
}

void dc_callback_kill(int script, int* yield, int* preturnint, int callback_index)
{
  log_debug("setting callback random");
  kill_callback(callback_index);
}


/**
 * DinkC variable binding
 */
void attach(void)
{
  /* Make sure the "system" variable exists - otherwise we might use a
     NULL pointer below */
  char* var_names[22] = { "&life", "&vision", "&result", "&speed",
		     "&timing", "&lifemax", "&exp", "&strength",
		     "&defense", "&gold", "&magic", "&level",
		     "&player_map", "&cur_weapon", "&cur_magic",
		     "&last_text", "&magic_level", "&update_status",
		     "&missile_target", "&enemy_sprite", "&magic_cost",
		     "&missle_source" };
  int n, i;
  for (n = 0; n < 22; n++)
    {
      if (!lookup_var(var_names[n], DINKC_GLOBAL_SCOPE))
	make_int(var_names[n], 0, DINKC_GLOBAL_SCOPE, -1);
      /* TODO: setting script to -1 is asking for troubles... */
    }

  for (i = 1; i < MAX_VARS; i++)
    {
      if (compare("&life", play.var[i].name)) plife = &play.var[i].var;
      if (compare("&vision", play.var[i].name)) pvision = &play.var[i].var;
      if (compare("&result", play.var[i].name)) presult = &play.var[i].var;
      if (compare("&speed", play.var[i].name)) pspeed = &play.var[i].var;
      if (compare("&timing", play.var[i].name))	ptiming = &play.var[i].var;
      if (compare("&lifemax", play.var[i].name)) plifemax = &play.var[i].var;
      if (compare("&exp", play.var[i].name)) pexp = &play.var[i].var;
      if (compare("&strength", play.var[i].name))  pstrength = &play.var[i].var;
      if (compare("&defense", play.var[i].name))  pdefense = &play.var[i].var;
      if (compare("&gold", play.var[i].name))  pgold = &play.var[i].var;
      if (compare("&magic", play.var[i].name))  pmagic = &play.var[i].var;
      if (compare("&level", play.var[i].name))  plevel = &play.var[i].var;
      if (compare("&player_map", play.var[i].name)) pplayer_map = &play.var[i].var;
      if (compare("&cur_weapon", play.var[i].name)) pcur_weapon = &play.var[i].var;
      if (compare("&cur_magic", play.var[i].name)) pcur_magic = &play.var[i].var;
      if (compare("&last_text", play.var[i].name)) plast_text = &play.var[i].var;
      if (compare("&magic_level", play.var[i].name)) pmagic_level = &play.var[i].var;
      if (compare("&update_status", play.var[i].name)) pupdate_status = &play.var[i].var;
      if (compare("&missile_target", play.var[i].name)) pmissile_target = &play.var[i].var;
      if (compare("&enemy_sprite", play.var[i].name)) penemy_sprite = &play.var[i].var;
      if (compare("&magic_cost", play.var[i].name)) pmagic_cost = &play.var[i].var;
      if (compare("&missle_source", play.var[i].name)) pmissle_source = &play.var[i].var;
    }
}


/**
 * Add a DinkC binding
 * 
 * Simple macro to allow using struct initializer e.g. {2,1,1,0....}
 * when declaring a DinkC function.
 */
#define DCBD_ADD(name, ...)                                 \
{                                                           \
  struct binding bd = { #name, (void*)dc_ ## name, __VA_ARGS__ }; \
  dinkc_bindings_add(bindings, &bd);                        \
}
/**
 * Map DinkC functions to C functions, with their arguments
 */
void dinkc_bindings_init()
{
  /* funcname, params, badparams_dcps, badparams_returnint_p, badparams_returnint */

  DCBD_ADD(sp_active,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_hit_sound,       {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_hit_sound_speed, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_attack_wait,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_attack,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_die,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_hit,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_idle,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_base_walk,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain_parm,             {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_brain_parm2,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_defense,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_dir,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_disabled,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_distance,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_exp,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_flying,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_follow,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_frame,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_frame_delay,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_gold,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_hard,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_hitpoints,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_move_nohard,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_mx,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_my,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_noclip,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nocontrol,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nodraw,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_nohit,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_notouch,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_pframe,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_picfreeze,              {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_pseq,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_que,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_range,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_reverse,                {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_seq,                    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_size,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_sound,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_speed,                  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_strength,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_target,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_timing,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_touch_damage,           {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_x,                      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_y,                      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(sp_kill,                   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_editor_num,             {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_kill_wait,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(sp_script,                 {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  /* sp_base_death is an alias for sp_base_die */
  struct binding bd_sp_base_death = *dinkc_bindings_lookup(bindings, "sp_base_die");
  bd_sp_base_death.funcname = "sp_base_death";
  dinkc_bindings_add(bindings, &bd_sp_base_death);

  DCBD_ADD(unfreeze,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(freeze,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_callback_random,   {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_dink_speed,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(reset_timer,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(set_keep_mouse,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_item,              {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_magic,             {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(add_exp,               {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_item,        {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_magic,       {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(show_bmp,              {2,1,1,0,0,0,0,0,0,0}, DCPS_YIELD        , 0, 0);
  DCBD_ADD(copy_bmp_to_screen,    {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(wait_for_button,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(stop_wait_for_button, {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(draw_screen,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(free_items,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(free_magic,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(kill_cur_item,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(kill_cur_magic,       {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(draw_status,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(arm_weapon,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(arm_magic,            {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(load_screen,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(say,                   {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop,              {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop_npc,          {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_stop_xy,           {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(say_xy,                {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(restart_game,         {-1,0,0,0,0,0,0,0,0,0}, DCPS_CONTINUE     , 0, 0);
  DCBD_ADD(wait,                  {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(preload_seq,           {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(script_attach,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_hard_sprite,      {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(activate_bow,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(disable_all_sprites, {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_background,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(draw_hard_map,       {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(enable_all_sprites,  {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fade_down,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fade_up,             {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_burn,            {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_last_bow_power,  {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_version,         {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_all_sounds,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_game,           {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_this_task,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(scripts_used,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(stopcd,              {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(stopmidi,            {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(turn_midi_off,       {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(turn_midi_on,        {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(count_item,               {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(count_magic,              {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(compare_sprite_script,    {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(compare_weapon,           {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(compare_magic,            {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(init,                     {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(dink_can_walk_off_screen, {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(push_active,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(stop_entire_game,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(editor_type,  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(editor_seq,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(editor_frame, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  DCBD_ADD(move,                 {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(spawn,                {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(run_script_by_number, {1,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(playmidi,             {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(playsound,            {1,1,1,1,1,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(sound_set_survive,    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(sound_set_vol,        {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(sound_set_kill,       {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(save_game,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(force_vision,              {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(fill_screen,               {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(load_game,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(game_exist,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(move_stop,                 {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(load_sound,                {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(debug,                     {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(busy,                      {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

  DCBD_ADD(make_global_int,            {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(inside_box,                 {1,1,1,1,1,1,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(random,                     {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(initfont,                   {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(set_mode,                   {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(kill_shadow,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(create_sprite,              {1,1,1,1,1,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(sp,                         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(is_script_attached,         {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
  DCBD_ADD(get_sprite_with_this_brain,      {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(get_rand_sprite_with_this_brain, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
  DCBD_ADD(set_button,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(hurt,                       {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
  DCBD_ADD(screenlock,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

  if (dversion >= 108)
    {
      DCBD_ADD(sp_blood_num,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_blood_seq,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_bottom, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_left,   {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_right,  {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(sp_clip_top,    {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

      DCBD_ADD(sp_custom,      {2,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);

      DCBD_ADD(sp_move_x, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(sp_move_y, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(sp_freeze, {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);


      DCBD_ADD(clear_editor_info, {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_day,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_month,    {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_date_year,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_time_game,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_time_real,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_truecolor,     {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(show_console,      {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(show_inventory,    {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(var_used,          {-1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

      DCBD_ADD(loopmidi,           {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);

      DCBD_ADD(math_abs,                 {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(math_sqrt,                {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(math_mod,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(make_global_function,     {2,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(set_save_game_info,       {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(load_map,                 {2,2,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(load_tile,                {2,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(map_tile,                 {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(map_hard_tile,            {1,1,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(load_palette,             {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_item,                 {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(get_magic,                {2,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(set_font_color,           {1,1,1,1,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(set_smooth_follow,        {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, -1);
      DCBD_ADD(set_dink_base_push,       {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(callback_kill,            {1,0,0,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 0, 0);
      DCBD_ADD(get_next_sprite_with_this_brain,  {1,1,1,0,0,0,0,0,0,0}, DCPS_GOTO_NEXTLINE, 1, 0);
    }
}

void dinkc_bindings_quit()
{
  if (bindings != NULL)
    hash_free(bindings);
  bindings = NULL;
}
