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

#include "IOGfxGLFuncs.h"

#include "SDL.h"

#include "log.h"

IOGfxGLFuncs::IOGfxGLFuncs() {
	GetError = (GLenum (APIENTRY*)(void))SDL_GL_GetProcAddress("glGetError");
	Enable = (void (APIENTRY*)(GLenum cap))SDL_GL_GetProcAddress("glEnable");
	GetIntegerv = (void (APIENTRY*)(GLenum pname, GLint* params))SDL_GL_GetProcAddress("glGetIntegerv");
	BlendFunc = (void (APIENTRY*)(GLenum sfactor, GLenum dfactor))SDL_GL_GetProcAddress("glBlendFunc");

	Clear = (void (APIENTRY *)(GLbitfield))SDL_GL_GetProcAddress("glClear");
	ClearColor = (void (APIENTRY *)(GLclampf, GLclampf, GLclampf, GLclampf))SDL_GL_GetProcAddress("glClearColor");
	Viewport = (void (APIENTRY *)(GLint x, GLint y, GLsizei width, GLsizei height))SDL_GL_GetProcAddress("glViewport");

	ReadPixels = (void (APIENTRY *)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels))SDL_GL_GetProcAddress("glReadPixels");

	GenTextures = (void (APIENTRY*)(GLsizei n, GLuint *textures))SDL_GL_GetProcAddress("glGenTextures");
	BindTexture = (void (APIENTRY*)(GLenum target, GLuint texture))SDL_GL_GetProcAddress("glBindTexture");
	DeleteTextures = (void (APIENTRY*)(GLsizei n, const GLuint *textures))SDL_GL_GetProcAddress("glDeleteTextures");
	TexParameteri = (void (APIENTRY*)(GLenum target, GLenum pname, GLint param))SDL_GL_GetProcAddress("glTexParameteri");
	TexImage2D = (void (APIENTRY*)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels))SDL_GL_GetProcAddress("glTexImage2D");

	GenBuffers = (void (APIENTRY*)(GLsizei n, GLuint* buffers))SDL_GL_GetProcAddress("glGenBuffers");
	BindBuffer = (void (APIENTRY*)(GLenum target, GLuint buffer))SDL_GL_GetProcAddress("glBindBuffer");
	BufferData = (void (APIENTRY*)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage))SDL_GL_GetProcAddress("glBufferData");
	DeleteBuffers = (void (APIENTRY*)(GLsizei n, const GLuint* buffers))SDL_GL_GetProcAddress("glDeleteBuffers");

	IsShader = (GLboolean (APIENTRY *)(GLuint shader))SDL_GL_GetProcAddress("glIsShader");
	CreateShader = (GLuint (APIENTRY *)(GLenum type))SDL_GL_GetProcAddress("glCreateShader");
	ShaderSource = (void (APIENTRY*)(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths))SDL_GL_GetProcAddress("glShaderSource");
	CompileShader = (void (APIENTRY*)(GLuint shader))SDL_GL_GetProcAddress("glCompileShader");
	GetShaderiv = (void (APIENTRY*)(GLuint shader, GLenum pname, GLint* param))SDL_GL_GetProcAddress("glGetShaderiv");
	GetShaderInfoLog = (void (APIENTRY*)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog))SDL_GL_GetProcAddress("glGetShaderInfoLog");
	AttachShader = (void (APIENTRY*)(GLuint program, GLuint shader))SDL_GL_GetProcAddress("glAttachShader");
	DeleteShader = (void (APIENTRY*)(GLuint shader))SDL_GL_GetProcAddress("glDeleteShader");

	IsProgram = (GLboolean (APIENTRY *)(GLuint program))SDL_GL_GetProcAddress("glIsProgram");
	CreateProgram = (GLuint (APIENTRY *)(void))SDL_GL_GetProcAddress("glCreateProgram");
	GetProgramiv = (void (APIENTRY*)(GLuint program, GLenum pname, GLint* param))SDL_GL_GetProcAddress("glGetProgramiv");
	LinkProgram = (void (APIENTRY*)(GLuint program))SDL_GL_GetProcAddress("glLinkProgram");
	GetProgramInfoLog = (void (APIENTRY*)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog))SDL_GL_GetProcAddress("glGetProgramInfoLog");
	DeleteProgram = (void (APIENTRY*)(GLuint program))SDL_GL_GetProcAddress("glDeleteProgram");

	GetAttribLocation = (GLint (APIENTRY*)(GLuint program, const GLchar* name))SDL_GL_GetProcAddress("glGetAttribLocation");
	GetUniformLocation = (GLint (APIENTRY*)(GLuint program, const GLchar* name))SDL_GL_GetProcAddress("glGetUniformLocation");

	UseProgram = (void (APIENTRY*)(GLuint program))SDL_GL_GetProcAddress("glUseProgram");
	ActiveTexture = (void (APIENTRY*)(GLenum texture))SDL_GL_GetProcAddress("glActiveTexture");
	Uniform1i = (void (APIENTRY*)(GLint location, GLint v0))SDL_GL_GetProcAddress("glUniform1i");
	Uniform1f = (void (APIENTRY*)(GLint location, GLfloat v0))SDL_GL_GetProcAddress("glUniform1f");
	Uniform3f = (void (APIENTRY*)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2))SDL_GL_GetProcAddress("glUniform3f");
	Uniform4f = (void (APIENTRY*)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))SDL_GL_GetProcAddress("glUniform4f");
	UniformMatrix3fv = (void (APIENTRY*)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value))SDL_GL_GetProcAddress("glUniformMatrix3fv");
	UniformMatrix4fv = (void (APIENTRY*)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value))SDL_GL_GetProcAddress("glUniformMatrix4fv");

	EnableVertexAttribArray = (void (APIENTRY*)(GLuint))SDL_GL_GetProcAddress("glEnableVertexAttribArray");
	VertexAttribPointer = (void (APIENTRY*)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer))SDL_GL_GetProcAddress("glVertexAttribPointer");
	DisableVertexAttribArray = (void (APIENTRY*)(GLuint))SDL_GL_GetProcAddress("glDisableVertexAttribArray");

	GenFramebuffers = (void (APIENTRY*)(GLsizei n, GLuint* framebuffers))SDL_GL_GetProcAddress("glGenFramebuffers");
	BindFramebuffer = (void (APIENTRY*)(GLenum target, GLuint framebuffer))SDL_GL_GetProcAddress("glBindFramebuffer");
	FramebufferTexture2D = (void (APIENTRY*)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level))SDL_GL_GetProcAddress("glFramebufferTexture2D");
	CheckFramebufferStatus = (GLenum (APIENTRY*)(GLenum target))SDL_GL_GetProcAddress("glCheckFramebufferStatus");
	DeleteFramebuffers = (void (APIENTRY*)(GLsizei n, const GLuint* framebuffers))SDL_GL_GetProcAddress("glDeleteFramebuffers");

	DrawArrays = (void (APIENTRY*)(GLenum mode, GLint first, GLsizei count))SDL_GL_GetProcAddress("glDrawArrays");
}

void IOGfxGLFuncs::logGetError() {
	switch (this->GetError()) {
	case GL_NO_ERROR:
		log_info("GL_NO_ERROR");
		break;
	case GL_INVALID_ENUM:
		log_info("GL_INVALID_ENUM");
		break;
	case GL_INVALID_VALUE:
		log_info("GL_INVALID_VALUE");
		break;
	case GL_INVALID_OPERATION:
		log_info("GL_INVALID_OPERATION");
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		log_info("GL_INVALID_FRAMEBUFFER_OPERATION");
		break;
	case GL_OUT_OF_MEMORY:
		log_info("GL_OUT_OF_MEMORY");
		break;
	}
}
