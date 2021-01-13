#include "ImageLoader.h"

#include "SDL.h"
#include "SDL_image.h"
#include "log.h"

/* Reference palette: this is the canonical Dink palette, loaded from
   TS01.bmp (for freedink) and esplash.bmp (for freedinkedit). The
   physical screen may be changed (e.g. show_bmp()), but this
   canonical palette will stay constant. */
SDL_Color GFX_ref_pal[256];
/* Reference blit surface that all others should mimic to avoid palette dithering;
 * GFX_ref_pal may change, but not this surface's palette */
// TODO: make IOGfxDisplaySW->blit *not* do any palette conversion, so we can get rid of this global
SDL_Surface* ImageLoader::blitFormat = NULL;

void ImageLoader::initBlitFormat(Uint32 format) {
	Uint32 Rmask=0, Gmask=0, Bmask=0, Amask=0; int bpp=0;
	SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
	ImageLoader::blitFormat = SDL_CreateRGBSurface(0, 1, 1, bpp,
			Rmask, Gmask, Bmask, Amask);
	if (format == SDL_PIXELFORMAT_INDEX8) {
		SDL_SetPaletteColors(ImageLoader::blitFormat->format->palette, GFX_ref_pal, 0, 256);
	}
}

/**
 * Load image to the current display format (indexed or truecolor)
 * This isn't centralized in display->upload() because dir.ff are handled differently,
 * and this is done independently of the target display renderer.
 */
SDL_Surface* ImageLoader::loadToBlitFormat(FILE* in) {
	if (in == NULL)
		return NULL;

	SDL_RWops* rw = SDL_RWFromFP(in, /*autoclose=*/SDL_TRUE);
	SDL_Surface* image = IMG_Load_RW(rw, 1);
	if (blitFormat->format->format == SDL_PIXELFORMAT_INDEX8)
		image = convertToPaletteFormat(image);

	/* Disable alpha in 32bit BMPs, like the original engine */
	SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_NONE);

	return image;
}

/**
 * Make a copy of the surface using the screen format: same
 * palette (with dithering), and same color depth (needed when
 * importing 24bit graphics in 8bit mode).
 */
SDL_Surface* ImageLoader::convertToPaletteFormat(SDL_Surface* image) {
	SDL_Surface *converted = SDL_CreateRGBSurface(0, image->w, image->h, 8, 0,0,0,0);
	SDL_SetPaletteColors(converted->format->palette, GFX_ref_pal, 0, 256);
	SDL_BlitSurface(image, NULL, converted, NULL);
	Uint32 key;
	if (SDL_GetColorKey(image, &key) == 0)
		SDL_SetColorKey(converted, SDL_TRUE, key);
	SDL_FreeSurface(image);
	/* Disable palette conversion in future blits */
	SDL_SetPaletteColors(converted->format->palette, blitFormat->format->palette->colors, 0, 256);
	return converted;
}

SDL_Surface* ImageLoader::newWithBlitFormat(int w, int h) {
	SDL_Surface* image;
	if (blitFormat->format->format == SDL_PIXELFORMAT_INDEX8) {
		image = SDL_CreateRGBSurface(0, w, h, 8, 0,0,0,0);
		if (image == NULL) {
			log_error("ImageLoader::newWithPaletteFormat: %s", SDL_GetError());
			return NULL;
		}
		/* Disable palette conversion in future blits */
		SDL_SetPaletteColors(image->format->palette, blitFormat->format->palette->colors, 0, 256);
	} else {
		Uint32 Rmask=0, Gmask=0, Bmask=0, Amask=0; int bpp=0;
		SDL_PixelFormatEnumToMasks(blitFormat->format->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
		image = SDL_CreateRGBSurface(0, w, h, bpp, Rmask, Gmask, Bmask, Amask);
	}
	return image;
}
