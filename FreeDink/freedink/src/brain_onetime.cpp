#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "live_sprites_manager.h"
#include "gfx.h"
#include "live_screen.h" /* draw_sprite_game */

void one_time_brain(int h)
{
	
	//goes once then draws last frame to background
	
	if (spr[h].move_active) 
	{
		process_move(h);
		return;
	}
	
	if (spr[h].follow > 0)
	{
		process_follow(h);
	}
	
	
	if (spr[h].seq == 0)
	{
	  draw_sprite_game(IOGFX_background, h);
		lsm_remove_sprite(h);
		return;
	}
	
	changedir(spr[h].dir,h,-1);
	automove(h);
	
}

void one_time_brain_for_real(int h)
{
	
	if (spr[h].move_active) 
	{
		process_move(h);
	}
	
	
	if (spr[h].follow > 0)
	{
		process_follow(h);
	}
	
	
	if (spr[h].seq == 0)
	{
		
		lsm_remove_sprite(h);
		return;
	}
	if (spr[h].dir > 0)
	{
		changedir(spr[h].dir,h,-1);
		automove(h);
	}
}
