/**
 * Game inventory

 * Copyright (C) 2014, 2015  Sylvain Beucler

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

#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "game_state.h"

extern void add_item(char name[10], int mseq, int mframe, enum item_type type);
extern void kill_item_script(char* name);
extern void kill_mitem_script(char* name);
extern void kill_cur_item( void );
extern void kill_cur_magic( void );

extern void process_item();

#endif
