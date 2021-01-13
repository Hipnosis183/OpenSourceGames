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
 * Abstract class used for physical object (object with a size, mass,
 * etc.). This object can have differents state : ready, is moving, or ghost
 * (is outside of the world).
 *
 * You can : make the object move (with collision test), change state, etc.
 * If the object go outside of the world, it become a ghost.
 *****************************************************************************/

#include <stdlib.h>
#include <iostream>

#include <WARMUX_debug.h>

#include "object/physics.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game_time.h"
#include "map/wind.h"
#include "tool/isnan.h"
#include "tool/math_tools.h"
#include "tool/string_tools.h"

// Physical constants
static const Double STOP_REBOUND_LIMIT = 0.5;
static const Double AIR_RESISTANCE_FACTOR = 40.0;
#define PHYS_DELTA_T_MS  20
static const Double PHYS_DELTA_T = PHYS_DELTA_T_MS / 1000.0;         // Physical simulation time step
static const Double PENDULUM_REBOUND_FACTOR = 0.8;
static const Double HALF_PI_PLUS    = HALF_PI+(Double)0.3;
static const Double HALF_PI_MINUS   = HALF_PI-(Double)0.3 ;

Physics::Physics ():
  m_motion_type(NoMotion),
  m_pos_x(),
  m_pos_y(),
  m_extern_force(),
  m_last_move(GameTime::GetInstance()->Read()),
  m_last_physical_engine_run(GameTime::GetInstance()->Read()),
  m_phys_width(),
  m_phys_height(),
  m_fix_point_gnd(),
  m_fix_point_dxy(),
  m_rope_angle(),
  m_rope_length(),
  m_rope_elasticity(10.0),
  m_elasticity_damping(0.20),
  m_balancing_damping(0.40),
  m_elasticity_off(true),
  m_cfg()
{
}

//---------------------------------------------------------------------------//
//--                         Class Parameters SET/GET                      --//
//---------------------------------------------------------------------------//

void Physics::SetPhysXY(Double x, Double y)
{
  if (m_pos_x.x0 != x || m_pos_y.x0 != y) {
    m_pos_x.x0 = x;
    m_pos_y.x0 = y;
    UpdateTimeOfLastMove();
  }
}

// Set the air resist factor
void Physics::SetSpeedXY(Point2d vector)
{
  if (EqualsZero(vector.x))
    vector.x = 0;
  if (EqualsZero(vector.y))
    vector.y = 0;
  bool was_moving = IsMoving();

  m_pos_x.x1 = vector.x;
  m_pos_y.x1 = vector.y;
  // setting to FreeFall is done in StartMoving()

  if (!was_moving && IsMoving()) {
    UpdateTimeOfLastMove();
    StartMoving();
  }
}

void Physics::AddSpeedXY(Point2d vector)
{
  if (EqualsZero(vector.x))
    vector.x = 0;
  if (EqualsZero(vector.y))
    vector.y = 0;
  bool was_moving = IsMoving();

  m_pos_x.x1 += vector.x;
  m_pos_y.x1 += vector.y;
  // setting to FreeFall is done in StartMoving()

  if (!was_moving && IsMoving()) {
    UpdateTimeOfLastMove();
    StartMoving();
  }
}

void Physics::GetSpeed(Double &norm, Double &angle) const
{
  Point2d speed;

  switch (m_motion_type) {
    case FreeFall:
      speed = GetSpeedXY();
      norm = speed.Norm();
      angle = speed.ComputeAngle();
      break;

    case Pendulum:
      // Transform angular speed to linear speed.

      norm = AbsoluteValue(m_rope_length.x0 * m_rope_angle.x1);

      if (m_rope_angle.x1 > 0)
        angle = AbsoluteValue(m_rope_angle.x0);
      else
        angle = AbsoluteValue(m_rope_angle.x0) - PI;

      if (m_rope_angle.x0 < 0)
        angle = -angle;
      break;

    case NoMotion:
      norm = 0.0;
      angle = 0.0;
      break;

    default:
      ASSERT(false);
      break;
  }
}

bool Physics::IsGoingUp() const
{
  Double angle = GetSpeedAngle();
  if (angle > -HALF_PI_PLUS && angle < -HALF_PI_MINUS)
    return true;

  return false;
}

bool Physics::IsGoingDown() const
{
  Double angle = GetSpeedAngle();
  if (angle < HALF_PI_PLUS && angle > HALF_PI_MINUS)
    return true;

  return false;
}

void Physics::SetExternForceXY (const Point2d& vector)
{
  bool was_moving = IsMoving();

  UpdateTimeOfLastMove();
  MSG_DBG_RTTI("physic.physic", "EXTERN FORCE %s.", typeid(*this).name());

  m_extern_force.SetValues(vector);

  if (!was_moving && IsMoving())
    StartMoving();
}

// Set fixation point positions
void Physics::SetPhysFixationPointXY(Double g_x, Double g_y, Double dx,
                                     Double dy)
{
  Double fix_point_x, fix_point_y;
  Double old_length;

  Point2d V;
  m_fix_point_gnd.x = g_x;
  m_fix_point_gnd.y = g_y;
  m_fix_point_dxy.x = dx;
  m_fix_point_dxy.y = dy;

  UpdateTimeOfLastMove();
  //  printf ("Fixation (%f,%f) dxy(%f,%f)\n",  g_x, g_y, dx, dy);

  fix_point_x = m_pos_x.x0 + dx;
  fix_point_y = m_pos_y.x0 + dy;

  old_length = m_rope_length.x0;
  m_rope_length.x0 = Point2d(fix_point_x,fix_point_y).Distance(Point2d(g_x,g_y));

  if (m_motion_type == Pendulum) {
    // We were already fixed. By changing the fixation point, we have
    // to recompute the angular speed depending of the new rope length.
    // And don't forget to recompute the angle, too!
    V.x = fix_point_x - g_x;
    V.y = fix_point_y - g_y;
    m_rope_angle.x0 = HALF_PI - V.ComputeAngle();

    m_rope_angle.x1 = m_rope_angle.x1 * old_length / m_rope_length.x0;
  } else {
    // We switch from a regular move to a pendulum move.
    // Compute the initial angle
    V.x = fix_point_x - g_x;
    V.y = fix_point_y - g_y;
    m_rope_angle.x0 = HALF_PI - V.ComputeAngle();

    // Convert the linear speed to angular speed.
    m_rope_angle.x1 = ( m_pos_x.x1 * cos(m_rope_angle.x0) -
                        m_pos_y.x1 * sin(m_rope_angle.x0) ) / m_rope_length.x0;

    // Reset the angular acceleration.
    m_rope_angle.x2 = 0;

    bool was_moving = IsMoving();
    m_motion_type = Pendulum;
    if (!was_moving && IsMoving())
      StartMoving();
  }
}

void Physics::UnsetPhysFixationPoint()
{
  Double speed_norm, angle;

  GetSpeed(speed_norm, angle);

  angle = -angle;

  SetSpeed(speed_norm, angle);

  UpdateTimeOfLastMove();

  m_pos_x.x2 = 0;
  m_pos_y.x2 = 0;

  m_rope_angle.Clear();
  m_rope_length.Clear();

  m_motion_type = FreeFall;
}

void Physics::ChangePhysRopeSize(Double dl)
{
  if (dl<0 && m_rope_length.x0<ONE_HALF)
    return;

  bool was_moving = IsMoving();

  m_rope_length.x0 += dl;

  // Recompute angular speed depending on the new rope length.
  m_rope_angle.x1 = m_rope_angle.x1 * (m_rope_length.x0 - dl) / m_rope_length.x0;

  if (!was_moving && IsMoving())
    StartMoving();
}


//---------------------------------------------------------------------------//
//--                            Physical Simulation                        --//
//---------------------------------------------------------------------------//

void Physics::StartMoving()
{
  UpdateTimeOfLastMove();

  if (m_motion_type == NoMotion)
    m_motion_type = FreeFall;

  MSG_DBG_RTTI("physic.physic", "Starting to move: %s.", typeid(*this).name());
}

void Physics::StopMoving()
{
  if (!IsMoving()) return;

  if (IsMoving())
    MSG_DBG_RTTI("physic.physic", "Stops moving: %s.", typeid(*this).name());
  // Always called by PhysicalObj::StopMoving
  m_pos_x.x1 = 0;
  m_pos_x.x2 = 0;
  m_pos_y.x1 = 0;
  m_pos_y.x2 = 0;
  UpdateTimeOfLastMove();
  if (m_motion_type != Pendulum)
    m_motion_type = NoMotion;

  m_extern_force.Clear();
}

bool Physics::IsMoving() const
{
  return !EqualsZero(m_pos_x.x1)  ||
         !EqualsZero(m_pos_y.x1)  ||
         !m_extern_force.IsNull() ||
         m_motion_type != NoMotion;
}

bool Physics::IsSleeping() const
{
  // return true if not moving since 1 sec.
  int delta = GameTime::GetInstance()->Read() - m_last_move;
  if (delta > 400) {
    MSG_DBG_RTTI("physic.sleep", "%s is sleeping since %d ms.",
                 typeid(*this).name(), delta);
    return true;
  }
  MSG_DBG_RTTI("physic.notsleeping", "%s is not sleeping.", typeid(*this).name());
  return false;
}

void Physics::UpdateTimeOfLastMove()
{
  m_last_move = GameTime::GetInstance()->Read();
}

// Compute the next position of the object during a pendulum motion.
void Physics::ComputePendulumNextXY(Double delta_t)
{
  MSG_DBG_RTTI("physic.pendulum", "%s: Pendulum; mass %s",
               typeid(*this).name(), Double2str(m_mass,5).c_str());

  //  Double l0 = 5.0;

  //  printf ("Physics::ComputePendulumNextXY - Angle %f\n", m_rope_angle.x0);

  // Elactic rope length equation
  // l" + D.l' + (k/m - a'^2).l = g.cos a + k/m . l0

//   ComputeOneEulerStep(m_rope_length,
//                       /* a */ 1,
//                       /* b */ m_elasticity_damping,
//                       /* c */ m_rope_elasticity / m_mass - m_rope_angle.x1 * m_rope_angle.x1,
//                       /* d */ game_mode.gravity * cos (m_rope_angle.x0) + m_rope_elasticity/m_mass * l0,
//                       delta_t);

  // Pendulum motion equation (angle equation)
  // a'' + (D + 2.l'/l).a' = -g/l.sin a + F/l.cos a
  m_rope_angle.ComputeOneEulerStep(
                      /* a */ 1,
                      /* b */ m_balancing_damping + 2 * m_rope_length.x1 / m_rope_length.x0,
                      /* c */ 0,
                      /* d */ -GameMode::GetInstance()->gravity / m_rope_length.x0 * sin(m_rope_angle.x0)
                              +m_extern_force.x / m_rope_length.x0 * cos(m_rope_angle.x0),
                               delta_t);

  Double x = m_fix_point_gnd.x - m_fix_point_dxy.x
           + m_rope_length.x0 * sin(m_rope_angle.x0);
  Double y = m_fix_point_gnd.y - m_fix_point_dxy.y
           + m_rope_length.x0 * cos(m_rope_angle.x0);

  MSG_DBG_RTTI("physic.pendulum", "%s angle=(%.2f,%.2f,%.2f) pos=(%.2f,%.2f) fixpoint=(%.2f,%.2f)",
               typeid(*this).name(),
               m_rope_angle.x0.tofloat(), m_rope_angle.x1.tofloat(), m_rope_angle.x2.tofloat(),
               x.tofloat(), y.tofloat(), m_fix_point_gnd.x.tofloat(), m_fix_point_gnd.y.tofloat());

  //  printf ("Physics::ComputePendulumNextXY - Angle(%f,%f,%f)\n",
  //            m_rope_angle.x0, m_rope_angle.x1, m_rope_angle.x2);

  SetPhysXY(x,y);

  Double speed_norm, angle;
  GetSpeed(speed_norm, angle);
  angle = -angle;
  SetSpeed(speed_norm, angle);
}

// Compute the next position of the object during a free fall.
void Physics::ComputeFallNextXY(Double delta_t)
{
  Double speed_norm, speed_angle;
  Double air_resistance_factor;

  Double weight_force;
  Double wind_force;

  // Free fall motion equation
  // m.g + wind -k.v = m.a

  // Weight force = m * g

  // printf ("av : (%5f,%5f) - (%5f,%5f) - (%5f,%5f)\n", m_pos_x.x0,
  //          m_pos_y.x0, m_pos_x.x1, m_pos_y.x1, m_pos_x.x2, m_pos_y.x2);

  weight_force = GameMode::GetInstance()->gravity * m_gravity_factor * m_mass;

  // Wind force

  wind_force = Wind::GetRef().GetStrength() * m_wind_factor;

  // Air resistanceance factor

  GetSpeed(speed_norm, speed_angle);

  if (!EqualsZero(speed_norm))
    UpdateTimeOfLastMove();

  air_resistance_factor = AIR_RESISTANCE_FACTOR * m_air_resist_factor;

  MSG_DBG_RTTI("physic.fall",
               "%s falls; mass %.5f, weight %.5f, wind %.5f, air %.5f, delta %.2f",
               typeid(*this).name(), m_mass.tofloat(), weight_force.tofloat(),
               wind_force.tofloat(), air_resistance_factor.tofloat(),
               delta_t.tofloat());

  MSG_DBG_RTTI("physic.fall",
               "%s before - x:{%.1f, %.1f, %.1f} - y:{%.1f, %.1f, %.1f} - delta:%.1f - extern_force: %.1f, %.1f",
               typeid(*this).name(), m_pos_x.x0.tofloat(), m_pos_x.x1.tofloat(), m_pos_x.x2.tofloat(),
               m_pos_y.x0.tofloat(), m_pos_y.x1.tofloat(), m_pos_y.x2.tofloat(),
               delta_t.tofloat(), m_extern_force.x.tofloat(), m_extern_force.y.tofloat());

  // Equation on X axys : m.x'' + k.x' = wind
  m_pos_x.ComputeOneEulerStep(m_mass, air_resistance_factor, 0,
                              wind_force + m_extern_force.x, delta_t);

  // Equation on Y axys : m.y'' + k.y' = m.g
  m_pos_y.ComputeOneEulerStep(m_mass, air_resistance_factor, 0,
                              weight_force + m_extern_force.y, delta_t);

  MSG_DBG_RTTI("physic.fall",
               "%s after - x:{%.1f, %.1f, %.1f - y:{%.1f, %.1f, %.1f} - delta:%.1f - extern_force: %.1f, %.1f",
               typeid(*this).name(), m_pos_x.x0.tofloat(), m_pos_x.x1.tofloat(), m_pos_x.x2.tofloat(),
               m_pos_y.x0.tofloat(), m_pos_y.x1.tofloat(), m_pos_y.x2.tofloat(),
               delta_t.tofloat(), m_extern_force.x.tofloat(), m_extern_force.y.tofloat());
    // printf ("F : Pd(%5f) EF(%5f)\n", weight_force, m_extern_force.y);

   // printf ("ap : (%5f,%5f) - (%5f,%5f) - (%5f,%5f)\n", m_pos_x.x0,
  //          m_pos_y.x0, m_pos_x.x1, m_pos_y.x1, m_pos_x.x2, m_pos_y.x2);
}

// Compute the position of the object at current time.
Point2d Physics::ComputeNextXY(Double delta_t){

  MSG_DBG_RTTI("physic.compute", "%s: delta: %f", typeid(*this).name(), delta_t.tofloat());

  if (FreeFall == m_motion_type) {
    ComputeFallNextXY(delta_t);
  }

  if (Pendulum == m_motion_type) {
    ComputePendulumNextXY(delta_t);
  }

  return Point2d(m_pos_x.x0, m_pos_y.x0);
}

void Physics::ResetLastRunTime()
{
  m_last_physical_engine_run = GameTime::GetInstance()->Read();
}

void Physics::RunPhysicalEngine()
{
  uint now = GameTime::GetInstance()->Read();
  if (m_last_physical_engine_run < m_last_move) {
    ASSERT(now >= m_last_move);
    m_last_physical_engine_run = m_last_move;
  } else {
    ASSERT(now >= m_last_physical_engine_run);
  }
  uint delta_t_ms = now - m_last_physical_engine_run;
  Point2d oldPos;
  Point2d newPos;

  m_last_physical_engine_run += (delta_t_ms/PHYS_DELTA_T_MS) * PHYS_DELTA_T_MS;

  // Compute object move for each physical engine time step.
  while (delta_t_ms >= PHYS_DELTA_T_MS) {
    oldPos = GetPos();
    newPos = ComputeNextXY(PHYS_DELTA_T); // causes 11�s error per iteration

    if (newPos != oldPos) {
      // The object has moved. Notify the son class.
      MSG_DBG_RTTI("physic.move", "%s moves (%.1f, %.1f) -> (%.1f, %.1f) - x:{%.1f, %.1f, %.1f - y:{%.1f, %.1f, %.1f} - step:%ums",
                   typeid(*this).name(), oldPos.x.tofloat(), oldPos.y.tofloat(), newPos.x.tofloat(), newPos.y.tofloat(),
                   m_pos_x.x0.tofloat(), m_pos_x.x1.tofloat(), m_pos_x.x2.tofloat(),
                   m_pos_y.x0.tofloat(), m_pos_y.x1.tofloat(), m_pos_y.x2.tofloat(), PHYS_DELTA_T_MS);
      NotifyMove(oldPos, newPos);
    }
    delta_t_ms -= PHYS_DELTA_T_MS;
  }
  return;
}

/* contact_angle is the angle of the surface we are rebounding on */
void Physics::Rebound(Point2d /*contactPos*/, Double contact_angle)
{
  Double norme, angle;

  // Get norm and angle of the object speed vector.
  GetSpeed(norme, angle);

  switch (m_motion_type) {
  case FreeFall :
    if (m_rebounding) {
      // Compute rebound angle.
      /* if no tangent rebound in the opposit direction */
      if (isNaN(contact_angle))
        angle = angle + PI;
      else
        angle =  PI - angle - 2 *  contact_angle;

      // Apply rebound factor to the object speed.
      norme = norme * m_rebound_factor;

      // Apply the new speed to the object.
      SetSpeed(norme, angle);

      // Check if we should stop rebounding.
      if (norme < STOP_REBOUND_LIMIT) {
        StopMoving();
        return;
      }
      SignalRebound();
    }
    else
      StopMoving();
    break;

  case Pendulum:
    {
      Point2d V;

      // Recompute new angle.
      V.x = m_pos_x.x0 + m_fix_point_dxy.x - m_fix_point_gnd.x;
      V.y = m_pos_y.x0 + m_fix_point_dxy.y - m_fix_point_gnd.y;

      m_rope_angle.x0 = HALF_PI - V.ComputeAngle();

      m_rope_angle.x1 = -PENDULUM_REBOUND_FACTOR * m_rope_angle.x1;

      m_rope_angle.x2 = 0;
      m_extern_force.Clear();
    }
    break;

  default:
    break;
  }

}
