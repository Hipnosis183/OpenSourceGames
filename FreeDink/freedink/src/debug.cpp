#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "debug.h"
#include "SDL.h"

int fps = 0;

static int frames = 0;
static Uint32 fps_lasttick = 0;

void debug_refreshFPS() {
  /* Refresh frame counter twice per second */
  if ((SDL_GetTicks() - fps_lasttick) > 500)
    {
      fps = frames * (1000/500);
      frames = 0;
      fps_lasttick = SDL_GetTicks();
    }
  frames++;
}

void debug_logic() {
	debug_refreshFPS();
}
