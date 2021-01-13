#include "FakeIOGfxDisplay.h"

FakeIOGfxDisplay::FakeIOGfxDisplay(int w, int h, bool truecolor, Uint32 flags) : IOGfxDisplay(w,h,truecolor,flags) {}
void FakeIOGfxDisplay::clear() {}
void FakeIOGfxDisplay::flip(IOGfxSurface* backbuffer, SDL_Rect* dstrect,
							bool interpolation, bool hwflip) {}
void FakeIOGfxDisplay::onSizeChange(int w, int h) {}
IOGfxSurface* FakeIOGfxDisplay::upload(SDL_Surface* s) { return NULL; }
IOGfxSurface* FakeIOGfxDisplay::allocBuffer(int surfW, int surfH) { return NULL; }
SDL_Surface* FakeIOGfxDisplay::screenshot(SDL_Rect* rect) { return NULL; }
