#ifndef IOGFXDISPLAY_H
#define IOGFXDISPLAY_H

#include "SDL.h"

#include "IOGfxSurface.h"

class IOGfxDisplay {
public:
	int w, h;
	bool truecolor;
	Uint32 flags;
	bool initializedVideo;
	SDL_Window* window;
	/* True color fade in [0,256]; 0 is completely dark, 256 is unaltered */
	double brightness; // TODO: move to int or unsigned short

public:
	IOGfxDisplay(int w, int h, bool truecolor, Uint32 flags);
	virtual ~IOGfxDisplay();

	virtual bool open();
	virtual void close();
	virtual void logDisplayInfo();

	virtual Uint32 getFormat();

	virtual void clear() = 0;
	/* Refresh the physical screen, applying brightness and palette */
	virtual void flip(IOGfxSurface* backbuffer, SDL_Rect* dstrect,
					  bool interpolation, bool hwflip) = 0;
	virtual void onSizeChange(int w, int h) = 0;
	virtual IOGfxSurface* upload(SDL_Surface* s) = 0;
	virtual IOGfxSurface* allocBuffer(int surfW, int surfH) = 0;

	virtual void flipStretch(IOGfxSurface* backbuffer);
	virtual void flipDebug(IOGfxSurface* backbuffer);

	virtual void surfToDisplayCoords(IOGfxSurface* backbuffer, int &x, int &y);
	virtual void centerScaledSurface(IOGfxSurface* surf, SDL_Rect* dst);

	virtual SDL_Surface* screenshot(SDL_Rect* rect) = 0;
	virtual SDL_Surface* screenshot();
	void screenshot(const char* output_file);

	bool createWindow();
	void logWindowInfo();
	void toggleFullScreen();
	void toggleScreenKeyboard();
};

extern void flip_it(void);

#endif
