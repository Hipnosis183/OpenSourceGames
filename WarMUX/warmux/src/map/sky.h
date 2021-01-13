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
 * Sky: background of the map
 *****************************************************************************/

#ifndef SKY_H
#define SKY_H

#include <list>
#include <vector>
#include <WARMUX_base.h>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>

class Surface;

class Sky
{
  // Only keep reference of already loaded images
  std::vector<const Surface*> images;
  Point2i last_pos;

  void RedrawParticleList(const std::list<Rectanglei>& list) const;
  void RedrawParticle(const Rectanglei &particle, const std::vector<Point2i>& sky_pos) const;
  Point2i GetSkyPos(uint layer) const;

public:
  void Init();
  void Reset();
  void Draw(bool redraw_all);
  void Free() { images.clear(); }
};

#endif
