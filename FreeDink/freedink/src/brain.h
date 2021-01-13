/**
 * Brains - Sprites AI

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

#ifndef _BRAIN_H
#define _BRAIN_H

extern /*bool*/int check_for_kill_script(int i);
extern void add_kill_sprite(int h);
extern int get_distance_and_dir(int h, int h1, int *dir);
extern int get_distance_and_dir_nosmooth(int h, int h1, int *dir);
extern void change_dir_to_diag( int *dir);
extern void changedir(int dir1, int k, int base);
extern void process_follow(int h);
extern void process_move(int h);
extern void draw_damage(int h);
extern void automove (int j);
extern void move(int u, int amount, char kind,  char kindy);
extern int autoreverse(int j);
extern int autoreverse_diag(int j);

#endif
