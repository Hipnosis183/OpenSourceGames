
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"

#include <gettext.h>
#define _(String) gettext(String)

#include "game_engine.h"
#include "freedink.h"
#include "game_engine.h"
#include "input.h"
#include "live_sprites_manager.h"
#include "sfx.h"
#include "game_choice.h"
#include "gfx_sprites.h"
#include "text.h"
#include "bgm.h"
#include "live_screen.h"

/**
 * Check whether planned new position (x1,y1) is solid
 * 
 * If hard value is > 100, warp using info from editor sprite #(100-value)
 * 
 * TODO: factor out with 'get_hard'; beware of 'return' semantic
 */
unsigned char get_hard_play(int x1, int y1)
{
  x1 -= 20;

  if (screenlock)
    {
      if (x1 < 0)        x1 = 0;
      else if (x1 > 599) x1 = 599;

      if (y1 < 0)        y1 = 0;
      else if (y1 > 399) y1 = 399;
    }
  if ((x1 < 0) || (y1 < 0) || (x1 > 599) || (y1 > 399))
    return 0;

  int value =  screen_hitmap[x1][y1];
  if (value > 100 && cur_ed_screen.sprite[value-100].is_warp != 0)
    {
      warp_editor_sprite = value;
      value = 0;
    }
  return(value);
}

void run_through_tag_list_push(int h)
{
	rect box;
	int i;

	for (i = 1; i <= last_sprite_created; i++)
	{
		if (spr[i].active) if (i != h) if
			((spr[i].script != 0) )
		{
			
			rect_copy(&box, &k[getpic(i)].hardbox);
			rect_offset(&box, spr[i].x, spr[i].y);
			
			//InflateRect(&box, 10,10);
			
			box.right += 2;
			box.left -= 2;
			box.top -= 2;
			box.bottom += 2;
			//draw_box(box, 33);
			
			if (inside_box(spr[h].x, spr[h].y, box))
			{	
				if (locate(spr[i].script, "PUSH")) run_script(spr[i].script);
			}
			
		}
		
	}
	
}

void process_bow( int h)
{
	int timetowait = 100;
	
	
	if (bow.wait < thisTickCount)
	{
		if (sjoy.right) spr[h].dir = 6;
		if (sjoy.left) spr[h].dir = 4;
		if (sjoy.up) spr[h].dir = 8;
		if (sjoy.down) spr[h].dir = 2;
	}
	
	
	
	if (sjoy.right) if (sjoy.up) 
	{
		spr[h].dir = 9;
		bow.wait = thisTickCount + timetowait;
	}
	if (sjoy.left) if (sjoy.up) 
	{
		spr[h].dir = 7;
		bow.wait = thisTickCount + timetowait;
	}
	if (sjoy.right) if (sjoy.down) 
	{
		spr[h].dir = 3;
		bow.wait = thisTickCount + timetowait;
		
	}
	if (sjoy.left) if (sjoy.down) 
	{
		spr[h].dir = 1;
		bow.wait = thisTickCount + timetowait;
		
	}
	spr[h].pseq = 100+spr[h].dir;
	
	
	if (bow.pull_wait < thisTickCount)
	{
		bow.pull_wait = thisTickCount + 10;
		if (bow.hitme) bow.time += 7;
		
		
		//	bowsound->SetFrequency(22050+(bow.time*10));
		
		if (bow.time > 500) bow.time = 500;
		spr[h].pframe = (bow.time / 100)+1;
	}
	
	
	if (!sjoy.joybitold[ACTION_ATTACK])
	{
		bow.active = /*false*/0;
		bow.last_power = bow.time;
		run_script(bow.script);
		//     bowsound->Stop();
		return;
	}
	
}


/**
 * Player
 */
void human_brain(int h)
{
  int diag;
  int crap;
  /*BOOL*/int bad;
  
  if (mode == 0)
    goto b1end;
  
  if (spr[h].move_active)
    {
      process_move(h);
      return;
    }
	
  if (spr[h].damage > 0)
    {
      draw_damage(h);
      
      *plife -= spr[h].damage;
      
      spr[h].damage = 0;
      if (*plife < 0)
	*plife = 0;
		
      int hurt = (rand() % 2)+1;
		
      if (hurt == 1)
	SoundPlayEffect(15, 25050, 2000, 0,0);
      if (hurt == 2)
	SoundPlayEffect(16, 25050, 2000, 0,0);
		
      //draw blood
    }
  
  if (play.push_active)
    {
      if (play.push_dir == 2 && !sjoy.down) 
	{
	  spr[h].nocontrol = /*false*/0;
	  play.push_active = false;
	}
      
      if (play.push_dir == 4 && !sjoy.left) 
	{
	  spr[h].nocontrol = /*false*/0;
	  play.push_active = false;
	}
      if (play.push_dir == 6 && !sjoy.right) 
	{
	  spr[h].nocontrol = /*false*/0;
	  play.push_active = false;
	}
      
      if (play.push_dir == 8 && !sjoy.up) 
	{
	  spr[h].nocontrol = /*false*/0;
	  play.push_active = false;
	}
    }
  
  if (spr[h].nocontrol)
    return;
	
  if (game_choice.active)
    goto freeze;
	
  if (spr[h].freeze)
    {
      //they are frozen
      if (sjoy.button[ACTION_TALK] == 1)
	{
	  //they hit the talk button while frozen, lets hurry up the process
	  int jj;
	  for (jj = 1; jj <= last_sprite_created; jj++)
	    {
	      // Msg("Checking %d, brain %d, script %d, my freeze is %d",jj, spr[jj].brain, spr[jj].script, spr[h].freeze);
	      if (spr[jj].brain == 8 && spr[jj].script == play.last_talk)
		{
		  //this sprite owns its freeze
		  spr[jj].kill_ttl = 1;
		  //force the message to be over
		}
	    }
	}
      goto freeze;
    }


  //******************************  KEYS THAT CAN BE PRESSED AT ANY TIME **************
  
  if (bow.active)
    {
      //bow is active!!
      process_bow(h);
      return;
    }

  if (play.push_active && thisTickCount > play.push_timer + 600)
    {
      spr[h].seq = dink_base_push + spr[h].dir;
      spr[h].frame = 1;
      spr[h].nocontrol = /*true*/1;
      //play.push_active = /*false*/0;
      run_through_tag_list_push(h);
      
      return;
    }
  
  if ((sjoy.button[ACTION_TALK] == 1))
    {
      if (!run_through_tag_list_talk(h))
	{
	  int did_dnotalk = 0;
	  if (dversion >= 108)
	    {
	      // addition of 'not talking to anything' script
	      int sc = load_script ("dnotalk", 0);
	      if (sc != 0 && locate (sc, "MAIN"))
		{
		  run_script (sc);
		  did_dnotalk = 1;
		}
	    }

	  if (did_dnotalk == 0)
	    {
	      kill_text_owned_by(h);	
	      int randy = (rand() % 6)+1;
	      if (randy == 1) say_text(_("`$I don't see anything here."), h, 0);
	      if (randy == 2) say_text(_("`$Huh?"), h, 0);
	      if (randy == 3) say_text(_("`$I'm fairly sure I can't talk to or use that."), h, 0);
	      if (randy == 4) say_text(_("`$What?"), h, 0);
	      if (randy == 5) say_text(_("`$I'm bored."), h, 0);
	      if (randy == 6) say_text(_("`$Not much happening here."), h, 0);
	    }
	}
    }
	
  if ((sjoy.button[ACTION_ATTACK] == 1) && (weapon_script != 0))
    {
      if (spr[h].base_hit > 0)
	{
	  if (locate(weapon_script, "USE"))
	    run_script(weapon_script);
	  goto b1end;
	}
    }
  
  //added AGAIN 10-19-99
  //Let's check keys for getting hit
  if (thisTickCount > but_timer)
    {
      int scancode;
      for (scancode = 0; scancode < SDL_NUM_SCANCODES; scancode++)
	{ 
	  if (scancode == SDL_SCANCODE_SPACE
	      || scancode == SDL_SCANCODE_6
	      || scancode == SDL_SCANCODE_7
	      || scancode == SDL_SCANCODE_LEFT
	      || scancode == SDL_SCANCODE_UP
	      || scancode == SDL_SCANCODE_RIGHT
	      || scancode == SDL_SCANCODE_DOWN
	      || scancode == SDL_SCANCODE_RETURN
	      || scancode == SDL_SCANCODE_TAB
	      || scancode == SDL_SCANCODE_ESCAPE
	      || scancode == SDL_SCANCODE_LCTRL
	      || scancode == SDL_SCANCODE_RCTRL
	      || scancode == SDL_SCANCODE_LSHIFT
	      || scancode == SDL_SCANCODE_RSHIFT
	      || scancode == SDL_SCANCODE_LALT
	      || scancode == SDL_SCANCODE_RALT
	      || SDL_GetKeyFromScancode((SDL_Scancode)scancode) == SDLK_m)
	    continue;
	  
	  char scriptname[30];
	  if (input_getscancodestate((SDL_Scancode)scancode))
	    {
	      /* Get the same keycodes than the original Dink engines
		 for letters, that is, uppercase ascii rather than
		 lowercase ascii */
	      int code;
	      int keycode = SDL_GetKeyFromScancode((SDL_Scancode)scancode);
	      if (keycode >= SDLK_a && keycode <= SDLK_z)
		code = 'A' + (keycode - SDLK_a);
	      else
		code = scancode;
	      
	      sprintf(scriptname, "key-%d", code);
	      but_timer = thisTickCount+200;
	      
	      int script = load_script(scriptname, 1);
	      if (locate(script, "MAIN"))
		{
		  run_script(script);
		  goto b1end;
		}
	    }
	}
    }

  {
    enum buttons_actions actions[5];
    int actions_script[5];
    int nb_actions = 1;
    actions[0] = ACTION_MAP;
    actions_script[0] = 6;
    if (dversion >= 108)
      {
	nb_actions = 5;
	actions[1] = ACTION_BUTTON7;
	actions_script[1] = 7;
	actions[2] = ACTION_BUTTON8;
	actions_script[2] = 8;
	actions[3] = ACTION_BUTTON9;
	actions_script[3] = 9;
	actions[4] = ACTION_BUTTON10;
	actions_script[4] = 10;
      }
    for (int i = 0; i < nb_actions; i++)
      {
	// button6.c, button7.c, ..., button10.c
	if (sjoy.button[actions[i]] == 1)
	  {
	    char script_filename[6+2+1]; // 'button' + '7'..'10' + '\0' (no '.c')
	    sprintf(script_filename, "button%d", actions_script[i]);
	    int mycrap = load_script(script_filename, 1);
	    if (locate(mycrap, "MAIN"))
	      run_script(mycrap);
	    goto b1end;
	  }
      }
  }
  
  if (magic_script != 0 && sjoy.joybit[ACTION_MAGIC])
    goto shootm;

  if (sjoy.button[ACTION_MAGIC] == 1)
    {
      if (magic_script == 0)
	{
	  if (dversion >= 108)
	    {
	      // addition of 'no magic' script
	      int sc = load_script ("dnomagic", 0);
	      if (sc != 0 && locate (sc, "MAIN"))
		{
		  run_script (sc);
		  goto b1end;
		}
	    }

	  int randy = (rand() % 6)+1;
	  kill_text_owned_by(h);	
	  if (randy == 1) say_text(_("`$I don't know any magic."), h, 0);
	  if (randy == 2) say_text(_("`$I'm no wizard!"), h, 0);
	  if (randy == 3) say_text(_("`$I need to learn magic before trying this."), h, 0);
	  if (randy == 4) say_text(_("`$I'm gesturing wildly to no avail!"), h, 0);
	  if (randy == 5) say_text(_("`$Nothing happened."), h, 0);
	  if (randy == 6) say_text(_("`$Hocus pocus!"), h, 0);
	  goto b1end;
	}
		
      //player pressed 1
      //lets magiced something
shootm:	
      if (*pmagic_level >= *pmagic_cost)
	{
	  if (locate(magic_script, "USE"))
	    run_script(magic_script);
	  goto b1end;	
	} 
    }
  
  if (sjoy.button[ACTION_INVENTORY])
    {
      if (dversion >= 108)
	{
	  // addition of 'enter key/inventory' script
	  int sc = load_script ("button4", 0);
	  if (sc != 0 && locate (sc, "MAIN"))
	    {
	      run_script (sc);
	      return;
	    }
	}
      
      show_inventory = 1;
      SoundPlayEffect(18, 22050,0,0,0);
      return;
    }
  
  if (sjoy.button[ACTION_MENU] == 1)
    {
      if (!showb.active && !bow.active && !game_choice.active)
	{
	  int sc = load_script("escape", 1000);
	  if (sc != 0 && locate(sc, "MAIN"))
	    run_script(sc);
	  return;
	}
    }
  
      if (input_getcharstate(SDLK_b))
	ResumeMidi();
      if (input_getcharstate(SDLK_n))
	PauseMidi();
  
  if (spr[h].skip > 0
      && spr[h].skip <= spr[h].skiptimer)
    {
      spr[h].skiptimer = 0;
      goto b1end;
    }
  
  
  diag = 0;
  if (sjoy.right) diag++;
  if (sjoy.left) diag++;
  if (sjoy.down) diag++;
  if (sjoy.up) diag++;

  
  //*********************************PROCESS MOVEMENT
  
  if (diag == 1)
    {
      if (sjoy.right)
	{
	  move(h,spr[h].speed,'+','0');
	  changedir(6,h,spr[h].base_walk);
	}
      
      if (sjoy.left) 
	{
	  move(h,spr[h].speed,'-','0');
	  changedir(4,h,spr[h].base_walk);
	}
      
      if (sjoy.down)
	{
	  move(h,spr[h].speed,'0','+');
	  changedir(2,h,spr[h].base_walk);
	}
      
      if (sjoy.up) 
	{
	  move(h,spr[h].speed,'0','-');
	  changedir(8,h,spr[h].base_walk);
	}
    }
  
  // ***************** DIAGONAL!!!!
  if (diag > 1 && diag < 3)
    {
      if (sjoy.up && sjoy.left)
	{
	  changedir(7,h,spr[h].base_walk);
	  move(h,spr[h].speed - (spr[h].speed / 3),'-','-');
	}
      
      if (sjoy.down && sjoy.left)
	{
	  changedir(1,h,spr[h].base_walk);
	  move(h,spr[h].speed - (spr[h].speed / 3),'-','+');
	}
      
      if (sjoy.down && sjoy.right)
	{
	  changedir(3,h,spr[h].base_walk);
	  move(h,spr[h].speed - (spr[h].speed / 3),'+','+');
	}
      
      if (sjoy.up && sjoy.right)
	{
	  changedir(9,h,spr[h].base_walk);
	  move(h,spr[h].speed - (spr[h].speed / 3),'+','-');
	}		
    }
  	
  bad = 0;
  if (sjoy.right) bad = 1;
  if (sjoy.left) bad = 1;
  if (sjoy.up) bad = 1;
  if (sjoy.down) bad = 1;
  
  if (bad)
    {
      if (spr[h].idle)
	{
	  spr[h].frame = 1;
	  spr[h].idle = /*FALSE*/0;
	}
      goto badboy;
    }
		
  if (not_in_this_base(spr[h].seq, spr[h].base_idle)) //unccoment to allow walk anim to end before idle anim to start
    {
    freeze:
      if (spr[h].dir == 1) spr[h].dir = 2;
      if (spr[h].dir == 3) spr[h].dir = 2;
      if (spr[h].dir == 7) spr[h].dir = 8;
      if (spr[h].dir == 9) spr[h].dir = 8;
      
      if (spr[h].base_idle != 0)
	changedir(spr[h].dir,h,spr[h].base_idle);
      spr[h].idle = /*TRUE*/1;   
    }
  
 badboy: 
 b1end:
  
  if (spr[h].dir == 2 || spr[h].dir == 4 || spr[h].dir == 6 || spr[h].dir == 8)
    goto smoothend;

  crap = check_if_move_is_legal(h);
  if (crap != 0)
    {
	  if (crap > 100 && cur_ed_screen.sprite[crap-100].is_warp != 0)
	warp_editor_sprite = crap;
		  
      //hit something, can we move around it?
		  
      if (spr[h].seq == spr[h].base_walk + 4
	  || spr[h].seq == spr[h].base_walk + 6)
	{
	  int hardm = get_hard_play(spr[h].x, spr[h].y-1);
	  if (hardm == 0)
	    spr[h].y -= 1;
	}

      if (spr[h].seq == spr[h].base_walk + 8
	  || spr[h].seq == spr[h].base_walk + 2)
	{
	  int hardm = get_hard_play(spr[h].x-1, spr[h].y);
	  if (hardm == 0)
	    spr[h].x -= 1;
	}


      if (spr[h].seq == spr[h].base_walk + 9)
	{
	  int hardm = get_hard_play(spr[h].x+1, spr[h].y);
	  if (hardm == 0)
	    {  
	      spr[h].x += 1;
	      
	    }
	  else
	    {
	      int hardm = get_hard_play(spr[h].x+1, spr[h].y+1);
	      if (hardm == 0)
		{  
		  spr[h].x += 1;
		  spr[h].y += 1;
		}
	      else
		{
		  int hardm = get_hard_play(spr[h].x+1, spr[h].y+2);
		  if (hardm == 0)
		    {  
		      spr[h].x += 1;
		      spr[h].y += 2;
		    }
		  else
		    {
		      int hardm = get_hard_play(spr[h].x, spr[h].y-1);
		      if (hardm == 0)
			{  
			  spr[h].y -= 1;
			  
			}
		      else
			{
			  int hardm = get_hard_play(spr[h].x-1, spr[h].y-1);
			  if (hardm == 0)
			    {  
			      spr[h].x -= 1;
			      spr[h].y -= 1;
			    }  
			}
		    }
		}
	    }
	}
      
      if (spr[h].seq == spr[h].base_walk + 7)
	{
	  int hardm = get_hard_play(spr[h].x-1, spr[h].y);
	  if (hardm == 0)
	    {  
	      spr[h].x -= 1;
	    }
	  else
	    {
	      int hardm = get_hard_play(spr[h].x-1, spr[h].y+1);
	      if (hardm == 0)
		{  
		  spr[h].x -= 1;
		  spr[h].y += 1;
		}
	      else
		{
		  int hardm = get_hard_play(spr[h].x-1, spr[h].y+2);
		  if (hardm == 0)
		    {  
		      spr[h].x -= 1;
		      spr[h].y += 2;
		    }
		  else
		    {
		      int hardm = get_hard_play(spr[h].x, spr[h].y-1);
		      if (hardm == 0)
			{  				
			  spr[h].y -= 1;
			}
		      else
			{
			  int hardm = get_hard_play(spr[h].x+1, spr[h].y-1);
			  if (hardm == 0)
			    {  				
			      spr[h].x += 1;
			      spr[h].y -= 1;
			    }
			}
		    }
		}
	    }
	}
      
      if (spr[h].seq == spr[h].base_walk + 1)
	{
	  int hardm = get_hard_play(spr[h].x-1, spr[h].y);
	  if (hardm == 0)
	    {  
	      spr[h].x -= 1;
	    }
	  else
	    {
	      int hardm = get_hard_play(spr[h].x-1, spr[h].y-1);
	      if (hardm == 0)
		{  
		  spr[h].x -= 1;
		  spr[h].y -= 1;
		}
	      else
		{
		  int hardm = get_hard_play(spr[h].x-1, spr[h].y-2);
		  if (hardm == 0)
		    {  
		      spr[h].x -= 1;
		      spr[h].y -= 2;
		    }
		  else
		    {
		      int hardm = get_hard_play(spr[h].x, spr[h].y+1);
		      if (hardm == 0)
			{  
			  spr[h].y += 1;
			}
		      else
			{
			  int hardm = get_hard_play(spr[h].x+1, spr[h].y+1);
			  if (hardm == 0)
			    {  
			      spr[h].x += 1;
			      spr[h].y += 1;
			    } 
			}
		    }
		}
	    }
	}
		  
      if (spr[h].seq == spr[h].base_walk + 3)
	{
	  int hardm = get_hard_play(spr[h].x+1, spr[h].y);
	  if (hardm == 0)
	    {  
	      spr[h].x += 1;
	    }
	  else
	    {
	      int hardm = get_hard_play(spr[h].x+1, spr[h].y-1);
	      if (hardm == 0)
		{  
		  spr[h].x += 1;
		  spr[h].y -= 1;
		}
	      else
		{
		  int hardm = get_hard_play(spr[h].x+1, spr[h].y-2);
		  if (hardm == 0)
		    {  
		      spr[h].x += 1;
		      spr[h].y -= 2;
		    }
		  else
		    {
		      int hardm = get_hard_play(spr[h].x, spr[h].y+1);
		      if (hardm == 0)
			{  
			  spr[h].y += 1;
			}
		      else
			{
			  int hardm = get_hard_play(spr[h].x-1, spr[h].y+1);
			  if (hardm == 0)
			    {  
			      spr[h].x -= 1;
			      spr[h].y += 1;
			    }
			}
		    }
		}
	    }
	}
    }

 smoothend:
  ;
}
