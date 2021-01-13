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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "IOGfxSurfaceGL2.h"

#include "SDL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "log.h"
#include "IOGfxDisplayGL2.h"
#include "IOGfxGLFuncs.h"
#include "ImageLoader.h" /* GFX_ref_pal */ // TODO: break dep

IOGfxSurfaceGL2::IOGfxSurfaceGL2(IOGfxDisplayGL2* display, GLuint texture, int w, int h, SDL_Color colorkey)
	: IOGfxSurface(w, h), display(display), texture(texture), colorkey(colorkey) {
	fbo = 0;
}

IOGfxSurfaceGL2::~IOGfxSurfaceGL2() {
	IOGfxGLFuncs* gl = display->gl;
	gl->DeleteTextures(1, &texture);
	gl->DeleteFramebuffers(1, &fbo);
}

/**
 * Lazy GL framebuffer creation
 * Only necessary for the few target textures (backbuffer, background...)
 */
bool IOGfxSurfaceGL2::bindAsFramebuffer() {
	IOGfxGLFuncs* gl = display->gl;
	if (fbo == 0) {
		// bind this texture as framebuffer
		gl->GenFramebuffers(1, &fbo);
		gl->BindFramebuffer(GL_FRAMEBUFFER, fbo);
		gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);
		GLenum status;
		if ((status = gl->CheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			log_error("glCheckFramebufferStatus: error 0x%x", status);
			switch (status) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				log_error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				log_error("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				log_error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				log_error("GL_FRAMEBUFFER_UNSUPPORTED");
				break;
			}
			return false;
		}
	} else {
		gl->BindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	return true;
}


/* Function specifically made for Dink'C fill_screen() */
void IOGfxSurfaceGL2::fill_screen(int num, SDL_Color* palette) {
	fillRect(NULL, palette[num].r, palette[num].g, palette[num].b);
	// TODO: check indexed mode + palette change
}

int IOGfxSurfaceGL2::fillRect(const SDL_Rect *dstrect, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Rect dstrect_if_not_null;
	if (dstrect == NULL) {
		dstrect_if_not_null.x = 0;
		dstrect_if_not_null.y = 0;
		dstrect_if_not_null.w = w;
		dstrect_if_not_null.h = h;
		dstrect = &dstrect_if_not_null;
	}

	if (!display->truecolor) {
		SDL_PixelFormat fmt;
		fmt.palette = SDL_AllocPalette(256);
		memcpy(fmt.palette->colors, GFX_ref_pal, sizeof(GFX_ref_pal));
		Uint8 i = SDL_MapRGB(&fmt, r,g,b);
		r = g = b = i;
	}

	IOGfxGLFuncs* gl = display->gl;

	display->gl->UseProgram(display->fillRect->program);
	display->setVertexAttrib(display->fillRect, display->fillRect->attributes["v_coord"], display->vboSpriteVertices, 4);

	glm::mat4 projection = glm::ortho(0.0f, 1.0f*display->w, 0.0f, 1.0f*display->h);
	glm::mat4 m_transform;
	m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(dstrect->x,dstrect->y, 0.0))
		* glm::scale(glm::mat4(1.0f), glm::vec3(dstrect->w, dstrect->h, 0.0));
	glm::mat4 mvp = projection * m_transform; // * view * model * anim;
	gl->UniformMatrix4fv(display->fillRect->uniforms["mvp"], 1, GL_FALSE, glm::value_ptr(mvp));
	gl->Uniform4f(display->fillRect->uniforms["color"], r/255.0,g/255.0,b/255.0,1.0);

	/* Push each element in buffer_vertices to the vertex shader */
	bindAsFramebuffer();
	gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//	gl->DisableVertexAttribArray(display->fillRect->attributes["v_coord"]);

	return 0;
}

int IOGfxSurfaceGL2::internalBlit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect, bool useColorKey) {
	if (src == NULL)
		return SDL_SetError("IOGfxSurfaceGL2::blitStretch: passed a NULL surface");
	if (dstrect == NULL)
		return SDL_SetError("IOGfxSurfaceGL2::blitStretch: passed a NULL dstrect");
	IOGfxSurfaceGL2* src_surf = dynamic_cast<IOGfxSurfaceGL2*>(src);
	GLuint src_tex = src_surf->texture;

	IOGfxGLFuncs* gl = display->gl;

	GLuint vbo;
	if (srcrect == NULL || (srcrect->x == 0 && srcrect->y == 0 && srcrect->w == src_surf->w && srcrect->h == src_surf->h)) {
		vbo = display->vboSpriteTexcoords;
	} else {
		float x1 = 1.0 * srcrect->x / src_surf->w;
		float y1 = 1.0 * srcrect->y / src_surf->h;
		float x2 = 1.0 * (srcrect->x+srcrect->w) / src_surf->w;
		float y2 = 1.0 * (srcrect->y+srcrect->h) / src_surf->h;
		GLfloat croppedSpriteTexcoords[] = {
			x1, y1,
			x2, y1,
			x1, y2,
			x2, y2,
		};
		gl->BindBuffer(GL_ARRAY_BUFFER, display->vboCroppedSpriteTexcoords);
		gl->BufferData(GL_ARRAY_BUFFER, sizeof(croppedSpriteTexcoords), croppedSpriteTexcoords, GL_STATIC_DRAW);
		vbo = display->vboCroppedSpriteTexcoords;
	}

	gl->UseProgram(display->blit->program);
	display->setVertexAttrib(display->blit, display->blit->attributes["v_coord"], display->vboSpriteVertices, 4);
	display->setVertexAttrib(display->blit, display->blit->attributes["v_texcoord"], vbo, 2);

	gl->ActiveTexture(GL_TEXTURE0);
	gl->Uniform1i(display->blit->uniforms["texture"], /*GL_TEXTURE*/0);
	gl->BindTexture(GL_TEXTURE_2D, src_tex);

	glm::mat4 projection = glm::ortho(0.0f, 1.0f*display->w, 0.0f, 1.0f*display->h);
	glm::mat4 m_transform;
	m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(dstrect->x,dstrect->y, 0.0))
		* glm::scale(glm::mat4(1.0f), glm::vec3(dstrect->w, dstrect->h, 0.0));
	glm::mat4 mvp = projection * m_transform; // * view * model * anim;
	gl->UniformMatrix4fv(display->blit->uniforms["mvp"], 1, GL_FALSE, glm::value_ptr(mvp));

	if (useColorKey && src_surf->colorkey.a == SDL_ALPHA_TRANSPARENT)
		gl->Uniform3f(display->blit->uniforms["colorkey"],
				src_surf->colorkey.r/255.0, src_surf->colorkey.g/255.0, src_surf->colorkey.b/255.0);
	else
		gl->Uniform3f(display->blit->uniforms["colorkey"], -1,-1,-1);

	/* Push each element in buffer_vertices to the vertex shader */
	bindAsFramebuffer();
	gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//	gl->DisableVertexAttribArray(display->blit->attributes["v_coord"]);
//	gl->DisableVertexAttribArray(display->blit->attributes["v_texcoord"]);

	return 0;
}


int IOGfxSurfaceGL2::blit(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect) {
	if (src == NULL)
		return SDL_SetError("IOGfxSurfaceGL2::blit: passed a NULL surface");
	IOGfxSurfaceGL2* src_surf = dynamic_cast<IOGfxSurfaceGL2*>(src);

	SDL_Rect dstrect_if_not_null;
	if (dstrect == NULL) {
		dstrect = &dstrect_if_not_null;
		dstrect->x = 0;
		dstrect->y = 0;
	}
	// Force no-stretch blit
	if (srcrect == NULL) {
		dstrect->w = src_surf->w;
		dstrect->h = src_surf->h;
	} else {
		dstrect->w = srcrect->w;
		dstrect->h = srcrect->h;
	}

	return internalBlit(src, srcrect, dstrect, true);
}

int IOGfxSurfaceGL2::blitStretch(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect) {
	return internalBlit(src, srcrect, dstrect, true);
}

int IOGfxSurfaceGL2::blitNoColorKey(IOGfxSurface* src, const SDL_Rect* srcrect, SDL_Rect* dstrect) {
	if (src == NULL)
		return SDL_SetError("IOGfxSurfaceGL2::blitStretch: passed a NULL surface");
	IOGfxSurfaceGL2* src_surf = dynamic_cast<IOGfxSurfaceGL2*>(src);

	SDL_Rect dstrect_if_not_null;
	if (dstrect == NULL) {
		dstrect = &dstrect_if_not_null;
		dstrect->x = 0;
		dstrect->y = 0;
	}
	// Force no-stretch blit
	if (srcrect == NULL) {
		dstrect->w = src_surf->w;
		dstrect->h = src_surf->h;
	} else {
		dstrect->w = srcrect->w;
		dstrect->h = srcrect->h;
	}

	return internalBlit(src, srcrect, dstrect, false);
}

SDL_Surface* IOGfxSurfaceGL2::screenshot() {
	IOGfxGLFuncs* gl = display->gl;

	bindAsFramebuffer();

	// assume 4-bytes alignment
	SDL_Surface* image = SDL_CreateRGBSurface(0,
		w, h, 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
	);
	unsigned char* pixels = (unsigned char*)image->pixels;
	gl->ReadPixels(0, 0, w, h,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Don't flip vertically - textures were uploaded OpenGL-reversed already

	return image;
}

unsigned int IOGfxSurfaceGL2::getMemUsage() {
	// TODO
	return 0;
}
