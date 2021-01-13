#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "brain.h"
#include "live_sprites_manager.h"

void text_brain(int h)
{
	if (!lsm_isValidSprite(h)
		|| (spr[h].text_owner != 0 && spr[h].text_owner != 1000 && !lsm_isValidSprite(spr[h].text_owner))) {
		lsm_remove_sprite(h);
		return;
	}

	// Select text type
	// Note: scripters can make hybrids, e.g. narrator (owner=1000) sprite that moves
	if ((spr[h].damage == -1) && (spr[h].text_owner != 1000)) {
		// Normal text sprite

		// Kill text if owner was killed
		if (!spr[spr[h].text_owner].active) {
			lsm_remove_sprite(h);
			return;
		}
		//give this text the cords from it's owner sprite
		// strength/defense = text sprite center
		spr[h].x = spr[spr[h].text_owner].x - spr[h].strength;
		spr[h].y = spr[spr[h].text_owner].y - spr[h].defense;
		
		if (spr[h].x < 1) spr[h].x = 1;
		if (spr[h].y < 1) spr[h].y = 1;
	} else if (spr[h].move_active) {
		// damage/experience gain (damage=0)
		process_move(h);
	} else {
		// Narrator text sprite (owner=1000)
		automove(h);
	}
}
