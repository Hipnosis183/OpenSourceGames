#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "freedink.h"
#include "live_sprites_manager.h"
#include "game_engine.h"
#include "live_screen.h"
#include "log.h"

void find_action(int h)
{
	
	spr[h].action = (rand() % 2)+1;
	
	
	if (spr[h].action == 1)
	{
		//sit and think
		spr[h].move_wait = thisTickCount +((rand() % 3000)+400);
		if (spr[h].base_walk != -1)
		{
			int dir = (rand() % 4)+1;  
			
			spr[h].pframe = 1;
			if (dir == 1)  spr[h].pseq = spr[h].base_walk+1;
			if (dir == 2)  spr[h].pseq = spr[h].base_walk+3;
			if (dir == 3)  spr[h].pseq = spr[h].base_walk+7;
			if (dir == 4)  spr[h].pseq = spr[h].base_walk+9;
		}
		
		return;
	}
	
	if (spr[h].action == 2)
	{
		//move
		spr[h].move_wait = thisTickCount +((rand() % 3000)+500);
		int dir = (rand() % 4)+1;  
		spr[h].pframe = 1;
		if (dir == 1)  changedir(1,h,spr[h].base_walk);
		if (dir == 2)  changedir(3,h,spr[h].base_walk);
		if (dir == 3)  changedir(7,h,spr[h].base_walk);
		if (dir == 4)  changedir(9,h,spr[h].base_walk);
		return;
	}
	
	
	log_error("Internal error:  Brain 16, unknown action.");
}


void people_brain(int h)
{
	if  (spr[h].damage > 0)
	{
		//got hit
		//SoundPlayEffect( 1,3000, 800 );
		if (spr[h].hitpoints > 0)
		{
			draw_damage(h);
			if (spr[h].damage > spr[h].hitpoints) spr[h].damage = spr[h].hitpoints;
			spr[h].hitpoints -= spr[h].damage;
			
			if (spr[h].hitpoints < 1)
			{
				//they killed it
				check_for_kill_script(h);
				
				if (spr[h].brain == 16)
				{
					if (spr[h].dir == 0) spr[h].dir = 3;
					spr[h].brain = 0;
					change_dir_to_diag(&spr[h].dir);
					add_kill_sprite(h);
					lsm_remove_sprite(h);
				}
				return;
				
			}
		}
		spr[h].damage = 0;
		
	}
	
	if (spr[h].move_active) 
	{
		process_move(h);
		return;
	}
	
	
	
	
	if (spr[h].freeze) return;
	
	if (spr[h].follow > 0)
	{
		process_follow(h);
		return;
	}
	
	
	if ((spr[h].move_wait < thisTickCount) && (spr[h].seq == 0))
	{
		
		spr[h].action = 0;
	}
	
	
	
	if (spr[h].action == 0) find_action(h);
	
	
	if (spr[h].action != 2) 
	{
		spr[h].seq = 0;
		return;
		
	}
	if (spr[h].seq_orig != 0)
		if (spr[h].seq == 0) spr[h].seq = spr[h].seq_orig;
		
		
		if (spr[h].y > playy)
			
		{
			
			if ( ((rand() % 2)+1) == 1)
				changedir(9,h,spr[h].base_walk);
			else changedir(7,h,spr[h].base_walk);
			
			
		}         
		
		if (spr[h].x > playx)
			
		{
			if ( ((rand() % 2)+1) == 1)
				changedir(1,h,spr[h].base_walk);
			else changedir(7,h,spr[h].base_walk);
			
		}         
		
		if (spr[h].y < 20)
		{
			if ( ((rand() % 2)+1) == 1)
				changedir(1,h,spr[h].base_walk);
			else changedir(3,h,spr[h].base_walk);
		}         
		
		if (spr[h].x < 30) 
		{
			if ( ((rand() % 2)+1) == 1)
				changedir(3,h,spr[h].base_walk);
			else changedir(9,h,spr[h].base_walk);
		}         
		
		automove(h);
		
		if (check_if_move_is_legal(h) != 0)
		{
			if ((rand() % 3) == 2)
			{
				changedir(autoreverse_diag(h),h,spr[h].base_walk);
				
			} else
			{
				spr[h].move_wait = 0;
				spr[h].pframe = 1;
				spr[h].seq = 0;
			}
		}
		
		
		//				changedir(hold,h,spr[h].base_walk);
		
		
}
