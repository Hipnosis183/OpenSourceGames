#ifndef __ISNAN_H__
#define __ISNAN_H__

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
 * isNaN function, defined using C++ limits and not with C99
 *
 * The problem is that isnan and isfinite are part of C99 but aren't part of
 * the C++ standard (which predates C99).
 *
 *****************************************************************************/

#include <limits>

#ifdef NAN
#undef NAN // please don't use C99 NAN
#endif

static inline Double getNaN()
{
  return std::numeric_limits<Double>::quiet_NaN();
}

static inline bool isNaN(Double x)
{
  return (x != x);
}

#endif /* __ISNAN_H__ */

