/**
 * Texture handler for OpenGL (ES) 2

 * Copyright (C) 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public
 * License along with GNU FreeDink.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef IOGFXSURFACEGL2_H
#define IOGFXSURFACEGL2_H

#include "IOGfxSurface.h"

#include "SDL.h"

#include "IOGfxGLFuncs.h"

class IOGfxDisplayGL2;
class IOGfxSurfaceGL2: public IOGfxSurface {
public:
	IOGfxDisplayGL2* display;
	GLuint texture;
	SDL_Color colorkey;
	GLuint fbo;

	IOGfxSurfaceGL2(IOGfxDisplayGL2* display, GLuint texture, int w, int h, SDL_Color colorkey);
	virtual ~IOGfxSurfaceGL2();

	virtual void fill_screen(int num, SDL_Color* palette);
	virtual int fillRect(const SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b);
	virtual int blit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect);
	virtual int blitStretch(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect);
	virtual int blitNoColorKey(IOGfxSurface *src, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	virtual SDL_Surface* screenshot();
	virtual unsigned int getMemUsage();

	bool bindAsFramebuffer();
	int internalBlit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect, bool useColorKey);
};

#endif
