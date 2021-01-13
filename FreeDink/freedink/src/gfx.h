/**
 * Header for graphics

 * Copyright (C) 2007, 2008, 2009, 2014, 2015  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _GFX_H
#define _GFX_H

#include "SDL.h"
#include "SDL2_framerate.h"
#include "rect.h"
#include "IOGfxSurface.h"
#include "IOGfxDisplaySW.h"

#define GFX_RES_W 640
#define GFX_RES_H 480
#define GFX_PLAY_W 600
#define GFX_PLAY_H 400
#define GFX_PLAY_LEFT 20
#define GFX_PLAY_TOP  0


extern int truecolor;
extern IOGfxDisplay* g_display;
extern IOGfxSurface* IOGFX_backbuffer;
extern IOGfxSurface* IOGFX_tmp1;
extern IOGfxSurface* IOGFX_tmp2;
extern IOGfxSurface* IOGFX_background;

extern /*bool*/int abort_this_flip;

extern Uint32 truecolor_fade_lasttick;


#define FPS 60
extern FPSmanager framerate_manager;


extern int gfx_init(bool dinkgl, bool windowed, char* splash_path);
extern void gfx_quit(void);
extern void gfx_log_meminfo(void);

#endif
