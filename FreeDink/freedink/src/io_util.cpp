/**
 * Filesystem helpers

 * Copyright (C) 2005, 2007, 2008, 2009  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  /* strcasecmp */
#include <dirent.h>
#include <errno.h>

/* stat(2) */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "SDL.h"
#ifdef HAVE_LIBZIP
#  include "SDL_rwops_libzip.h"
#else
#  ifdef HAVE_ZZIPLIB
#include "SDL_rwops_zzip.h"
#  endif
#endif

#include "paths.h"

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__ || defined __EMSCRIPTEN__
#else
/* Returns a pointer to the end of the current path element (file or
   directory) */
static char*
end_of_elt(char *str)
{
  char *p = str;
  while ((*p != '/') && (*p != '\\') && (*p != '\0'))
    p++;
  return p;
}
#endif

/**
 * Look for filename case-insensitively, to mimic MS Woe's
 * case-insensitive file system. It modifies the 'filename' buffer
 * in-place. Also converts '\' to '/'. I REPEAT: the filename buffer
 * is modified, so it cannot be "a constant string", or you'll get a
 * segfault. */
void
ciconvert (char *filename)
{
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
	/* For __EMSCRIPTEN__ remember to -s CASE_INSENSITIVE_FS=1 */
  return;
#elif defined __EMSCRIPTEN__
  /* convert windows-style backslashes in paths */
  char *pcur_elt = filename;
  while (*pcur_elt)
	{
		if (*pcur_elt == '\\')
			*pcur_elt = '/';
		pcur_elt++;
	}
#else
  /* Parse all the directories that composes filename */
  char *cur_dir = NULL;
  char *pcur_elt, *pend_of_elt, *pend_of_cur_dir;
  int error = 0;
  
  /* First simply try to open the file, constant complexity. Permits
     decent speed for careful authors who write exact filenames. */
  FILE *f;
  if ((f = fopen(filename, "r")) != NULL)
    {
      fclose(f);
      return;
    }

  /* Else, check each path element of the filename */
  /* No need to support volumes ("C:\"...) because this function
     already returned in case-insensitive environments (woe&dos) */
  if (filename[0] == '/')
    {
      cur_dir = (char*)malloc(strlen("/") + strlen(filename) + 1);
      strcpy(cur_dir, "/");
      pend_of_cur_dir = cur_dir + 1;
    }
  else
    {
      cur_dir = (char*)malloc(strlen("./") + strlen(filename) + 1);
      strcpy(cur_dir, "./");
      pend_of_cur_dir = cur_dir + 2;
    }
  pcur_elt = filename;
  do
    {
      char end_of_elt_backup;

      /* skip leading or multiple slashes */
      while (*pcur_elt == '/' || *pcur_elt == '\\')
	{
	  *pcur_elt = '/';
	  pcur_elt++;
	}

      pend_of_elt = end_of_elt(pcur_elt);
      end_of_elt_backup = *pend_of_elt;
      *pend_of_elt = '\0';
      /* pcur_elt is now a \0-terminated string containing the current
	 path element. */
      
      /* Now check if there's a matching entry in the directory */
      DIR *list;
      struct dirent *entry;
      int found = 0;
      
      list = opendir (cur_dir);
      if (list != NULL)
	{
	  /* note: we may want to sort list first */
	  while ((entry = readdir (list)) != NULL && !found)
	    {
	      if (strcasecmp(pcur_elt, entry->d_name) == 0)
		{
		  /* Good case-insensitive match: replace the
		     user-provided filename with it */
		  strcpy(pcur_elt, entry->d_name);
		  found = 1;
		}
		}
	  closedir (list);
	}
      else if (errno == EACCES)
	{
	  /* Maybe attempting to read a private prefix dir such as
	     /data/data on Android, try to use it as-is. */
	  found = 1;
	}
      if (!found)
	error = 1;

      /* Prepare parsing next path element, unless the current element
	 was the last one */
      if (end_of_elt_backup != '\0')
	{
	  *pend_of_elt = '/'; /* restore */

	  /* Prepare next directory */
	  {
	    int cur_elt_len = pend_of_elt - pcur_elt;
	    strncpy(pend_of_cur_dir, pcur_elt, cur_elt_len + 1);
	    pend_of_cur_dir += cur_elt_len + 1;
	    *pend_of_cur_dir = '\0';
	  }
	  
	  /* go to the next path element */
	  pcur_elt = pend_of_elt + 1;
	}
    }
  while(*pend_of_elt != '\0' && !error);
  free(cur_dir);

  /* If there was an error, we return a half-converted path (maybe the
     file didn't exist yet, but leading directories still needed to be
     converted); otherwise, filename contains the fully-converted
     path, ready to be opened on a case-sensitive filesystem. */
#endif /* !_WIN32 !__EMSCRIPTEN__*/
}

/**
 * Does this file exist and can be opened?
 * Note: this is case-sensitive.
 */
int exist(char *name)
{
  FILE *fp = fopen(name, "rb");
  if (!fp)
    return 0;
  fclose(fp);
  return 1;
}

/**
 * Is it a directory that exists?
 */
int is_directory(const char *name)
{
  char *tmp_filename = strdup(name);
  int accessible = 0;
  int retval = 0;

  struct stat buf;
  /* ciconvert(tmp_filename); */
  accessible = stat(tmp_filename, &buf);
  free(tmp_filename);

  if (accessible < 0)
    retval = 0;
  else
    retval = S_ISDIR(buf.st_mode);

  return retval;
}

/**
 * So-called "portable" dirname - that is, it supports backslash and
 * forward-slash. That way, it can process filenames from dink.ini, in
 * particular. Return a newly allocated string.
 */
char*
pdirname (const char* filename)
{
  char *retval = strdup(filename);
  char *pc = retval + strlen(retval);
  while (pc >= retval && *pc != '/' && *pc != '\\')
    pc--;
  if (pc >= retval)
    *pc = '\0';
  return retval;
}


/**
 * Read integer portably (same result with MSB and LSB
 * endianness). Source data is a file with little-endian data.
 */
int read_lsb_int(FILE *f)
{
  unsigned char buf[4];
  fread(buf, 4, 1, f);
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}


/**
 * Write integer portably (same result with MSB and LSB
 * endianness). Will write little-endian data to file.
 */
void write_lsb_int(int n, FILE *f)
{
  unsigned char buf[4];
  buf[0] = n & 0xFF;
  buf[1] = (n >> (1*8)) & 0xFF;
  buf[2] = (n >> (2*8)) & 0xFF;
  buf[3] = (n >> (3*8)) & 0xFF;
  fwrite(buf, 4, 1, f);
}

/**
 * Read unsigned integer portably (same result with MSB and LSB
 * endianness). Source data is a file with little-endian data.
 */
unsigned int read_lsb_uint(FILE *f)
{
  unsigned char buf[4];
  fread(buf, 4, 1, f);
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}

/**
 * Write integer portably (same result with MSB and LSB
 * endianness). Will write little-endian data to file.
 */
void write_lsb_uint(unsigned int n, FILE *f)
{
  unsigned char buf[4];
  buf[0] = n & 0xFF;
  buf[1] = (n >> (1*8)) & 0xFF;
  buf[2] = (n >> (2*8)) & 0xFF;
  buf[3] = (n >> (3*8)) & 0xFF;
  fwrite(buf, 4, 1, f);
}


/**
 * Read short portably (same result with MSB and LSB
 * endianness). Source data is a file with little-endian data.
 */
short read_lsb_short(FILE *f)
{
  unsigned char buf[2];
  fread(buf, 2, 1, f);
  return (buf[1] << 8) | (buf[0]);
}


/**
 * Write short portably (same result with MSB and LSB
 * endianness). Will write little-endian data to file.
 */
void write_lsb_short(short n, FILE *f)
{
  unsigned char buf[2];
  buf[0] = n & 0xFF;
  buf[1] = (n >> (1*8)) & 0xFF;
  fwrite(buf, 2, 1, f);
}
