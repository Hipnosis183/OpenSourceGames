/**
 * Dink.ini - loading graphics

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2008, 2009, 2010, 2015  Sylvain Beucler

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

#include <stdlib.h>
#include "paths.h"
#include "dinkini.h"
#include "str_util.h"
#include "log.h"

// TODO: break deps
#include "bgm.h"

#include "gfx_sprites.h"

struct idata
{
  enum idata_type type;
  int seq;
  int frame;
  int xoffset, yoffset;
  rect hardbox;
};
static struct idata *id;
int nb_idata = 0;

/**
 * Allocate idata
 */
void dinkini_init(int nb_idata_param)
{
  nb_idata = nb_idata_param;
  id = (struct idata*)calloc(sizeof(struct idata), nb_idata);
}

/**
 * Deallocate idata
 */
void dinkini_quit()
{
  if (id != NULL)
    free(id);
  id = NULL;
}

/**
 * Store sprite instructions from dink.ini
 */
void make_idata(enum idata_type type, int myseq, int myframe, int xoffset, int yoffset, rect crect)
{
  int i;
  for (i = 1; i < nb_idata; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	{
	  //found empty one
	  id[i].type = type;
	  id[i].seq = myseq;
	  id[i].frame = myframe;
	  id[i].xoffset = xoffset;
	  id[i].yoffset = yoffset;
	  rect_copy(&id[i].hardbox, &crect);
	  
	  return;
	}
    }
  
  log_error("Out of idata spots (max is %d), no more sprite corrections can be allowed.", nb_idata);
}

/**
 * Interpret/execute sprite instructions
 */
void program_idata(void)
{
  int i;
  for (i = 1; i < nb_idata; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	return;

      if (id[i].type == IDATA_SPRITE_INFO)
	{
	  k[seq[id[i].seq].frame[id[i].frame]].xoffset = id[i].xoffset;
	  k[seq[id[i].seq].frame[id[i].frame]].yoffset = id[i].yoffset;
	  rect_copy(&k[seq[id[i].seq].frame[id[i].frame]].hardbox, &id[i].hardbox);
	  
	  // Msg("Programming idata type %d in %d...Seq %d Frame %d (Hardbox is %d %d %d %d)", id[i].type, i,
	  //     id[i].seq, id[i].frame, id[i].hardbox.left,id[i].hardbox.right, id[i].hardbox.top, id[i].hardbox.bottom);
	}
      
      if (id[i].type == IDATA_FRAME_SPECIAL)
	{
	  //set special
	  seq[id[i].seq].special[id[i].frame] = id[i].xoffset;
	}
      if (id[i].type == IDATA_FRAME_DELAY)
	{
	  //set delay
	  seq[id[i].seq].delay[id[i].frame] = id[i].xoffset;
	}
      
      if (id[i].type == IDATA_FRAME_FRAME)
	{
	  if (id[i].xoffset == -1)
	    seq[id[i].seq].frame[id[i].frame] = -1;
	  else
	    seq[id[i].seq].frame[id[i].frame] = seq[id[i].xoffset].frame[id[i].yoffset];
	}
    }
}


/**
 * Parse a dink.ini line, and store instructions for later processing
 * (used in game initialization through 'load_batch')
 */
void pre_figure_out(char* line, bool playmidi)
{
  int i;
  char* ev[10];
  memset(&ev, 0, sizeof(ev));
  for (i = 0; i < 10; i++)
    ev[i] = separate_string(line, i+1, ' ');
  char *command = ev[0];

  // PLAYMIDI  filename
  if (compare(command, "playmidi"))
    {
      char* midi_filename = ev[1];
      if (playmidi)
	PlayMidi(midi_filename);
    }

  // LOAD_SEQUENCE_NOW  path  seq  BLACK
  // LOAD_SEQUENCE_NOW  path  seq  LEFTALIGN
  // LOAD_SEQUENCE_NOW  path  seq  NOTANIM
  // LOAD_SEQUENCE_NOW  path  seq  speed
  // LOAD_SEQUENCE_NOW  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  else if (compare(command, "LOAD_SEQUENCE_NOW"))
    {
      rect hardbox;
      memset(&hardbox, 0, sizeof(rect));

      int myseq = atol(ev[2]);
      seq[myseq].is_active = 1;
      seq_set_ini(myseq, line);

      int flags = 0;
      if (compare(ev[3], "BLACK"))
	{
	  flags = DINKINI_NOTANIM | DINKINI_BLACK;
	}
      else if (compare(ev[3], "LEFTALIGN"))
	{
	  flags = DINKINI_LEFTALIGN;
	}
      else if (compare(ev[3], "NOTANIM"))
	{
	  //not an animation!
	  flags = 0;
	}
      else
	{
	  //yes, an animation!
	  hardbox.left = atol(ev[6]);
	  hardbox.top = atol(ev[7]);
	  hardbox.right = atol(ev[8]);
	  hardbox.bottom = atol(ev[9]);

	  flags = DINKINI_NOTANIM;
	}

      load_sprites(ev[1],atol(ev[2]),atol(ev[3]),atol(ev[4]),atol(ev[5]),
		   hardbox, flags);


      /* In the original engine, due to a bug, make_idata() modifies
	 unused sequence #0, but this isn't really important because
	 sequence was already configured in was already done in
	 'load_sprites'. This is consistent with 'figure_out', which
	 doesn't call 'make_idata' at all. */
      /* We still call 'make_idata' for compatibility, to use the same
	 number of idata, hence preserving the same max_idata. */
      make_idata(IDATA_SPRITE_INFO, 0,0, 0,0, hardbox);
    }

  // LOAD_SEQUENCE  path  seq  BLACK
  // LOAD_SEQUENCE  path  seq  LEFTALIGN
  // LOAD_SEQUENCE  path  seq  NOTANIM
  // LOAD_SEQUENCE  path  seq  speed
  // LOAD_SEQUENCE  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  else if (compare(command, "LOAD_SEQUENCE"))
    {
      int myseq = atol(ev[2]);
      seq_set_ini(myseq, line);
      seq[myseq].is_active = 1;
    }
  
  else if (compare(command, "SET_SPRITE_INFO"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      //if (k[seq[myseq].frame[myframe]].frame = 0) Msg("Changing sprite that doesn't exist...");
      
      rect hardbox;
      int myseq = atol(ev[1]);
      int myframe = atol(ev[2]);
      rect_set(&hardbox, atol(ev[5]), atol(ev[6]), atol(ev[7]), atol(ev[8]));
      make_idata(IDATA_SPRITE_INFO, myseq, myframe,atol(ev[3]), atol(ev[4]),hardbox);
    }
  
  else if (compare(command, "SET_FRAME_SPECIAL"))
    {
      rect hardbox;
      int myseq = atol(ev[1]);
      int myframe = atol(ev[2]);
      int special = atol(ev[3]);
      make_idata(IDATA_FRAME_SPECIAL, myseq, myframe, special, 0, hardbox);
    }
  
  else if (compare(command, "SET_FRAME_DELAY"))
    {
      rect hardbox;
      int myseq = atol(ev[1]);
      int myframe = atol(ev[2]);
      int delay = atol(ev[3]);
      make_idata(IDATA_FRAME_DELAY, myseq, myframe, delay, 0, hardbox);
    }
  
  // SET_FRAME_FRAME  seq frame  new_seq new_frame
  // SET_FRAME_FRAME  seq frame  -1
  else if (compare(command, "SET_FRAME_FRAME"))
    {
      rect hardbox;
      int myseq = atol(ev[1]);
      int myframe = atol(ev[2]);
      int new_seq = atol(ev[3]);
      int new_frame = atol(ev[4]);
      
      make_idata(IDATA_FRAME_FRAME, myseq, myframe, new_seq, new_frame, hardbox);
    }

  /* Clean-up */
  for (i = 0; i < 10; i++)
    free(ev[i]);
}


/* Parse dink.ini */
void load_batch(bool playmidi)
{
  FILE *in = NULL;
  char line[255];
  
  log_info("Loading dink.ini");
  /* Open the text file in binary mode, so it's read the same way
     under different OSes (Unix has no text mode) */
  if ((in = paths_dmodfile_fopen("dink.ini", "rb")) == NULL)
    log_error("Error opening dink.ini for reading.");
  else
    {
      while(fgets(line, 255, in) != NULL) 
	{
		pre_figure_out(line, playmidi);
	  /* printf("[pre_figure_out] %s", line); */
	}
      fclose(in);
    }

  program_idata();
}



/**
 * Parse a delayed seq[].ini or a DinkC init("...") , and act
 * immediately
 */
void figure_out(char* line)
{
  int myseq = 0, myframe = 0;
  int special = 0;
  int special2 = 0;
  int i;
  char* ev[10];
  memset(&ev, 0, sizeof(ev));
  for (i = 0; i < 10; i++)
    {
      ev[i] = separate_string(line, i+1, ' ');
      if (ev[i] == NULL)
	ev[i] = strdup("");
    }
  char *command = ev[0];

  // LOAD_SEQUENCE_NOW  path  seq  BLACK
  // LOAD_SEQUENCE_NOW  path  seq  LEFTALIGN
  // LOAD_SEQUENCE_NOW  path  seq  NOTANIM
  // LOAD_SEQUENCE_NOW  path  seq  speed  offsetx offsety  hard.left hard.top hard.right hard.bottom
  if (compare(command, "LOAD_SEQUENCE_NOW") ||
      compare(command, "LOAD_SEQUENCE"))
    {
      rect hardbox;
      memset(&hardbox, 0, sizeof(rect));

      int myseq = atol(ev[2]);
      seq[myseq].is_active = 1;
      seq_set_ini(myseq, line);

      int flags = 0;

      if (compare(ev[3], "BLACK"))
	{
	  flags = DINKINI_NOTANIM | DINKINI_BLACK;
	}
      else if (compare(ev[3], "LEFTALIGN"))
	{
	  flags = DINKINI_LEFTALIGN;
	}
      else if (compare(ev[3], "NOTANIM"))
	{
	  //not an animation!
	  flags = 0;
	}
      else
	{
	  //yes, an animation!
	  hardbox.left = atol(ev[6]);
	  hardbox.top = atol(ev[7]);
	  hardbox.right = atol(ev[8]);
	  hardbox.bottom = atol(ev[9]);
	  
	  flags = DINKINI_NOTANIM;
	}

      load_sprites(ev[1],atol(ev[2]),atol(ev[3]),atol(ev[4]),atol(ev[5]),
		   hardbox, flags);
      
      program_idata();
    }

  else if (compare(command, "SET_SPRITE_INFO"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[1]);
      myframe = atol(ev[2]);
      k[seq[myseq].frame[myframe]].xoffset = atol(ev[3]);
      k[seq[myseq].frame[myframe]].yoffset = atol(ev[4]);
      k[seq[myseq].frame[myframe]].hardbox.left = atol(ev[5]);
      k[seq[myseq].frame[myframe]].hardbox.top = atol(ev[6]);
      k[seq[myseq].frame[myframe]].hardbox.right = atol(ev[7]);
      k[seq[myseq].frame[myframe]].hardbox.bottom = atol(ev[8]);
    }
  
  else if (compare(command, "SET_FRAME_SPECIAL"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[1]);
      myframe = atol(ev[2]);
      special = atol(ev[3]);
      
      seq[myseq].special[myframe] = special;
      log_debug("Set special.  %d %d %d", myseq, myframe, special);
    }

  else if (compare(command, "SET_FRAME_DELAY"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[1]);
      myframe = atol(ev[2]);
      special = atol(ev[3]);
      
      seq[myseq].delay[myframe] = special;
      log_debug("Set delay.  %d %d %d",myseq, myframe, special);
    }

  else if (compare(command, "SET_FRAME_FRAME"))
    {
      //           name   seq    speed       offsetx     offsety       hardx      hardy
      myseq = atol(ev[1]);
      myframe = atol(ev[2]);
      special = atol(ev[3]);
      special2 = atol(ev[4]);
      
      if (special == -1)
	seq[myseq].frame[myframe] = special;
      else
	seq[myseq].frame[myframe] = seq[special].frame[special2];
      log_debug("Set frame.  %d %d %d", myseq, myframe, special);
    }

  /* Clean-up */
  for (i = 0; i < 10; i++)
    free(ev[i]);
}


/**
 * Load sequence in memory if not already, using cached dink.ini info
 */
void check_seq_status(int seq_no)
{
  if (seq_no > 0 && seq_no < MAX_SEQUENCES)
    {
      /* Skip empty/unused sequences */
      if (!seq[seq_no].is_active)
	return;

      if (seq[seq_no].frame[1] == 0 || GFX_k[seq[seq_no].frame[1]].k == NULL)
	figure_out(seq[seq_no].ini);
    }
  else if (seq_no > 0)
    {
      log_error("Warning: check_seq_status: invalid sequence %d", seq_no);
    }
}

/* Editor only */
void check_frame_status(int h, int frame)
{
        if (!seq[h].is_active) return;

        if (h > 0)
        {
                // Msg("Smartload: Loading seq %d..", spr[h].seq);
                if (seq[h].frame[1] == 0 || GFX_k[seq[h].frame[1]].k == NULL)
                {
                        figure_out(seq[h].ini);
                }
                else
                {
                        //it's been loaded before.. is it lost or still there?
                }
        }
}

/**
 * Load all +1->+9 sequences from base sequence 'base' in memory,
 * useful to load all of a moving sprite sequences
 */
void check_base(int base)
{
  int i;
  for (i = 1; i < 10; i++)
    if (seq[base+i].is_active)
      check_seq_status(base+i);
}
