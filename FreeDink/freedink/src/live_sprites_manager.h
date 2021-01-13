/**
 * Live sprites manager

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2007, 2008, 2009, 2014, 2015  Sylvain Beucler

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

#ifndef _LIVE_SPRITES_MANAGER_H
#define _LIVE_SPRITES_MANAGER_H

#include "live_sprite.h"

#define MAX_SPRITES_AT_ONCE 300
extern struct sp spr[];
extern int last_sprite_created;

extern void live_sprites_manager_init();

extern bool lsm_isValidSprite(int sprite);
extern void lsm_remove_sprite(int sprite);

extern int add_sprite(int x1, int y, int brain,int pseq, int pframe );
extern int add_sprite_dumb(int x1, int y, int brain,int pseq, int pframe,int size);
extern void random_blood(int mx, int my, int sprite);

extern void kill_text_owned_by(int sprite);
extern int does_sprite_have_text(int sprite);
extern /*bool*/int text_owned_by(int sprite);

extern void lsm_kill_all_nonlive_sprites();
extern void get_last_sprite(void);
extern int find_sprite(int editor_sprite);

#endif
