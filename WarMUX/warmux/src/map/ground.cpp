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
 * Terrain de jeu.
 *****************************************************************************/

#include "map/ground.h"
#include <iostream>
#include <SDL_stdinc.h>

#include <SDL_video.h>
#include <limits.h>
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "graphic/colors.h"
#include "include/constant.h"
#include "tool/isnan.h"

void Ground::Init()
{
  std::cout << "o " << _("Ground initialization...") << ' ';
  std::cout.flush();

  // Load ground data
  InfoMapAccessor *normal = ActiveMap()->LoadData();

  //Load alpha threshold from XML
  alpha_threshold = normal->GetAlphaThreshold();

  // Check if the map is "opened"
  open = normal->IsOpened();

  const std::string& filename = normal->GetGroundFileName();
  bool ret;
  if (open) {
    ret = LoadImage(filename, alpha_threshold,
                    ActiveMap()->GetUpperLeftPad(),
                    ActiveMap()->GetLowerRightPad());
  } else {
    ret = LoadImage(filename, alpha_threshold, Point2i(), Point2i());
  }
  if (!ret)
    Error(_("Error loading ground"));

  // Check the size of the map
  ASSERT(Constants::MAP_MIN_SIZE <= GetSize());
  ASSERT(GetSizeX()*GetSizeY() <= Constants::MAP_MAX_SIZE);

  std::cout << _("done") << std::endl;
}

void Ground::Reset()
{
  Init();
  lastPos.SetValues(INT_MAX, INT_MAX);
}

/*
 * Returns the angle between the tangent at point (x,y) of the ground and
 * horizontal
 * the angle is always > 0.
 * returns -1.0 if no tangent was found (pixel (x,y) does not touch any
 * other piece of ground
 */
Double Ground::Tangent(int x,int y) const
{
  //Approximation : returns the chord instead of the tangent to the ground

  /* We try to find 2 points on the ground on each side of (x,y)
   * the points should be at the limit between land and vaccum
   * (p1 =  point on the left
   * p2 =  point on the right
   */
  Point2i p1,p2;
  if (!AdjacentPoint(x,y, p1.x,p1.y, -1,-1))
    return getNaN();

  if (!AdjacentPoint(x,y, p2.x,p2.y, p1.x,p1.y)) {
    p2.x = x;
    p2.y = y;
  }
/*
  if(p1.x == p2.x)
    return PI / 2.0;
  if(p1.y == p2.y)
    return PI;
*/
  //ASSERT (p1.x != p2.x);

  /* Double tangeante = atan((Double)(p2.y-p1.y)/(Double)(p2.x-p1.x));

  while(tangeante <= 0.0)
    tangeante += PI;
  while(tangeante > 2 * PI)
    tangeante -= PI;

  return tangeante; */

  //calculated with a good old TI-83... using table[a][b] = atan( (a-2) / (b-2) )
  static const Double table[5][5] = {
    {     QUARTER_PI,          .46364,      PI,    -.46364+M_PI, PI - QUARTER_PI},
    {         1.1071,      QUARTER_PI,      PI, PI - QUARTER_PI,     1.1071+M_PI},
    {        HALF_PI,         HALF_PI, HALF_PI,         HALF_PI,         HALF_PI},
    {   -1.1071+M_PI, PI - QUARTER_PI,      PI,      QUARTER_PI,          1.1071},
    {PI - QUARTER_PI,     -.46364+M_PI,     PI,          .46364,     QUARTER_PI}};

  assert(p2.x-p1.x >= -2 && p2.x-p1.x <= 2);
  assert(p2.y-p1.y >= -2 && p2.y-p1.y <= 2);

  return table[(p2.y-p1.y)+2][(p2.x-p1.x)+2];
}

bool Ground::AdjacentPoint(int x,int y,  int & p_x,int & p_y,
                           int bad_x,int bad_y) const
{
  //Look for a pixel around (x,y) that is at the edge of the ground
  //and vaccum
  //return true (and set p_x and p_y) if this point have been found
  if (GetWorld().IsOutsideWorld(Point2i(x-1,y))
      || GetWorld().IsOutsideWorld(Point2i(x+1,y))
      || GetWorld().IsOutsideWorld(Point2i(x,y-1))
      || GetWorld().IsOutsideWorld(Point2i(x,y+1))) {
    return false;
  }

  // check adjacents pixels one by one:
  //upper right pixel
  if (x-1 != bad_x || y-1 != bad_y)
    if (!IsEmpty(Point2i(x-1,y-1)) &&
        (IsEmpty(Point2i(x-1,y)) ||
        IsEmpty(Point2i(x,y-1)))) {
      p_x=x-1;
      p_y=y-1;
      return true;
    }

  //upper pixel
  if (x != bad_x || y-1 != bad_y)
    if (!IsEmpty(Point2i(x,y-1)) &&
        (IsEmpty(Point2i(x-1,y-1)) ||
        IsEmpty(Point2i(x+1,y-1)))) {
      p_x=x;
      p_y=y-1;
      return true;
    }

  //upper right pixel
  if (x+1 != bad_x || y-1 != bad_y)
    if (!IsEmpty(Point2i(x+1,y-1)) &&
        (IsEmpty(Point2i(x,y-1)) ||
        IsEmpty(Point2i(x+1,y)))) {
      p_x=x+1;
      p_y=y-1;
      return true;
    }

  //pixel at the right
  if (x+1 != bad_x || y != bad_y)
    if (!IsEmpty(Point2i(x+1,y)) &&
        (IsEmpty(Point2i(x+1,y-1)) ||
        IsEmpty(Point2i(x,y+1)))) {
      p_x=x+1;
      p_y=y;
      return true;
    }

  //bottom right pixel
  if (x+1 != bad_x || y+1 != bad_y)
    if (!IsEmpty(Point2i(x+1,y+1)) &&
        (IsEmpty(Point2i(x+1,y)) ||
        IsEmpty(Point2i(x,y+1)))) {
      p_x=x+1;
      p_y=y+1;
      return true;
    }

  //bottom pixel
  if (x != bad_x || y+1 != bad_y)
    if (!IsEmpty(Point2i(x,y+1))
        &&(IsEmpty(Point2i(x-1,y+1))
        || IsEmpty(Point2i(x+1,y+1)))) {
      p_x=x;
      p_y=y+1;
      return true;
    }

  //bottom left pixel
  if (x-1 != bad_x || y+1 != bad_y)
    if (!IsEmpty(Point2i(x-1,y+1))
        &&(IsEmpty(Point2i(x-1,y))
        || IsEmpty(Point2i(x,y+1)))) {
      p_x=x-1;
      p_y=y+1;
      return true;
    }

  //pixel at left
  if (x-1 == bad_x && y == bad_y)
    if (!IsEmpty(Point2i(x-1,y))
        &&(IsEmpty(Point2i(x-1,y-1))
        || IsEmpty(Point2i(x-1,y+1)))) {
      p_x=x-1;
      p_y=y;
      return true;
    }

  return false;
}

void Ground::Draw(bool redraw_all)
{
  Surface& window = GetMainWindow();

  Point2i cPos = Camera::GetInstance()->GetPosition();
  Point2i windowSize = window.GetSize();
  Point2i margin = (windowSize - GetSize())/2;

  if (Camera::GetInstance()->HasFixedX()) { // ground is less wide than screen !
    window.BoxColor(Rectanglei(0, 0, margin.x, windowSize.y), black_color);
    window.BoxColor(Rectanglei(windowSize.x - margin.x, 0, margin.x, windowSize.y), black_color);
  }

  if (Camera::GetInstance()->HasFixedY()) { // ground is less wide than screen !
    window.BoxColor(Rectanglei(0, 0, windowSize.x, margin.y), black_color);
    window.BoxColor(Rectanglei(0, windowSize.y - margin.y, windowSize.x, margin.y), black_color);
  }

  if (lastPos != cPos || redraw_all) {
    lastPos = cPos;
    DrawTile();
    return;
  }

  RedrawParticleList(*GetWorld().to_redraw_now);

  // Draw on top of sky (redisplayed on top of particles)
  RedrawParticleList(*GetWorld().to_redraw_particles_now);

  // Draw on top of new position of particles (redisplayed on top of particles)
  RedrawParticleList(*GetWorld().to_redraw_particles);

  CheckPreview();
}

void Ground::RedrawParticleList(const std::list<Rectanglei>& list) const
{
  std::list<Rectanglei>::const_iterator it;
  const Camera& cam = Camera::GetConstRef();

  for (it = list.begin(); it != list.end(); ++it) {
    Rectanglei rect = *it;
    rect.Clip(cam);
    if (!rect.IsSizeZero())
      DrawTile_Clipped(rect);
  }
}
