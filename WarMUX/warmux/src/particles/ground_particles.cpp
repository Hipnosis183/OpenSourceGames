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
 * Particle Engine
 *****************************************************************************/

#include "particles/ground_particles.h"
#include "particles/particle.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "map/map.h"

GroundParticle::GroundParticle(const Point2i& size, const Point2i& position) :
  Particle("ground_particle")
{
  SetCollisionModel(false, false, false);
  SetSize(Point2i(1,1));
  m_initial_time_to_live = 1; // used as a boolean because we redefine Refresh!
  m_time_left_to_live = 1; // used as a boolean because we redefine Refresh!
  image = NULL;

  Rectanglei rec;
  rec.SetPosition( position - size / 2);
  rec.SetSize( size );
  image = new Sprite(GetWorld().ground.GetPart(rec));
}

void GroundParticle::Refresh()
{
  UpdatePosition();
  image->SetRotation_rad(GameTime::GetInstance()->Read() / (Double)180.0 * PI);
  image->Update();
  if(IsOutsideWorld()) {
    m_time_left_to_live = 0;
  }
}
