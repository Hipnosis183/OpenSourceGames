#ifndef IOGFXDISPLAYSW_H
#define IOGFXDISPLAYSW_H

#include "IOGfxDisplay.h"

class IOGfxDisplaySW: public IOGfxDisplay {
private:
	SDL_Renderer* renderer;
	/* Streaming texture to push software buffer -> hardware */
	SDL_Texture* render_texture_linear;
	/* Non-interpolated texture for printing raw textures during tests screenshots */
	SDL_Texture* render_texture_nearest;
	/* Intermediary texture to convert 8bit->24bit in non-truecolor */
	SDL_Surface *rgb_screen;

public:
	IOGfxDisplaySW(int w, int h, bool truecolor, Uint32 flags);
	virtual ~IOGfxDisplaySW();

	virtual bool open();
	virtual void close();
	virtual void logDisplayInfo();

	virtual void clear();
	virtual void onSizeChange(int w, int h);
	virtual IOGfxSurface* upload(SDL_Surface* s);
	virtual IOGfxSurface* allocBuffer(int surfW, int surfH);
	virtual void flip(IOGfxSurface* backbuffer, SDL_Rect* dstrect,
					  bool interpolation, bool hwflip);

	virtual SDL_Surface* screenshot(SDL_Rect* rect);

	bool createRenderer();
	void logRenderersInfo();

	bool createRenderTexture(int w, int h);
	void logRenderTextureInfo();
};

#endif
