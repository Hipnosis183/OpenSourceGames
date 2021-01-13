/**
 * Access to GL functions without linking to system-specific library

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

#ifndef IOGRAPHICSGLFUNCS_H
#define IOGRAPHICSGLFUNCS_H

#ifndef APIENTRY
#  if defined(_WIN32)
#    define APIENTRY __stdcall
#  else
#    define APIENTRY
#  endif
#endif

//#include <GLES2/gl2.h>
//#if 0
#include <stddef.h> /* ptrdiff_t */
typedef void GLvoid;
typedef char GLchar;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef ptrdiff_t GLsizeiptr;

typedef unsigned char GLboolean;
#define GL_FALSE 0
#define GL_TRUE 1

#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_OUT_OF_MEMORY 0x0505

#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

#define GL_DEPTH_BUFFER_BIT   0x0100
#define GL_STENCIL_BUFFER_BIT 0x0400
#define GL_COLOR_BUFFER_BIT   0x4000

#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909

#define GL_UNSIGNED_BYTE 0x1401
#define GL_FLOAT 0x1406
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLAMP 0x2900

#define GL_ARRAY_BUFFER 0x8892
#define GL_TRIANGLE_STRIP 0x0005
#define GL_STATIC_DRAW 0x88E4

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1

#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD

#define GL_READ_BUFFER 0x0C02
#define GL_FRONT 0x0404
#define GL_BACK 0x0405

//#endif

class IOGfxGLFuncs {
public:
	IOGfxGLFuncs();

	GLenum (APIENTRY *GetError)(void);
	void (APIENTRY *Enable)(GLenum cap);
	void (APIENTRY *GetIntegerv)(GLenum pname, GLint* params);
	void (APIENTRY *BlendFunc)(GLenum sfactor, GLenum dfactor);

	void (APIENTRY *Clear)(GLbitfield);
	void (APIENTRY *ClearColor)(GLclampf, GLclampf, GLclampf, GLclampf);
	void (APIENTRY *Viewport)(GLint x, GLint y, GLsizei width, GLsizei height);

	void (APIENTRY *ReadBuffer)(GLenum mode);
	void (APIENTRY *ReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

	void (APIENTRY *GenTextures)(GLsizei n, GLuint *textures);
	void (APIENTRY *BindTexture)(GLenum target, GLuint texture);
	void (APIENTRY *DeleteTextures)(GLsizei n, const GLuint *textures);
	void (APIENTRY *TexParameteri)(GLenum target, GLenum pname, GLint param);
	void (APIENTRY *TexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);

	void (APIENTRY *GenBuffers)(GLsizei n, GLuint* buffers);
	void (APIENTRY *BindBuffer)(GLenum target, GLuint buffer);
	void (APIENTRY *BufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
	void (APIENTRY *DeleteBuffers)(GLsizei n, const GLuint* buffers);

	GLboolean (APIENTRY *IsShader)(GLuint shader);
	GLuint (APIENTRY *CreateShader)(GLenum type);
	void (APIENTRY *ShaderSource)(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
	void (APIENTRY *CompileShader)(GLuint shader);
	void (APIENTRY *GetShaderiv)(GLuint shader, GLenum pname, GLint* param);
	void (APIENTRY *GetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
	void (APIENTRY *AttachShader)(GLuint program, GLuint shader);
	void (APIENTRY *DeleteShader)(GLuint shader);

	GLboolean (APIENTRY *IsProgram)(GLuint program);
	GLuint (APIENTRY *CreateProgram)(void);
	void (APIENTRY *GetProgramiv)(GLuint program, GLenum pname, GLint* param);
	void (APIENTRY *LinkProgram)(GLuint program);
	void (APIENTRY *GetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
	void (APIENTRY *DeleteProgram)(GLuint program);

	GLint (APIENTRY *GetAttribLocation)(GLuint program, const GLchar* name);
	GLint (APIENTRY *GetUniformLocation)(GLuint program, const GLchar* name);

	void (APIENTRY *UseProgram)(GLuint program);
	void (APIENTRY *ActiveTexture)(GLenum texture);
	void (APIENTRY *Uniform1i)(GLint location, GLint v0);
	void (APIENTRY *Uniform1f)(GLint location, GLfloat v0);
	void (APIENTRY *Uniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	void (APIENTRY *Uniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void (APIENTRY *UniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (APIENTRY *UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

	void (APIENTRY *EnableVertexAttribArray)(GLuint);
	void (APIENTRY *VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
	void (APIENTRY *DisableVertexAttribArray)(GLuint);

	void (APIENTRY *GenFramebuffers)(GLsizei n, GLuint* framebuffers);
	void (APIENTRY *BindFramebuffer)(GLenum target, GLuint framebuffer);
	void (APIENTRY *FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	GLenum (APIENTRY *CheckFramebufferStatus)(GLenum target);
	void (APIENTRY *DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);

	void (APIENTRY *DrawArrays)(GLenum mode, GLint first, GLsizei count);

	void logGetError();
};

#endif
