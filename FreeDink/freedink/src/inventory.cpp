/**
 * Game inventory

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

#include "inventory.h"
#include "game_engine.h"
#include "log.h"
#include "str_util.h" /* compare */
#include "IOGfxDisplay.h"
#include "gfx.h" /* GFX_lpDDSBack */
#include "gfx_sprites.h" /* draw_status_all, GFX_k */
#include "dinkini.h" /* check_seq_status */
#include "game_choice.h" /* talk.active */
#include "input.h"
#include "sfx.h"
#include "update_frame.h" /* talk_process */
#include "status.h" /* draw_status_all */

/* Blinking selector in the inventory screen */
static Uint32 item_timer;
static int item_pic;

/* Used by dinkc_bindings.c only */
void add_item(char* name, int mseq, int mframe, enum item_type type)
{
  if (type == ITEM_REGULAR)
    {
      //add reg item
      int i;
      for (i = 0; i < NB_ITEMS; i++)
	{
	  if (play.item[i].active == 0)
	    {
	      log_info("Weapon/item %s added to inventory.", name);
	      play.item[i].seq = mseq;
	      play.item[i].frame = mframe;
	      strncpy(play.item[i].name, name, sizeof(play.item[i].name));
	      play.item[i].name[sizeof(play.item[i].name)-1] = '\0';
	      play.item[i].active = 1;
	      
	      int crap1 = load_script(play.item[i].name, 1000);
	      if (locate(crap1, "PICKUP"))
		run_script(crap1);
	      
	      break;
	    }
	}
    }
  else
    {
      //add magic item
      int i;
      for (i = 0; i < NB_MITEMS; i++)
	{
	  if (play.mitem[i].active == 0)
	    {
	      log_info("Magic %s added to inventory.", name);
	      play.mitem[i].seq = mseq;
	      play.mitem[i].frame = mframe;
	      strncpy(play.mitem[i].name, name, sizeof(play.mitem[i].name));
	      play.mitem[i].name[sizeof(play.mitem[i].name)-1] = '\0';
	      play.mitem[i].active = 1;
	      
	      int crap = load_script(play.mitem[i].name, 1000);
	      if (locate(crap, "PICKUP"))
		run_script(crap);
	      
	      break;
	    }
	}
    }
}

/* Used by dinkc_bindings.c only */
void kill_cur_item()
{
  if (*pcur_weapon >= 1 && *pcur_weapon <= NB_ITEMS)
    {
      if (play.item[*pcur_weapon - 1].active == 1)
	{
	  if (weapon_script != 0 && locate(weapon_script, "DISARM"))
	    run_script(weapon_script);
	  weapon_script = load_script(play.item[*pcur_weapon - 1].name, 0);
	  play.item[*pcur_weapon - 1].active = 0;
	  *pcur_weapon = 0;
	  if (weapon_script != 0 && locate(weapon_script, "HOLDINGDROP"))
	    run_script(weapon_script);
	  if (weapon_script != 0 && locate(weapon_script, "DROP"))
	    run_script(weapon_script);
	  weapon_script = 0;
	}
      else
	{
	  log_error("Can't kill cur item, none armed.");
	}
    }
}

/* Used by dinkc_bindings.c only */
void kill_item_script(char* name)
{
  int select = 0;
  {
    int i = 0;
    for (; i < NB_ITEMS; i++)
      {
	if (play.item[i].active)
	  if (compare(play.item[i].name, name))
	    {
	      select = i;
	      goto found;
	    }
      }
  }
  return;

 found:
  if (*pcur_weapon - 1 == select)
    {
      //holding it right now
      if (locate(weapon_script, "HOLDINGDROP"))
	run_script(weapon_script);
      if (locate(weapon_script, "DISARM"))
	run_script(weapon_script);
      
      *pcur_weapon = 0;
      weapon_script = 0;
    }

  int script = load_script(play.item[select].name, 0);
  play.item[select].active = /*false*/0;

  if (locate(script, "DROP"))
    run_script(script);
  
  draw_status_all();
}

/* Used by dinkc_bindings.c only */
void kill_mitem_script(char* name)
{
  int select = 0;
  {
    int i = 0;
    for (; i < NB_MITEMS; i++)
      {
	if (play.mitem[i].active)
	  if (compare(play.mitem[i].name, name))
	    {
	      select = i;
	      goto found;
	    }
      }
  }
  return;

 found:
  if (*pcur_magic - 1 == select)
    {
      //holding it right now
      if (locate(magic_script, "HOLDINGDROP"))
	run_script(magic_script);
      if (locate(magic_script, "DISARM"))
	run_script(magic_script);

      // TODO: this should be *pcur_magic; keeping for compatibility
      // for now:
      *pcur_weapon = 0;
      magic_script = 0;
    }
  
  int script = load_script(play.mitem[select].name, 0);
  play.mitem[select].active = 0;

  if (locate(script, "DROP"))
    run_script(script);

  draw_status_all();
}

/* Used by dinkc_bindings.c only */
void kill_cur_magic()
{
  if (*pcur_magic >= 1 && *pcur_magic <= NB_MITEMS)
    {
      if (play.mitem[*pcur_magic - 1].active == 1)
	{
	  if (magic_script != 0 && locate(magic_script, "DISARM"))
	    run_script(magic_script);
	  magic_script = load_script(play.mitem[*pcur_magic - 1].name, 0);
	  play.mitem[*pcur_magic - 1].active = /*false*/0;
	  *pcur_magic = 0;
	  
	  if (magic_script != 0 && locate(magic_script, "HOLDINGDROP"))
	    run_script(magic_script);
	  if (magic_script != 0 && locate(magic_script, "DROP"))
	    run_script(magic_script);
	  magic_script = 0;
	}
      else
	{
	  log_error("Can't kill cur magic, none armed.");
	}
    }
}



/**
 * Draw an item icon (or an item selection square) in the inventory
 * screen
 */
static void draw_item(int item_idx0, enum item_type type, int mseq, int mframe)
{
  int mx = 0;
  int my = 0;

  if (type == ITEM_REGULAR)
    {
      mx = 260;
      my = 83;
      
      mx += (item_idx0 % 4) * (18 + 65);
      my += (item_idx0 / 4) * (20 + 55);
    }
  else
    {
      mx = 45;
      my = 83;
      
      mx += (item_idx0 % 2) * (18 + 65);
      my += (item_idx0 / 2) * (20 + 55);
    }
	
  check_seq_status(mseq);
	
  if (GFX_k[seq[mseq].frame[mframe]].k == NULL) 
    {
      if (type == ITEM_REGULAR)
	{
	  log_debug("Whups, item %d seq %d frame %d not loaded, killed it",
		    item_idx0, mseq, mframe);
	  play.item[item_idx0].active = 0;
	}
      else
	{
	  log_debug("Whups, magic %d seq %d frame %d not loaded, killed it",
		    item_idx0, mseq, mframe);
	  play.mitem[item_idx0].active = 0;
	}
      return;
    }
  SDL_Rect dst;
  dst.x = mx; dst.y = my;
  IOGFX_backbuffer->blit(GFX_k[seq[mseq].frame[mframe]].k, NULL, &dst);
}


/**
 * Inventory screen mode
 */
void process_item()
{
  IOGFX_backbuffer->blit(IOGFX_background, NULL, NULL);
	
  check_seq_status(423);
  //lets blit the main screen over it
  SDL_Rect dst = {20, 0};
  IOGFX_backbuffer->blit(GFX_k[seq[423].frame[1]].k, NULL, &dst);

  //draw all currently owned items; magic
  {
    int i = 0;
    for (; i < NB_MITEMS; i++)
      if (play.mitem[i].active)
	draw_item(i, ITEM_MAGIC, play.mitem[i].seq, play.mitem[i].frame);
  }
  //draw selection box around armed magic
  if (*pcur_magic >= 1 && *pcur_magic <= NB_MITEMS && play.item[*pcur_magic - 1].active)
    draw_item(*pcur_magic - 1, ITEM_MAGIC, 423, 5);
  

  //draw all currently owned items; normal
  {
    int i = 0;
    for (; i < NB_ITEMS; i++)
      if (play.item[i].active)
	draw_item(i, ITEM_REGULAR, play.item[i].seq, play.item[i].frame);
  }
  //draw selection box around armed weapon
  if (*pcur_weapon >= 1 && *pcur_weapon <= NB_ITEMS && play.item[*pcur_weapon - 1].active)
    draw_item(*pcur_weapon - 1, ITEM_REGULAR, 423, 4);
  
  
  if (play.curitem < 0
      || (play.item_type == ITEM_REGULAR && play.curitem >= NB_ITEMS)
      || (play.item_type == ITEM_MAGIC && play.curitem >= NB_MITEMS))
    play.curitem = 0;

  if (thisTickCount > item_timer)
    {
      //draw the selector around it, alternating from 2 to 3
      if (item_pic == 2)
	item_pic = 3;
      else
	item_pic = 2;
      item_timer = thisTickCount + 400;
    }
  draw_item(play.curitem, play.item_type, 423, item_pic);
  
  if (play.item_type == ITEM_REGULAR)
    {
      int hor  = play.curitem % 4;
      int vert = play.curitem / 4;
			
      //choosing weapon/item
      if (sjoy.button[ACTION_ATTACK])
	{
	  if (play.item[play.curitem].active)
	    {
	      //arm weapon
	      SoundPlayEffect(18, 42050,0,0,0);
	      if (*pcur_weapon != 0)
		{
		  //disarm old weapon
		  if (locate(weapon_script, "DISARM"))
		    run_script(weapon_script);
		}

	      //load weapons script
	      *pcur_weapon = play.curitem + 1;
	      weapon_script = load_script(play.item[*pcur_weapon - 1].name, 1000);
	      if (locate(weapon_script, "ARM"))
		run_script(weapon_script);
	      if (locate(weapon_script, "ARMMOVIE"))
		run_script(weapon_script);
	      draw_status_all();
	    }
	}
      else if (sjoy.rightd) 
	{
	  if (hor < 3)
	    {
	      play.curitem++;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
      else if (sjoy.leftd) 
	{
	  if (hor > 0)
	    {
	      play.curitem--; 
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	  else
	    {
	      SoundPlayEffect(11, 22050,0,0,0);
	      //switch to magic mode
	      play.item_type = ITEM_MAGIC;
	      play.curitem = vert * 2 + 1;
	    }
	}
      else if (sjoy.downd)
	{
	  if (vert < 3)
	    {
	      play.curitem += 4;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
      else if (sjoy.upd)
	{
	  if (vert > 0)
	    {
	      play.curitem -= 4;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
    }
  else		
    {
      int hor  = play.curitem % 2;
      int vert = play.curitem / 2;
      
      if (sjoy.button[ACTION_ATTACK])
	{
	  if (play.mitem[play.curitem].active)
	    {
	      //arm magic
	      SoundPlayEffect(18, 42050,0,0,0);
	      if (*pcur_magic != 0)
		{
		  //disarm old weapon
		  if (locate(magic_script, "DISARM"))
		    run_script(magic_script);
		}
	      //load magics script
	      *pcur_magic = play.curitem + 1;
	      magic_script = load_script(play.mitem[*pcur_magic - 1].name, 1000);
	      if (locate(magic_script, "ARM"))
		run_script(magic_script);
	      if (locate(magic_script, "ARMMOVIE"))
		run_script(magic_script);
	      draw_status_all();
	    }
	}
      else if (sjoy.rightd) 
	{
	  if (hor < 1)
	    {
	      play.curitem++;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	  else
	    { 
	      play.item_type = ITEM_REGULAR;
	      play.curitem = vert * 4;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
      else if (sjoy.leftd) 
	{
	  if (hor > 0)
	    {
	      play.curitem--;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
      else if (sjoy.downd)
	{
	  if (vert < 3)
	    {
	      play.curitem += 2;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
      else if (sjoy.upd)
	{
	  if (vert > 0) 
	    {
	      play.curitem -= 2;
	      SoundPlayEffect(11, 22050,0,0,0);
	    }
	}
    }
  
  //a special process callbacks for just stuff that was created in this mode? 
  // process_callbacks_special();

  if (sjoy.button[ACTION_INVENTORY])
    {
      SoundPlayEffect(17, 22050,0,0,0);
      show_inventory = 0;
    }
}
