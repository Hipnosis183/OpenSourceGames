#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "live_sprites_manager.h"
#include "game_engine.h" /* base_timing */

void scale_brain(int h)
{
	
	if (spr[h].size == spr[h].brain_parm)
	{
		lsm_remove_sprite(h);
		
		
		return;
	}
	
	int num = 5 * (base_timing / 4);
	
	
	
	if (spr[h].size > spr[h].brain_parm)
	{
		if (spr[h].size - num < spr[h].brain_parm) num = spr[h].size - spr[h].brain_parm;
		spr[h].size -= num;
	}
	
	if (spr[h].size < spr[h].brain_parm) 
	{
		if (spr[h].size + num > spr[h].brain_parm) num = spr[h].brain_parm - spr[h].size;   
		spr[h].size += num;
	}
	if (spr[h].move_active) 
	{
		process_move(h);
		return;
	}
	
	
	
	if (spr[h].dir > 0)
	{
		changedir(spr[h].dir,h,-1);
		automove(h);
	}
}
