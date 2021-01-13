#ifndef FAKEIOGFXDISPLAY_H
#define FAKEIOGFXDISPLAY_H

#include "IOGfxDisplay.h"

class FakeIOGfxDisplay: public IOGfxDisplay {
public:
	FakeIOGfxDisplay(int w, int h, bool truecolor, Uint32 flags);
	virtual void clear();
	virtual void flip(IOGfxSurface* backbuffer, SDL_Rect* dstrect,
					  bool interpolation, bool hwflip);
	virtual void onSizeChange(int w, int h);
	virtual IOGfxSurface* upload(SDL_Surface* s);
	virtual IOGfxSurface* allocBuffer(int surfW, int surfH);
	virtual SDL_Surface* screenshot(SDL_Rect* rect);
};

#endif
