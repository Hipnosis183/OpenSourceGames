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

#ifndef _BRAINS_H
#define _BRAINS_H

extern void bounce_brain(int h);
extern void button_brain(int h);
extern void pill_brain(int h);
extern void duck_brain(int h);
extern void human_brain(int h);
extern void dragon_brain(int h);
extern void missile_brain(int h, int repeat);
extern void missile_brain_expire(int h);
extern void mouse_brain(int h);
extern void no_brain(int h);
extern void one_time_brain(int h);
extern void one_time_brain_for_real(int h);
extern void people_brain(int h);
extern void pig_brain(int h);
extern void repeat_brain(int h);
extern void scale_brain(int h);
extern void shadow_brain(int h);
extern void text_brain(int h);

#endif
