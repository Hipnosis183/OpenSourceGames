/**
 * Background music (currently .midi's and audio CDs)

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2014, 2015  Sylvain Beucler

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

/* CD functions */
#include "SDL.h"
/* MIDI functions */
#include "SDL_mixer.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strcasecmp */

#include "bgm.h"
#include "io_util.h"
#include "str_util.h"
#include "paths.h"
#include "log.h"
#include "sfx.h" /* sound_on */

/* Current background music (not cd) */
static Mix_Music *music_data = NULL;
static char* last_midi = NULL;
static int loop_midi = 0;
int midi_active = 1;

/*
 * MIDI functions
 */

/**
 * Returns whether the background music is currently playing
 */
int something_playing(void)
{
  return Mix_PlayingMusic();
}


/**
 * Clean-up music when it's finished or manually halted
 */
static void callback_HookMusicFinished()
{
  if (music_data != NULL)
    Mix_FreeMusic (music_data);
}

/**
 * Thing to play the midi
 */
int PlayMidi(char *midi_filename)
{
  char relpath[256];
  char *fullpath = NULL;
  
  /* no midi stuff right now */
  if (sound_on == /*false*/0)
    return 1;
  
  /* Do nothing if the same midi is already playing */
  /* TODO: Does not differentiate midi and ./midi, qsf\\midi and
     qsf/midi... Ok, midi is supposed to be just a number, but..*/
  if (last_midi != NULL && compare(last_midi, midi_filename)
      && something_playing())
    {
      log_info("I think %s is already playing, I should skip it...", midi_filename);
      return 0;
    }

      
  // Attempt to play .ogg in addition to .mid, if playing a ".*\.mid$"
  char* oggv_filename = NULL;
  int pos = strlen(midi_filename) - strlen(".mid");
  if (strcasecmp(midi_filename + pos, ".mid") == 0)
    {
      oggv_filename = strdup(midi_filename);
      strcpy(oggv_filename + pos, ".ogg");
    }

  /* Try to load the ogg vorbis or midi in the DMod or the main game */
  int exists = 0;
  fullpath = (char*)malloc(1);
  if (!exists && oggv_filename != NULL)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", oggv_filename);
      fullpath = paths_dmodfile(relpath);
      exists = exist(fullpath);
    }
  if (!exists)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", midi_filename);
      fullpath = paths_dmodfile(relpath);
      exists = exist(fullpath);
    }
  if (!exists && oggv_filename != NULL)
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", oggv_filename);
      fullpath = paths_fallbackfile(relpath);
      exists = exist(fullpath);
    }
  if (!exist(fullpath))
    {
      free(fullpath);
      sprintf(relpath, "sound/%s", midi_filename);
      fullpath = paths_fallbackfile(relpath);
      exists = exist(fullpath);
    }
  free(oggv_filename);

  if (!exist(fullpath))
    {
      free(fullpath);
      log_warn("Error playing midi %s, doesn't exist in any dir.", midi_filename);
      return 0;
    }


  /* Save the midi currently playing */
  if (last_midi != NULL)
    free(last_midi);
  last_midi = strdup(midi_filename);


  /* Stop whatever is playing before we play something else. */
  Mix_HaltMusic ();
  
  /* Load the file */
  if ((music_data = Mix_LoadMUS(fullpath)) == NULL)
    {
      log_warn("Unable to play '%s': %s", fullpath, Mix_GetError());
      free(fullpath);
      return 0;
    }

  /* Play it */
  Mix_HookMusicFinished(callback_HookMusicFinished);
  Mix_PlayMusic (music_data, (loop_midi == 1) ? -1 : 1);

  free(fullpath);
  return 1;
}



/**
 * Pause midi file if we're not already paused
 */
/* TODO: should be used when player hits 'n' or alt+'n' - but I never
   got it to work in the original game */
int PauseMidi()
{
  Mix_PauseMusic();
  return 1;
}
	
/**
 * Resumes playing of a midi file
 */
/* TODO: should be used when player hits 'b' or alt+'b' - but I never
   got it to work in the original game */
int
ResumeMidi()
{
  Mix_ResumeMusic();
  return 1;
}


/**
 * Stops a midi file playing	
 */
// TODO: rename *Midi to *BGM (BackGround Music) - we may support
// background .ogg's in the future
// DinkC binding: stopmidi()
int StopMidi()
{
  Mix_HaltMusic(); // return always 0
  return 1;
}

/**
 * Initialize BackGround Music (currently nothing to do, MIDI init is
 * done with SDL_INIT_AUDIO in sfx.c).
 */
void bgm_init(void)
{
}

void bgm_quit(void)
{
  Mix_HaltMusic();
  if (last_midi != NULL)
    free(last_midi);
  last_midi = NULL;
}



/** DinkC procedures **/
void loopmidi(int arg_loop_midi)
{
  if (arg_loop_midi > 0)
    loop_midi = 1;
  else
    loop_midi = 0;
}
