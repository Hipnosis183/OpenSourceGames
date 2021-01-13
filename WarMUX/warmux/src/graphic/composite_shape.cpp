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
 * Composite Shape. Use Polygon and Sprite to draw a shape.
 *****************************************************************************/

#include "graphic/composite_shape.h"
#include "graphic/surface.h"
#include "graphic/polygon.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/map.h"
#include "tool/affine_transform.h"

void CompositeShape::ApplyTransformation(const AffineTransform2D & trans)
{
  for(std::vector<Polygon *>::iterator poly = layers.begin();
      poly != layers.end(); poly++) {
    (*poly)->ApplyTransformation(trans);
  }
}

void CompositeShape::Draw(Surface * dest)
{
  for(std::vector<Polygon *>::iterator poly = layers.begin();
      poly != layers.end(); poly++) {
    (*poly)->Draw(dest);
  }
}

void CompositeShape::DrawOnScreen()
{
  Point2d min, max;
  Point2i tmp;
  int i = 0;
  for(std::vector<Polygon *>::iterator poly = layers.begin();
      poly != layers.end(); poly++, i++) {
    (*poly)->Draw(&GetMainWindow());
    if(i == 0) {
      min = (*poly)->GetMin();
      max = (*poly)->GetMax();
    } else {
      min = min.min((*poly)->GetMin());
      max = max.max((*poly)->GetMax());
    }
  }
  tmp = Point2i(max) - Point2i(min) + 1;
  GetWorld().ToRedrawOnScreen(Rectanglei(Point2i(min), Point2i(tmp) + 2));
}
