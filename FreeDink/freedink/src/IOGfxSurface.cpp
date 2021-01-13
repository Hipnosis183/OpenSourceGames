#include "IOGfxSurface.h"

IOGfxSurface::IOGfxSurface(int w, int h)
: w(w), h(h) {
}

IOGfxSurface::~IOGfxSurface() {
}

void IOGfxSurface::vlineRGB(Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Rect dst = { x, y1, 1, y2-y1 };
	fillRect(&dst, r, g, b);
}
void IOGfxSurface::hlineRGB(Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Rect dst = { x1, y, x2-x1, 1 };
	fillRect(&dst, r, g, b);
}
