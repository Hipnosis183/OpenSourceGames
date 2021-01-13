/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Function used to format string.
 * Eg. : Format("Hello %s", "world") returns "Hello World".
 *****************************************************************************/

#include <WARMUX_i18n.h>
#include <WARMUX_error.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef USE_FRIBIDI
# include <fribidi/fribidi.h>
# include <cstring>

FriBidiCharType pbase_dir = FRIBIDI_TYPE_ON;
FriBidiChar unicode_buffer[2048];
char buffer[2048];

char* localization(const char * message) {
  char* string = gettext(message);
  int l        = strlen(string);
  int l_u      = fribidi_charset_to_unicode(FRIBIDI_CHAR_SET_UTF8, string, l, unicode_buffer);

  fribidi_log2vis(unicode_buffer, l_u, &pbase_dir, unicode_buffer, NULL, NULL, NULL);
  fribidi_unicode_to_charset(FRIBIDI_CHAR_SET_UTF8, unicode_buffer, l_u, (char *)buffer);
  return buffer;
}
#endif /* USE_FRIBIDI */

#define BUFFER_SIZE   512

std::string Format(const char *format, ...)
{
  char buffer[BUFFER_SIZE];
  va_list argp;
  std::string result;

  va_start(argp, format);
  int size = vsnprintf(buffer, BUFFER_SIZE, format, argp);
  va_end(argp);

  //if (size < 0)
  //  Error("Error formating string...");

  if (size < BUFFER_SIZE) {
    result = std::string(buffer);
  } else {
    char *bigBuffer = (char *)malloc((size + 1) * sizeof(char));
    if (bigBuffer == NULL)
      Error("Out of memory !");

    // We need to redo va_start/va_end before calling vsnprintf
    // with same arguments else the va_list may be already modified
    va_start(argp, format);
    size = vsnprintf(bigBuffer, size + 1, format, argp);
    va_end(argp);

    if(size < 0)
      Error( "Error formating string...");

    result = std::string(bigBuffer);
    free(bigBuffer);
  }

  return result;
}

#ifdef ENABLE_NLS
static void I18N_SetDir(const std::string &dir)
{
  printf("o Bind text domain to: %s\n"
         "o Codeset: %s\n"
         "o Text domain: %s\n",
         bindtextdomain(PACKAGE, dir.c_str()), bind_textdomain_codeset(PACKAGE, "UTF-8"), textdomain(PACKAGE));
}

void InitI18N(const std::string &dir, const std::string &default_language)
{
#ifdef ANDROID
  // Yes this is a leak
  static char *def = NULL;

  if (!def) {
    const char *lang = getenv("LANG");
    if (lang) {
      def = (char*)malloc(strlen(lang)+1);
      strcpy(def, lang);
    } else {
      def = (char*)malloc(1);
      def[0] = 0;
    }
  }

  if (default_language == "")
    setenv("LANG", def, 1);
  else
    setenv("LANG", default_language.c_str(), 1);

  // setlocale always return NULL
  printf("o Locale: %s\n", getenv("LANG"));
#else
  const char *locale = setlocale(LC_ALL, "");

  if (!locale) {
    fprintf(stderr, "Couldn't set locale!\n");
    return;
  }
  printf("o Locale: %s\n", default_language.c_str());

# ifdef _WIN32
  std::string variable = "LANGUAGE=";
  variable += default_language;
  _putenv(variable.c_str());
# else
  setenv("LANGUAGE", default_language.c_str(), 1);
# endif
#endif

  I18N_SetDir(dir);
}
#endif
