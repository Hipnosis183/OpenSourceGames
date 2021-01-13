
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BgTilesetsManager.h"
#include "paths.h"
#include "log.h"
#include "gfx.h"
#include "ImageLoader.h"
#include "IOGfxDisplay.h"
#include "SDL_image.h"

BgTilesetsManager::BgTilesetsManager() {
	memset(&slots, 0, sizeof(slots));
}

BgTilesetsManager::~BgTilesetsManager() {
	unloadAll();
}

// Load the tiles from the BMPs
void BgTilesetsManager::loadDefault() {
  char crap[30];
  char crap1[10];
  int h;

  log_info("loading tilescreens...");
  for (h = 1; h <= GFX_TILES_NB_SETS; h++)
    {
      if (h < 10)
	strcpy(crap1,"0");
      else
	strcpy(crap1, "");      
      sprintf(crap, "tiles/Ts%s%d.BMP", crap1, h);

      loadSlot(h, crap);

      if (slots[h] == NULL)
	exit(0);
    }
  
  log_info("Done with tilescreens...");
}

void BgTilesetsManager::loadSlot(int slot, char* relpath) {
	FILE* in = paths_dmodfile_fopen(relpath, "rb");
	if (in == NULL)
		in = paths_fallbackfile_fopen(relpath, "rb");

	if (slots[slot] != NULL) {
		delete slots[slot];
		slots[slot] = NULL;
	}

	SDL_Surface* image = ImageLoader::loadToBlitFormat(in);
	slots[slot] = g_display->upload(image);

	/* Note: attempting SDL_RLEACCEL showed no improvement for the
	   memory usage, including when using a transparent color and
	   blitting the surface once. It did show a decrease of 400kB (out
	   of 6000kB) when using transparent color 255, but in this case the
	   color is not supposed to be transparent. */

	if (slots[slot] == NULL)
		log_error("Couldn't find tilescreen %s: %s\n", relpath, SDL_GetError());
}

/**
 * Free memory used by tiles
 */
void BgTilesetsManager::unloadAll() {
	int h = 0;
	for (h=1; h <= GFX_TILES_NB_SETS; h++) {
		if (slots[h] != NULL)
			delete slots[h];
		slots[h] = NULL;
	}
}

int BgTilesetsManager::getMemUsage() {
	int sum = 0;
	int i = 0;
	IOGfxSurface* s = NULL;
	for (; i < GFX_TILES_NB_SETS+1; i++) {
		s = slots[i];
		if (s != NULL)
			sum += s->getMemUsage();
	}
	return sum;
}
