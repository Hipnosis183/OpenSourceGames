/**
 * System initialization, common to FreeDink and FreeDinkEdit

 * Copyright (C) 2007, 2008, 2009, 2010, 2014, 2015  Sylvain Beucler

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
#include <math.h>
#include <getopt.h>
#include <unistd.h> /* chdir */
#include <xalloc.h>

#include <locale.h>
#include "gettext.h"
#define _(String) gettext (String)

#include "app.h"
#include "SDL.h"
#include "dinkini.h"
#include "DMod.h"
#include "hardness_tiles.h"
#include "live_screen.h"
#include "gfx.h"
#include "sfx.h"
#include "input.h"
#include "paths.h"
#include "log.h"

#ifdef __ANDROID__
#include <string.h> /* strerror */
#include <errno.h>
#include <jni.h>
extern char **environ;

static void log_environment() {
	for (char **pair = environ; *pair; pair++)
		log_info("%s", *pair);

}

// No default locale on Android, need to get it manually  (T-T)
// TODO: submit me for inclusion in SDL2
// TODO: ScummVM uses a simpler technique, combine %s_%s:
//   __system_property_get("persist.sys.language", buf1)
//   __system_property_get("persist.sys.country", buf2)
static void android_set_LANG() {
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
	jmethodID mid;
	
	// locale = Locale.getDefault()
    jclass localeClass = env->FindClass("java/util/Locale");
    mid = env->GetStaticMethodID(localeClass, "getDefault",
								 "()Ljava/util/Locale;");
	jobject locale = env->CallStaticObjectMethod(localeClass, mid);
	
	// locale.toString()
	mid = env->GetMethodID(localeClass, "toString",
						   "()Ljava/lang/String;");
	jstring toString = (jstring)env->CallObjectMethod(locale, mid);
	const char* lang = env->GetStringUTFChars(toString, NULL);
	
	SDL_setenv("LANG", lang, 1);
	env->ReleaseStringUTFChars(toString, lang);
}
#endif

/**
 * What to do if running out of memory
 */
void xalloc_die (void) {
  fprintf(stderr, "Memory exhausted!");
  abort();
}

/**
 * chdir to ease locating resources
 */
static void app_chdir() {
#ifdef __ANDROID__
  /* SD Card - SDL_AndroidGetExternalStoragePath()
     == /storage/sdcard0/Android/data/org.freedink/files */
  log_info("Android external storage: %s\n", SDL_AndroidGetExternalStoragePath());
  if (SDL_AndroidGetExternalStoragePath() == NULL)
    log_error("Could not get external storage path '%s': %s'",
	      SDL_AndroidGetExternalStoragePath(),
	      SDL_GetError());
  int state = SDL_AndroidGetExternalStorageState();
  log_info("- read : %s\n", (state&SDL_ANDROID_EXTERNAL_STORAGE_READ) ? "yes":"no");
  log_info("- write: %s\n", (state&SDL_ANDROID_EXTERNAL_STORAGE_WRITE) ? "yes":"no");

  /* SDL_AndroidGetInternalStoragePath() == /data/data/org.freedink/files/ */
  log_info("Android internal storage: %s\n", SDL_AndroidGetInternalStoragePath());

  if (chdir(SDL_AndroidGetExternalStoragePath()) < 0)
    log_error("Could not chdir to '%s': %s'",
	      SDL_AndroidGetExternalStoragePath(),
	      strerror(errno));

  android_set_LANG();
  log_environment();
#elif defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
  /* .exe's directory */
  log_info("Woe exe dir: %s\n", SDL_GetBasePath());
  if (chdir(SDL_GetBasePath()) < 0)
    log_error("Could not chdir to '%s': %s'",
	      SDL_GetBasePath(),
	      strerror(errno));
#endif
}

/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void App::print_version() {
	printf("%s %s\n", PACKAGE_NAME, VERSION);
	printf("FreeDink is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions; see the GNU GPL for details.\n");
	printf("http://gnu.org/licenses/gpl.html\n");
	printf("There is NO WARRANTY, to the extent permitted by law.\n");
	exit(EXIT_SUCCESS);
}


/**
 * Prints the version on the standard ouput. Based on the homonymous
 * function from ratpoison
 */
void App::print_help(int argc, char *argv[])
{
  printf(_("Usage: %s [OPTIONS]...\n"), argv[0]);
  printf("%s\n", description.c_str());
  printf("\n");
  /* TODO: Display the default configuration here */
  printf(_("  -h, --help            Display this help screen\n"));
  printf(_("  -v, --version         Display the version\n"));
  printf("\n");
  printf(_("  -g, --game <dir>      Specify a DMod directory\n"));
  printf(_("  -r, --refdir <dir>    Specify base directory for dink/graphics, D-Mods, etc.\n"));
  printf("\n");
  printf(_("  -d, --debug           Explain what is being done\n"));
  printf(_("  -i, --noini           Do not attempt to write dinksmallwood.ini\n"));
  printf(_("  -j, --nojoy           Do not attempt to use joystick\n"));
  printf(_("  -s, --nosound         Do not play sound\n"));
  printf(_("  -t, --truecolor       Allow more colours (for recent D-Mod graphics)\n"));
  printf(_("  -w, --window          Use windowed mode instead of screen mode\n"));
  printf(_("  -7, --v1.07           Enable v1.07 compatibility mode\n"));
  printf(_("  -S, --software-rendering  Don't use OpenGL\n"));
  printf("\n");
  // Tentative option names:
  //printf(_("  --dinkgl              Full OpenGL acceleration\n"));
  //printf(_("  --soft                Semi-accelerated rendering with software surfaces\n"));

  /* printf ("Type 'info freedink' for more information\n"); */

  /* TRANSLATORS: The placeholder indicates the bug-reporting address
     for this package.  Please add _another line_ saying "Report
     translation bugs to <...>\n" with the address for translation
     bugs (typically your translation team's web or email
     address).  */
  printf(_("Report bugs to %s.\n"), PACKAGE_BUGREPORT);

  exit(EXIT_SUCCESS);
}


/**
 * Initialize gettext as early as possible
 */
App::App() :
  splash_path(NULL),
  g_b_no_write_ini(0),
  opt_version(108),
  dinkini_playmidi(false),
  dinkgl(true), windowed(false) {
  /* chdir to resource paths under woe&android */
  app_chdir();

  /* Reset debug levels */
  log_debug_off();

  /** i18n **/
  /* Only using LC_MESSAGES because LC_CTYPE (part of LC_ALL) may
     bring locale-specific behavior in the DinkC parsers. If that's a
     problem we may use some gnulib modules
     (cf. (gettext.info.gz)Triggering) */
  /* Ex. with scanf("%f"...):
     LANG=C            1.1 -> 1.100000
     LANG=C            1,1 -> 1.000000
     LANG=fr_FR.UTF-8  1.1 -> 1,000000
     LANG=fr_FR.UTF-8  1,1 -> 1,100000 */
  /* setlocale (LC_ALL, ""); */
  // log_info("LANG=%s", getenv("LANG"));  // no debug log yet
  setlocale(LC_MESSAGES, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bindtextdomain(PACKAGE "-gnulib", LOCALEDIR);
  textdomain(PACKAGE);

  /* SDL can display messages in either ASCII or UTF-8. Thus we need
     gettext to output translations in UTF-8. */
  /* That's a problem for console messages in locales that are not
     UTF-8-encoded. If this is really a problem, we'll have to perform
     some character conversion directly. We can't create a separate
     message catalog for those, because several console messages are
     also displayed on the SDL screen and need a separate conversion
     anyway. */
  bind_textdomain_codeset(PACKAGE, "UTF-8");
}

/**
 * Check the command line arguments and initialize the required global
 * variables
 */
bool App::check_arg(int argc, char *argv[]) {
	int c;
	char *refdir_opt = NULL;
	char *dmoddir_opt = NULL;
	int debug_p = 0;
	joystick = 1;
	
	/* Options '-debug', '-game', '-noini', '-nojoy', '-nosound' and
	   '-window' (with one dash '-' only) are required to maintain
	   backward compatibility with the original game */
	struct option long_options[] =
		{
			{"debug",     no_argument,       NULL, 'd'},
			{"refdir",    required_argument, NULL, 'r'},
			{"game",      required_argument, NULL, 'g'},
			{"help",      no_argument,       NULL, 'h'},
			{"noini",     no_argument,       NULL, 'i'},
			{"nojoy",     no_argument,       NULL, 'j'},
			{"nosound",   no_argument,       NULL, 's'},
			{"version",   no_argument,       NULL, 'v'},
			{"window",    no_argument,       NULL, 'w'},
			{"v1.07",     no_argument,       NULL, '7'},
			{"truecolor", no_argument,       NULL, 't'},
			{"nomovie"  , no_argument,       NULL, ','},
			{"software-rendering", no_argument, NULL, 'S'},
			{0, 0, 0, 0}
		};
	
	char short_options[] = "dr:g:hijsvw7tS";

	/* Allow parsing options multiple times, e.g. in EMSCRIPTEN */
	optind = 0;
	truecolor = 0;

	/* Loop through each argument */
	while ((c = getopt_long_only (argc, argv, short_options, long_options, NULL)) != EOF)
    {
		switch (c) {
		case 'd':
			debug_p = 1;
			/* Enable early debugging, before we can locate DEBUG.txt */
			SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
			break;
		case 'r':
			refdir_opt = strdup(optarg);
			break;
		case 'g':
			dmoddir_opt = strdup(optarg);
			break;
		case 'h':
			print_help(argc, argv);
			break;
		case 'j':
			joystick = 0;
			break;
		case 'i':
			g_b_no_write_ini = 1;
			break;
		case 's':
		  sound_on = 0;
		  break;
		case 'v':
			print_version();
			break;
		case 'w':
			windowed = true;
			break;
		case '7':
			opt_version = 107;
			break;
		case 't':
			truecolor = 1;
			break;
		case 'S':
			dinkgl = false;
			break;
		case ',':
			printf(_("Note: -nomovie is accepted for compatibility, but has no effect.\n"));
			break;
		default:
			return false;
		}
    }

	if (optind < argc) {
		fprintf(stderr, "Invalid additional argument: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		printf(" (did you forget '--game'?)\n");
		return false;
	}
	
	log_init();
	
	if (!paths_init(argv[0], refdir_opt, dmoddir_opt)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, PACKAGE_STRING,
								 log_getLastLog(), NULL);
		return false;
	}

	free(refdir_opt);
	free(dmoddir_opt);
	
	if (debug_p == 1)
		{
			/* Remove DEBUG.TXT when starting Dink (but not when toggling debug) */
			char* fullpath = paths_dmodfile("DEBUG.TXT");
			remove(fullpath);
			log_set_output_file(fullpath);
			free(fullpath);
			
			log_debug_on();
    }
	
	log_info("Game directory is '%s'.", paths_getdmoddir());
	return true;
}

/* freedink and freedinkedit's common init procedure. This procedure
   will also initialize each subsystem as needed (eg InitSound) */
int App::main(int argc, char *argv[]) {
  if (!check_arg(argc, argv))
    return EXIT_FAILURE;

#ifdef __ANDROID__
  //log_debug_on();
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
#endif
  
  /* SDL */
  /* Init timer subsystem */
#ifndef __EMSCRIPTEN__
  if (SDL_Init(SDL_INIT_TIMER) == -1) {
    log_error("Timer initialization error: %s\n", SDL_GetError());
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, PACKAGE_STRING,
								 log_getLastLog(), NULL);
    return EXIT_FAILURE;
  }
#endif

  /* Quits in case we couldn't do it properly first (i.e. attempt to
     avoid stucking the user in 640x480 when crashing) */
  //atexit(app_quit);

  /* GFX */
  if (gfx_init(dinkgl, windowed, splash_path) < 0) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, PACKAGE_STRING,
							 log_getLastLog(), NULL);
	return EXIT_FAILURE;
  }
  g_dmod.load(paths_getdmoddir());
  
  /* Joystick */
  input_init();

  /* SFX & BGM */
  sfx_init();

  // D-Mod loading
  // TODO: move to a separate D-Mod object
  {
	  /** localization (l10n) for this D-Mod's .mo (after options are parsed) */
	  char* dmod_localedir = paths_dmodfile("l10n");
	  log_info("localedir: %s", LOCALEDIR);
	  log_info("localedir for dmod: %s", dmod_localedir);
	  // get translations in the D-Mod's "l10n" folder; for Dink
	  // itself, it would be worth adding a fallback to LOCALEDIR, in
	  // case we try to fix AppStream's 'gettext-data-not-found'
	  bindtextdomain(paths_getdmodname(), dmod_localedir);
	  bind_textdomain_codeset(paths_getdmodname(), "UTF-8");
	  free(dmod_localedir);

	  int nb_idata = (opt_version >= 108) ? 1000 : 600;
	  dinkini_init(nb_idata);
	  
	  live_screen_init();
	  
	  //dinks normal walk
	  log_info("Loading batch...");
	  load_batch(dinkini_playmidi);
	  log_info(" done!");
	  
	  log_info("Loading hard...");
	  load_hard();
	  log_info(" done!");
	  
	  log_info("World data....");
	  g_dmod.map.load();
	  log_info(" done!");
  }

  init();

  loop();

  return EXIT_SUCCESS;
}


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
void myemscripten_loop(void* arg) {
	App* app = (App*)arg;
	app->one_iter();
}
#endif

void App::loop() {
	/* Main loop */
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(myemscripten_loop, this, 0, 1);
#else
	while(run) {
		one_iter();
    }
#endif
}

void App::one_iter() {
		/* Controller: dispatch events */
		SDL_Event event;
		SDL_Event* ev = &event;
		input_reset();
		while (SDL_PollEvent(ev)) {
			switch(ev->type)
				{
				case SDL_QUIT:
					run = 0;
					break;
				default:
					input(ev);
					break;
				}
		}
		
		/* Main app logic */
		logic();
		
		/* Clean-up finished sounds: normally this is done by
		   SDL_mixer but since we're using effects tricks to
		   stream&resample sounds, we need to do this manually. */
		sfx_cleanup_finished_channels();

#ifdef __EMSCRIPTEN__
		if (!run) {
			delete this;
		}
#endif
}

/**
 * Release all objects we use
 */
App::~App() {
	SDL_Event ev;
	ev.type = SDL_QUIT;
	SDL_PushEvent(&ev);
	
	dinkini_quit();
	
	sfx_quit();
	
	input_quit();

	g_dmod.unload();
	gfx_quit();
	
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_QuitSubSystem(SDL_INIT_TIMER);
	
	SDL_Quit();
	
	paths_quit();

	log_quit();

#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();
	EM_ASM( Module.onExit() );
#endif
}
