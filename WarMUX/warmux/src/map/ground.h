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
 * Ground
 *****************************************************************************/

#ifndef GROUND_H
#define GROUND_H

#include <list>
#include "tile.h"

class Ground : public Tile
{
  bool open;
  bool AdjacentPoint(int x,int y, int & p_x,int & p_y,
                     int pas_bon_x,int pas_bon_y) const;
  Point2i lastPos;

  uint alpha_threshold; //Keep redundant to rise performance

public:
  void Init();
  void Draw(bool redraw_all);
  void Reset();
  void Free() { FreeMem(); }

  // Is the playground "open" ?
  bool IsOpen() const { return open; }

  //returns the angle formed by the ground tangent au terrain
  Double Tangent(int x,int y) const ;

  void RedrawParticleList(const std::list<Rectanglei>& list) const;
};

#endif
