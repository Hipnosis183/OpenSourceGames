/**
 * Game state

 * Copyright (C) 2015  Sylvain Beucler

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

/* Engine variables directly mapped with DinkC variables */
int *pvision, *plife, *presult, *pspeed, *ptiming, *plifemax,
	*pexp, *pstrength, *pcur_weapon, *pcur_magic, *pdefense,
	*pgold, *pmagic, *plevel, *plast_text, *pmagic_level,
	*pplayer_map, *pupdate_status, *pmissile_target, *penemy_sprite,
	*pmagic_cost, *pmissle_source;

int dinkspeed = 3;
int push_active = 1;
unsigned int dink_base_push = 310;
