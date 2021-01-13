
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "live_sprites_manager.h"
#include "freedink.h"
#include "game_engine.h"
#include "gfx.h"
#include "gfx_sprites.h"
#include "sfx.h"
#include "log.h"

void make_missile(int x1, int y1, int dir, int speed, int seq, int frame, int strength)
{
	int crap = add_sprite(x1,y1,11,seq,frame);
	spr[crap].speed = speed;
	spr[crap].seq = seq;
	spr[crap].timing = 0;
	spr[crap].strength = strength;
	spr[crap].flying = /*true*/1;
	changedir(dir, crap, 430);
	
}

void missile_brain(int h, /*bool*/int repeat)
{
  rect box;
  int j;
  automove(h);
  
  *pmissle_source = h;
  int hard = check_if_move_is_legal(h);
  if (repeat && spr[h].seq == 0)
    spr[h].seq = spr[h].seq_orig;
  spr[1].hitpoints = *plife; 
  
  if (hard > 0 && hard != 2) 
    {      
      //lets check to see if they hit a sprites hardness
      if (hard > 100)
	{
	  int ii;
	  for (ii = 1; ii < last_sprite_created; ii++)
	    {
	      if (spr[ii].sp_index == hard-100)
		{
		  if (spr[ii].script > 0)
		    {
		      *pmissile_target = 1;
		      *penemy_sprite = 1;
		      
		      if (locate(spr[ii].script, "HIT"))
			{
			  kill_returning_stuff(spr[ii].script);
			  run_script(spr[ii].script);
			}
		    }
		  
		  if (spr[h].script > 0)
		    {
		      *pmissile_target = ii;
		      *penemy_sprite = 1;
		      if (locate(spr[h].script, "DAMAGE")) 
			{
			  kill_returning_stuff(spr[h].script);
			  run_script(spr[h].script);
			}
		    }
		  else
		    {
		      if (spr[h].attack_hit_sound == 0)
			SoundPlayEffect( 9,22050, 0 ,0,0);
		      else
			SoundPlayEffect( spr[h].attack_hit_sound,spr[h].attack_hit_sound_speed, 0 ,0,0);
		      
		      lsm_remove_sprite(h);
		    }
		  
		  //run missile end	
		  return;
		}
	    }
	}
      //run missile end	
      
      if (spr[h].script > 0)
	{
	  *pmissile_target = 0;
	  if (locate(spr[h].script, "DAMAGE"))
	    run_script(spr[h].script);
	}
      else
	{
	  if (spr[h].attack_hit_sound == 0)
	    SoundPlayEffect(9, 22050, 0, 0, 0);
	  else
	    SoundPlayEffect(spr[h].attack_hit_sound,spr[h].attack_hit_sound_speed, 0, 0, 0);
	  
	  lsm_remove_sprite(h);
	  return;
	}
    }
  
  if (spr[h].x > 1000) lsm_remove_sprite(h);
  if (spr[h].y > 700) lsm_remove_sprite(h);
  if (spr[h].y < -500) lsm_remove_sprite(h);
  if (spr[h].x < -500) lsm_remove_sprite(h);
  
  //did we hit anything that can die?
  
  for (j = 1; j <= last_sprite_created; j++)
    {
      if (spr[j].active && h != j && spr[j].nohit != 1 && spr[j].notouch == /*false*/0)
	if (spr[h].brain_parm != j && spr[h].brain_parm2!= j)
	  //if (spr[j].brain != 15) if (spr[j].brain != 11)
	  {
	    rect_copy(&box, &k[getpic(j)].hardbox);
	    rect_offset(&box, spr[j].x, spr[j].y);
	    
	    if (spr[h].range != 0)
	      rect_inflate(&box, spr[h].range,spr[h].range);
	    
	    if (debug_mode) {
	    	SDL_Rect r = {box.left, box.top, box.right-box.left, box.bottom-box.top};
	    	IOGFX_backbuffer->fillRect(&r, 222, 173, 255);
	    }
	    
	    if (inside_box(spr[h].x, spr[h].y, box))
	      {
		spr[j].notouch = /*true*/1;
		spr[j].notouch_timer = thisTickCount+100;
		spr[j].target = 1;
		*penemy_sprite = 1;
		//change later to reflect REAL target
		if (spr[h].script > 0)
		  {
		    *pmissile_target = j;
		    if (locate(spr[h].script, "DAMAGE"))
		      run_script(spr[h].script);
		  }
		else
		  {
		    if (spr[h].attack_hit_sound == 0)
		      SoundPlayEffect(9, 22050, 0, 0, 0);
		    else
		      SoundPlayEffect(spr[h].attack_hit_sound,spr[h].attack_hit_sound_speed, 0, 0,0);
		  }
		
		if (spr[j].hitpoints > 0 && spr[h].strength != 0)
		  {
		    int hit = 0;
		    if (spr[h].strength == 1)
		      hit = spr[h].strength - spr[j].defense;
		    else
		      hit = (spr[h].strength / 2)
			+ ((rand() % (spr[h].strength / 2)) + 1)
			- spr[j].defense;
		    
		    if (hit < 0)
		      hit = 0;
		    spr[j].damage += hit;
		    if (hit > 0)
		      random_blood(spr[j].x, spr[j].y-40, j);
		    spr[j].last_hit = 1;
		    //Msg("Damage done is %d..", spr[j].damage);
		  }
		
		if (spr[j].script > 0)
		  {
		    //CHANGED did = h
		    *pmissile_target = 1;
		    
		    if (locate(spr[j].script, "HIT"))
		      {
			kill_returning_stuff(spr[j].script);
			run_script(spr[j].script);
		      }
		  }
	      }
	    //run missile end	
	    
	  }
    }
}


void missile_brain_expire(int h)
{
	missile_brain(h, /*false*/0);
	if (spr[h].seq == 0)
		lsm_remove_sprite(h);
	
}
