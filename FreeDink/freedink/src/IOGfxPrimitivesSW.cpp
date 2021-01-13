#include "SDL.h"
#include "SDL_image.h"
#include "SDL2_rotozoom.h"

#include "log.h"
#include "paths.h"
#include "io_util.h"

#include "gfx.h"
#include "gfx_palette.h"
#include "ImageLoader.h"

/* Used to implement DinkC's copy_bmp_to_screen(). Difference with
   show_cmp: does not set showb.* (wait for button), install the image
   to lpDDSTwo (background) and not lpDDSBack (screen double
   buffer) */
void copy_bmp(char* name) {
	char* fullpath = paths_dmodfile(name);
	SDL_Surface* image = IMG_Load(fullpath);
	if (image == NULL) {
		log_error("Couldn't load '%s': %s", name, SDL_GetError());
		return;
	}

	/* Set physical screen palette */
	if (!truecolor) {
		gfx_palette_set_from_surface(image);

		/* In case the DX bug messed the palette, let's convert the
		   image to the new palette. This also converts 24->8bit if
		   necessary. */
		SDL_PixelFormat* fmt = gfx_palette_get_phys_format();
		SDL_Surface *converted = SDL_ConvertSurface(image, fmt, 0);
		SDL_FreeFormat(fmt);
		SDL_FreeSurface(image);
		image = converted;

		/* Next blit without palette conversion */
		SDL_SetPaletteColors(image->format->palette,
				ImageLoader::blitFormat->format->palette->colors, 0, 256);
	}
	IOGfxSurface* surf = g_display->upload(image);
	IOGFX_background->blit(surf, NULL, NULL);
	delete surf;

	abort_this_flip = /*true*/1;
}

void show_bmp(char* name, int script) {
	char* fullpath = paths_dmodfile(name);
	SDL_Surface* image = IMG_Load(fullpath);
	if (image == NULL) {
		log_error("Couldn't load '%s': %s", name, SDL_GetError());
		return;
	}

	/* Set physical screen palette */
	if (!truecolor) {
		gfx_palette_set_from_surface(image);

		/* In case the DX bug messed the palette, let's convert the
		   image to the new palette. This also converts 24->8bit if
		   necessary. */
		SDL_PixelFormat* fmt = gfx_palette_get_phys_format();
		SDL_Surface *converted = SDL_ConvertSurface(image, fmt, 0);
		SDL_FreeFormat(fmt);
		SDL_FreeSurface(image);
		image = converted;
		/* Disable palette conversion in future blits */
		SDL_SetPaletteColors(image->format->palette,
				ImageLoader::blitFormat->format->palette->colors, 0, 256);
	}
	IOGfxSurface* surf = g_display->upload(image);
	IOGFX_tmp1->blit(surf, NULL, NULL);
	delete surf;

	// After show_bmp(), and before the flip_it() call in updateFrame(),
	// other parts of the code will draw sprites on lpDDSBack and mess
	// the showbmp(). So skip the next flip_it().
	abort_this_flip = /*true*/1;
}
