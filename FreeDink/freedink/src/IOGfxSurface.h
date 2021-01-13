#ifndef IOGFXSURFACE_H
#define IOGFXSURFACE_H

#include "SDL.h"
#include "rect.h"

class IOGfxSurface {
public:
	int w, h;
	IOGfxSurface(int w, int h);
	virtual ~IOGfxSurface();
	virtual void fill_screen(int num, SDL_Color* palette) = 0;
	virtual int fillRect(const SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b) = 0;
	virtual int blit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect) = 0;
	virtual int blitStretch(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect) = 0;
	virtual int blitNoColorKey(IOGfxSurface *src, const SDL_Rect *srcrect, SDL_Rect *dstrect) = 0;
	virtual SDL_Surface* screenshot() = 0;
	virtual unsigned int getMemUsage() = 0;

	virtual void vlineRGB(Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b);
	virtual void hlineRGB(Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b);
};

#endif
