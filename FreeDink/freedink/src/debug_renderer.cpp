#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "debug_renderer.h"
#include "debug.h"
#include "game_state.h"
#include "game_engine.h"
#include "live_sprites_manager.h"
#include "input.h"
#include "gfx_fonts.h"
#include "log.h"

void debug_renderer_render() {
	if (debug_mode)
		{
			char msg[100];
			FONTS_SetTextColor(200, 200, 200);
			if (mode == 0) strcpy(msg,"");
			if (mode == 1)
				{
					int x;
					sprintf(msg,"X is %d y is %d  FPS: %d", spr[1].x, spr[1].y, fps);
					//let's add the key info to it.
					for (x = 0; x < 256; x++)
						if (input_getcharstate(x))
							sprintf(msg + strlen(msg), " (Key %i)", x);
				}
			if (mode == 3)
				{
					sprintf(msg, "Sprites: %d  FPS: %d"
							"  Moveman X%d X%d: %d Y%d Y%d Map %d",
							last_sprite_created, fps/*_show*/,
							spr[1].lpx[0], spr[1].lpy[0], spr[1].moveman,
							spr[1].lpx[1], spr[1].lpy[1], *pplayer_map);
				}
	    
			print_text_wrap_debug(msg, 0, 0);
			const char* lastLog = log_getLastLog();
			if (strlen(lastLog) > 0)
				{
					//let's also draw this...
					print_text_wrap_debug(lastLog, 0, 20);
				}
		}
}
