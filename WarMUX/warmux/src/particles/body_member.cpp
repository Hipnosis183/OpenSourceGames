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

#include "particles/body_member.h"
#include "particles/particle.h"
#include "graphic/sprite.h"
#include "network/randomsync.h"

BodyMemberParticle::BodyMemberParticle(Sprite& spr, const Point2i& position)
  : Particle("body_member_particle")
  , angle_rad(0)
{
  SetCollisionModel(true, false, false);
  m_time_left_to_live = 100;
  // Bug #17408: make sure there's an available surface for the sprite
  spr.RefreshSurface();
  image = new Sprite(spr.GetSurface());
  image->EnableCaches(false, 0); // Some generic particle code requires it to be flipped
  ASSERT(image->GetWidth() && image->GetHeight());
  SetXY(position);

  SetSize(image->GetSize());
  SetOnTop(true);
  MSG_DEBUG("random.get", "BodyMemberParticle::BodyMemberParticle(...) speed vector length");
  Double speed_vector_length = (Double)RandomSync().GetInt(10, 15);
  MSG_DEBUG("random.get", "BodyMemberParticle::BodyMemberParticle(...) speed vector angle");
  Double speed_vector_angle = - RandomSync().GetDouble(0, 3);
  SetSpeed(speed_vector_length, speed_vector_angle);
}

void BodyMemberParticle::Refresh()
{
  m_time_left_to_live--;
  UpdatePosition();

  angle_rad += GetSpeedXY().Norm() * 20;
  angle_rad = fmod(angle_rad, 2 *PI);
  //FIXME what about negatives values ? what would happen ?
  if (m_time_left_to_live < 50)
    image->SetAlpha(m_time_left_to_live / 50.0);
  image->SetRotation_rad(angle_rad);
  image->Update();
}
