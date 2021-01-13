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
 * Weapon's crosshair
 *****************************************************************************/

#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "graphic/surface.h"
#include <WARMUX_base.h>
#include <WARMUX_point.h>

class CrossHair
{
private:
  bool enable;
  bool hidden;

private:
  Surface image;
  Point2i crosshair_position;

public:
  CrossHair();

  void Reset() const;

  // Refresh crosshair angle
  void Refresh(Double angle);

  // Draw crosshair
  void Draw() const;

  void SetActive(bool enable);
  bool IsActive() const { return enable; };

  // Use it only for a few seconds (character walking, jumping, ...)
  void Hide();
  void Show();
};

#endif /* CROSSHAIR_H */
