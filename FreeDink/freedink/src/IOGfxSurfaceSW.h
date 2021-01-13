#ifndef IOGFXSURFACESW_H
#define IOGFXSURFACESW_H

#include "IOGfxSurface.h"
#include "SDL.h"

class IOGfxSurfaceSW: public IOGfxSurface {
public:
	SDL_Surface* image;
	IOGfxSurfaceSW(SDL_Surface* image);
	virtual ~IOGfxSurfaceSW();
	virtual void fill_screen(int num, SDL_Color* palette);
	virtual int fillRect(const SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b);
	virtual int blit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect);
	virtual int blitStretch(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect);
	virtual int blitNoColorKey(IOGfxSurface *src, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	virtual SDL_Surface* screenshot();
	virtual unsigned int getMemUsage();
};

#endif
