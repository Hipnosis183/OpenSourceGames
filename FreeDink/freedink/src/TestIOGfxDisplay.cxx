/**
 * Test graphics display

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cxxtest/TestSuite.h>

#include "IOGfxDisplaySW.h"
#include "IOGfxDisplayGL2.h"
#include "IOGfxSurfaceSW.h"
#include "SDL_image.h"
#include "freedink_xpm.h"
#include "log.h"
#include "ImageLoader.h" /* GFX_ref_pal */ // TODO: break dep
#include "gfx_palette.h"

/**
 * Test graphics output
 * Shortcomings:
 * - Buggy driver init
 * - SDL_Renderer drops all ops when window is hidden; use a visible window
 * - Android display is 16-bit, so use fuzzy color comparisons
 * - Stretched display (fullscreen and Android) use linear interpolation, so
 *   check fuzzy pixel positions after flipStretch()
 * - No OpenGL ES 2.0 API to retrieve texture contents, only retrieve main buffer
 */
class TestIOGfxDisplay : public CxxTest::TestSuite {
public:
	IOGfxDisplay* display;
	struct {
		bool gl;
		bool truecolor;
	} lastDisplay;

	SDL_Color white;
	SDL_Color black;
	SDL_Color green;
	SDL_Color blue;
	SDL_Color dark123;


	TestIOGfxDisplay() {
		TS_ASSERT_EQUALS(SDL_InitSubSystem(SDL_INIT_VIDEO), 0);
		//SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

		display = NULL;
		lastDisplay.gl = false;
		lastDisplay.truecolor = false;

		white   = {255, 255, 255, 255};
		black   = {  0,   0,   0, 255};
		green   = {255, 255,   0, 255};
		blue    = {  0,   0, 255, 255};
		dark123 = {  1,   2,   3, 255};
	}
	~TestIOGfxDisplay() {
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}



	void setUp() {
		// init palette; hopefully can be removed if we get rid of blitFormat
		for (int i = 0; i < 256; i++) {
			GFX_ref_pal[i].r = i;
			GFX_ref_pal[i].g = i;
			GFX_ref_pal[i].b = i;
			GFX_ref_pal[i].a = 255;
		}
		// Use a color that fail tests if RGBA->ABGR-reversed (non-symmetric)
		// Fully saturated green and blue to avoid fuzzy color comparisons
		GFX_ref_pal[1] = green;
		GFX_ref_pal[2] = blue;
		GFX_ref_pal[3] = dark123;
		gfx_palette_set_phys(GFX_ref_pal);
	}
	void tearDown() {
	}



	void openDisplay(bool gl, bool truecolor, Uint32 flags) {
		//log_info("* Requesting %s %s", gl?"GL2":"SW", truecolor?"truecolor":"");
		//flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		//if (gl) flags |= SDL_WINDOW_HIDDEN; // off-screen rendering
		//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
		//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

		// cache
		if (display && lastDisplay.gl == gl && lastDisplay.truecolor == truecolor) {
			display->clear();
			return;
		}
		if (display) {
			display->close();
			delete display;
			display = NULL;
		}

		if (gl)
			display = new IOGfxDisplayGL2(50, 50, truecolor, flags);
		else
			display = new IOGfxDisplaySW(50, 50, truecolor, flags);
		bool opened = display->open();
		TS_ASSERT_EQUALS(opened, true);

		lastDisplay.gl = gl;
		lastDisplay.truecolor = truecolor;

		display->logDisplayInfo();
	}
	void closeDisplay() {
		//SDL_Delay(1000);
	}

	void getColorAtRGBA(SDL_Surface* img, int x, int y, SDL_Color* c) {
		Uint32* line = (Uint32*)(&((Uint8*)img->pixels)[y*img->pitch]);
		SDL_GetRGBA(line[x], img->format, &c->r, &c->g, &c->b, &c->a);
	}

	void getColorAtI(SDL_Surface* img, int x, int y, SDL_Color* c) {
		if (img->format->BitsPerPixel == 32) {
			Uint32* line = (Uint32*)(&((Uint8*)img->pixels)[y*img->pitch]);
			SDL_GetRGBA(line[x], img->format, &c->r, &c->g, &c->b, &c->a);
		} else if (img->format->BitsPerPixel == 8) {
			c->r = c->g = c->b = ((Uint8*)img->pixels)[x + y*img->pitch];
			c->a = 255;
		} else {
			c->r = c->g = c->b = c->a = 0;
		}
	}


	void ctestSplash() {
		// A first inter-texture blit before anything else
		// In 2015 the splash screen (first blit+flip) was ignored/delayed
		SDL_Surface* img;
		IOGfxSurface *backbuffer, *splash;

		backbuffer = display->allocBuffer(50, 50);
		//g->flip(backbuffer); // not a single flip

		img = SDL_CreateRGBSurface(0, 40, 40, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		// Make a bit square to cope with stretching and color interpolation
		pixels[0] = 1;
		pixels[1] = 1;
		pixels[2] = 1;
		pixels[img->pitch+0] = 1;
		pixels[img->pitch+1] = 1;
		pixels[img->pitch+2] = 1;
		pixels[img->pitch*2+0] = 1;
		pixels[img->pitch*2+1] = 1;
		pixels[img->pitch*2+0] = 1;
		//SDL_SaveBMP(img, "splash-img.bmp");
		splash = display->upload(img); img = NULL;
		//g->flip(splash); // not a single flip

		SDL_Rect dstrect = {0, 0, -1, -1};
		backbuffer->blit(splash, NULL, &dstrect);

		// Check the result:
		display->flipStretch(backbuffer);
		// Double the blit in case snapshot() reads from the backbuffer
		// https://bugzilla.libsdl.org/show_bug.cgi?id=3619#c4
		// Kinda defeats the goal of this test...
		display->flipStretch(backbuffer);

		SDL_Surface* screenshot = display->screenshot();
		//SDL_SaveBMP(screenshot, "splash-screenshot.bmp");
		TS_ASSERT(screenshot != NULL);
		if (screenshot == NULL)
			return;
		Uint8 cr, cg, cb, ca;
		int x = 1, y = 1;
		display->surfToDisplayCoords(backbuffer, x, y);
		SDL_GetRGBA(((Uint32*)screenshot->pixels)[x+y*screenshot->w],
					screenshot->format,
					&cr, &cg, &cb, &ca);
		TS_ASSERT_EQUALS(cr, 255);
		TS_ASSERT_EQUALS(cg, 255);
		TS_ASSERT_EQUALS(cb, 0);
		TS_ASSERT_EQUALS(ca, 255);
		SDL_FreeSurface(screenshot);

		delete splash;
		delete backbuffer;
	}



	void ctest_alloc() {
		IOGfxSurface* surf = display->allocBuffer(300, 300);
		TS_ASSERT(surf != NULL);
		TS_ASSERT_EQUALS(surf->w, 300);
		TS_ASSERT_EQUALS(surf->h, 300);
		delete surf;
	}



	void ctest_Display_screenshot() {
		SDL_Surface *img, *screenshot;
		IOGfxSurface *backbuffer, *surf;
		SDL_Color cs;
		SDL_Rect bbbox;

		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		img = SDL_CreateRGBSurface(0, 39, 39, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		pixels[0] = 255;
		pixels[1] = 1;
		pixels[img->pitch+0] = 3;
		surf = display->upload(img); img = NULL;
		backbuffer->blit(surf, NULL, NULL);


		// Check final framebuffer
		display->flipDebug(backbuffer);
		// Check that pic is not vertically flipped
		screenshot = display->screenshot(&bbbox);

		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));

		getColorAtRGBA(screenshot, 1, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));

		// Check main buffer precision; not exact on Android's RGB565
		getColorAtRGBA(screenshot, 0, 1, &cs);
		TS_ASSERT_RELATION(std::less_equal<int>, cs.r, 8);
		TS_ASSERT_RELATION(std::less_equal<int>, cs.g, 4);
		TS_ASSERT_RELATION(std::less_equal<int>, cs.b, 8);

		SDL_FreeSurface(screenshot);


		delete surf;
		delete backbuffer;
	}



	void ctest_Surface_screenshot_truecolor() {
		SDL_Surface *img, *screenshot;
		IOGfxSurface *backbuffer, *surf;
		SDL_Color cs;
		SDL_Rect bbbox;


		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		img = SDL_CreateRGBSurface(0, 40, 40, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		pixels[0] = 255;
		pixels[1] = 1;
		pixels[img->pitch+0] = 3;
		surf = display->upload(img); img = NULL;
		backbuffer->blit(surf, NULL, NULL);


		// Check texture directly
		screenshot = backbuffer->screenshot();
		TS_ASSERT(screenshot != NULL);
		if (screenshot == NULL)
			return;

		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));

		getColorAtRGBA(screenshot, 1, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));

		getColorAtRGBA(screenshot, 0, 1, &cs);
		TS_ASSERT_SAME_DATA(&cs, &dark123, sizeof(SDL_Color));

		SDL_FreeSurface(screenshot);


		delete surf;
		delete backbuffer;
	}

	void ctest_Surface_screenshot_indexed() {
		SDL_Surface *img, *screenshot;
		IOGfxSurface *backbuffer, *surf;
		SDL_Color ce, cs;
		SDL_Rect bbbox;


		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		img = SDL_CreateRGBSurface(0, 40, 40, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		pixels[0] = 255;
		pixels[1] = 1;
		pixels[3] = 3;
		surf = display->upload(img); img = NULL;
		backbuffer->blit(surf, NULL, NULL);


		// Check texture directly
		screenshot = backbuffer->screenshot();
		TS_ASSERT(screenshot != NULL);
		if (screenshot == NULL)
			return;

		ce = { 255, 255, 255, 255 };
		getColorAtI(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &ce, sizeof(SDL_Color));

		ce = { 1, 1, 1, 255 };
		getColorAtI(screenshot, 1, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &ce, sizeof(SDL_Color));

		ce = { 3, 3, 3, 255 };
		getColorAtI(screenshot, 3, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &ce, sizeof(SDL_Color));

		SDL_FreeSurface(screenshot);


		delete surf;
		delete backbuffer;
	}


	void ctest_surfToDisplayCoords() {
		SDL_Surface* img;
		IOGfxSurface *surf;
		int x, y;

		img = SDL_CreateRGBSurface(0, 50, 50, 8, 0, 0, 0, 0);
		surf = display->upload(img); img = NULL;

		// Force display to 50x50 even if the system gave us something else
		display->onSizeChange(50, 50);

		x = 0; y = 0;
		display->surfToDisplayCoords(surf, x, y);
		TS_ASSERT_EQUALS(x, 0);
		TS_ASSERT_EQUALS(y, 0);

		x = 11; y = 12;
		display->surfToDisplayCoords(surf, x, y);
		TS_ASSERT_EQUALS(x, 11);
		TS_ASSERT_EQUALS(y, 12);

		display->onSizeChange(800, 480);

		x = 0; y = 0;
		display->surfToDisplayCoords(surf, x, y);
		TS_ASSERT_EQUALS(x, 164);
		TS_ASSERT_EQUALS(y, 4);

		x = 11; y = 12;
		display->surfToDisplayCoords(surf, x, y);
		TS_ASSERT_EQUALS(x, 270);
		TS_ASSERT_EQUALS(y, 120);

		display->onSizeChange(50, 50);

		delete surf;
	}


	void ctest_blit() {
		SDL_Surface *img, *screenshot;
		IOGfxSurface *backbuffer, *surf;
		SDL_Color cs;
		SDL_Rect bbbox;

		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		img = SDL_CreateRGBSurface(0, 5, 5, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		SDL_SetColorKey(img, SDL_TRUE, 0);
		pixels[0] = 255;
		pixels[1] = 1;
		pixels[img->pitch] = 255;
		pixels[img->pitch+1] = 1;
		surf = display->upload(img);

		SDL_Rect dstrect = {0, 0, -1, -1};

		dstrect.x = 0; dstrect.y = 0;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 1, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		dstrect.x = 20; dstrect.y = 20;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 20, 20, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 21, 20, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		dstrect.x = -1; dstrect.y = -1;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		dstrect.x = 49; dstrect.y = 49;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 49, 49, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);


		dstrect.x = 0; dstrect.y = 0;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);
		dstrect.x = -2; dstrect.y = -2;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);
		dstrect.x = -2; dstrect.y = -2;
		backbuffer->blitNoColorKey(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &black, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		dstrect.x = 0; dstrect.y = 0;
		dstrect.w = 10; dstrect.h = 20;
		backbuffer->blitStretch(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 7, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 3, 7, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		SDL_Rect srcrect = {1,1, 1,1};
		dstrect.x = 0; dstrect.y = 0;
		backbuffer->blit(surf, &srcrect, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		delete surf;


		if (display->truecolor) {
			// BMP typically encoded as BGR need to be uploaded with proper format
			img = SDL_CreateRGBSurface(0, 5, 5, 24,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
									   0x0000ff00, 0x00ff0000, 0xff000000, 0x00000000
#else
									   0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000
#endif
									   );
			pixels = (Uint8*)img->pixels;
			pixels[0] = 255; pixels[1] = 0; pixels[2] = 0;
			pixels[3] = 255; pixels[4] = 0; pixels[5] = 0;
			pixels[img->pitch  ] = 255; pixels[img->pitch+1] = 0; pixels[img->pitch+2] = 0;
			pixels[img->pitch+3] = 255; pixels[img->pitch+4] = 0; pixels[img->pitch+5] = 0;
			IOGfxSurface *surfRgb = display->upload(img);

			dstrect = {0, 0, -1, -1};

			dstrect.x = 0; dstrect.y = 0;
			backbuffer->blit(surfRgb, NULL, &dstrect);
			display->flipDebug(backbuffer);
			screenshot = display->screenshot(&bbbox);
			getColorAtRGBA(screenshot, 0, 0, &cs);
			TS_ASSERT_SAME_DATA(&cs, &blue, sizeof(SDL_Color));
			SDL_FreeSurface(screenshot);

			delete surfRgb;
		} else {
			// Image already converted to 8-bit by
			// ImageLoader::loadToBlitFormat, no 24-bit
		}

		delete backbuffer;
	}



	void ctest_fillRect() {
		IOGfxSurface *backbuffer;
		SDL_Surface *screenshot;
		SDL_Color cs;
		SDL_Rect bbbox;

		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		SDL_Rect dstrect = {-1, -1, -1, -1};

		backbuffer->fillRect(NULL, 255, 255, 0);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		dstrect.x = 5;  dstrect.y = 5;
		dstrect.w = 20; dstrect.h = 10;
		backbuffer->fillRect(&dstrect, 0, 0, 255);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 4, 4, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 5, 5, &cs);
		TS_ASSERT_SAME_DATA(&cs, &blue, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 24, 14, &cs);
		TS_ASSERT_SAME_DATA(&cs, &blue, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 25, 14, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 24, 15, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		backbuffer->vlineRGB(10, 0,49, 255,255,0);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 10, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 10, 49, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		backbuffer->hlineRGB(0,49, 10, 255,255,0);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 10, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 49, 10, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		delete backbuffer;
	}

	void ctest_fill_screen() {
		IOGfxSurface *backbuffer;
		SDL_Surface *screenshot;
		SDL_Color cs;
		SDL_Rect bbbox;

		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		backbuffer->fill_screen(0, GFX_ref_pal);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &black, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		backbuffer->fill_screen(1, GFX_ref_pal);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &green, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		backbuffer->fill_screen(2, GFX_ref_pal);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &blue, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		// Using a /8 value to support RGB565 (Android) screens
		SDL_Color gray16 = {16,16,16,255};
		backbuffer->fill_screen(16, GFX_ref_pal);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &gray16, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		delete backbuffer;
	}

	void ctestBrightness() {
		SDL_Surface *img, *screenshot;
		IOGfxSurface *backbuffer, *surf;
		SDL_Color cs;
		SDL_Rect bbbox;

		backbuffer = display->allocBuffer(50, 50);
		bbbox = { 0,0, 50,50 };

		img = SDL_CreateRGBSurface(0, 5, 5, 8, 0, 0, 0, 0);
		Uint8* pixels = (Uint8*)img->pixels;
		SDL_SetPaletteColors(img->format->palette, GFX_ref_pal, 0, 256);
		SDL_SetColorKey(img, SDL_TRUE, 0);
		pixels[0] = 255;
		pixels[1] = 1;
		pixels[img->pitch] = 255;
		pixels[img->pitch+1] = 1;
		surf = display->upload(img);

		SDL_Rect dstrect = {0, 0, -1, -1};
		dstrect.x = 0; dstrect.y = 0;

		display->brightness = 0;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 1, 1, &cs);
		TS_ASSERT_SAME_DATA(&cs, &black, sizeof(SDL_Color));
		SDL_FreeSurface(screenshot);

		display->brightness = 100;
		backbuffer->blit(surf, NULL, &dstrect);
		display->flipDebug(backbuffer);
		screenshot = display->screenshot(&bbbox);
		getColorAtRGBA(screenshot, 0, 0, &cs);
		TS_ASSERT_SAME_DATA(&cs, &white, sizeof(SDL_Color));
		getColorAtRGBA(screenshot, 1, 1, &cs);
		TS_ASSERT_RELATION(std::less_equal<int>, cs.r, 100);
		TS_ASSERT_RELATION(std::less_equal<int>, cs.g, 100);
		TS_ASSERT_RELATION(std::greater_equal<int>, cs.r, 98);
		TS_ASSERT_RELATION(std::greater_equal<int>, cs.g, 97);
		TS_ASSERT_EQUALS(cs.b, 0);
		SDL_FreeSurface(screenshot);
	}


	void test01SplashGL2Truecolor() {
		openDisplay(true, true, 0); // 0 <= off-screen rendering may have bugs >(
		ctestSplash();
		closeDisplay();
	}
	void test_Display_screenshotGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_Display_screenshot();
		closeDisplay();
	}
	void test_Surface_screenshotGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_Surface_screenshot_truecolor();
		closeDisplay();
	}
	void test_allocGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_alloc();
		closeDisplay();
	}
	void test_surfToDisplayCoordsGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_surfToDisplayCoords();
		closeDisplay();
	}
	void test_blitGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_blit();
		closeDisplay();
	}
	void test_fill_screenGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_fill_screen();
		closeDisplay();
	}
	void test_fillRectGL2Truecolor() {
		openDisplay(true, true, 0);
		ctest_fillRect();
		closeDisplay();
	}
	void testBrightnessGL2Truecolor() {
		openDisplay(true, true, 0);
		ctestBrightness();
		closeDisplay();
	}


	void testSplashGL2() {
		openDisplay(true, false, 0);
		ctestSplash();
		closeDisplay();
	}
	void test_Display_screenshotGL2() {
		openDisplay(true, false, 0);
		ctest_Display_screenshot();
		closeDisplay();
	}
	void test_Surface_screenshotGL2() {
		openDisplay(true, false, 0);
		ctest_Surface_screenshot_indexed();
		closeDisplay();
	}
	void test_blitGL2() {
		openDisplay(true, false, 0);
		ctest_blit();
		closeDisplay();
	}
	void test_fillRectGL2() {
		openDisplay(true, false, 0);
		ctest_fillRect();
		closeDisplay();
	}
	void test_fill_screenGL2() {
		openDisplay(true, false, 0);
		ctest_fill_screen();
		closeDisplay();
	}
	// Needs specific palette-based test
//	void testBrightnessGL2Truecolor() {
//		openDisplay(true, true, 0);
//		ctestBrightness();
//		closeDisplay();
//	}


	void testSplashSWTruecolor() {
		openDisplay(false, true, 0); // 0 <= can't render off-screen >(
		ctestSplash();
		closeDisplay();
	}
	void test_Display_screenshotSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_Display_screenshot();
		closeDisplay();
	}
	void test_Surface_screenshotSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_Surface_screenshot_truecolor();
		closeDisplay();
	}
	void test_allocSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_alloc();
		closeDisplay();
	}
	void test_blitSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_blit();
		closeDisplay();
	}
	void test_fillRectSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_fillRect();
		closeDisplay();
	}
	void test_fill_screenSWTruecolor() {
		openDisplay(false, true, 0);
		ctest_fill_screen();
		closeDisplay();
	}
	void testBrightnessSWTruecolor() {
		openDisplay(false, true, 0);
		ctestBrightness();
		closeDisplay();
	}


	void testSplashSW() {
		openDisplay(false, false, 0);
		ctestSplash();
		closeDisplay();
	}
	void test_Display_screenshotSW() {
		openDisplay(false, false, 0);
		ctest_Display_screenshot();
		closeDisplay();
	}
	void test_Surface_screenshotSW() {
		openDisplay(false, false, 0);
		ctest_Surface_screenshot_indexed();
		closeDisplay();
	}
	void test_blitSW() {
		openDisplay(false, false, 0);
		ctest_blit();
		closeDisplay();
	}
	void test_fillRectSW() {
		openDisplay(false, false, 0);
		ctest_fillRect();
		closeDisplay();
	}
	void test_fill_screenSW() {
		openDisplay(false, false, 0);
		ctest_fill_screen();
		closeDisplay();
	}
	// Needs specific palette-based test
//	void testBrightnessSW() {
//		openDisplay(false, false, 0);
//		ctestBrightness();
//		closeDisplay();
//	}
};
