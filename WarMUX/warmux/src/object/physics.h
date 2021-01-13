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

#ifndef PHYSICS_H
#define PHYSICS_H

#include <WARMUX_base.h>
#include <WARMUX_euler_vector.h>
#include <WARMUX_point.h>
#include "object_cfg.h"

static const Double MIN_FALL_SPEED = 0.1;

enum MotionType_t
{
  NoMotion,
  FreeFall,
  Pendulum,
};

typedef enum {
  NO_COLLISION = 0,
  COLLISION_ON_GROUND,
  COLLISION_ON_OBJECT
} collision_t;

class Game;
class Action;

class Physics : private ObjectConfig
{
private:
  MotionType_t m_motion_type;
  EulerVector m_pos_x;          // x0 = pos, x1 = speed, x2 = acc on the X axys
  EulerVector m_pos_y;          // x0 = pos, x1 = speed, x2 = acc on the Y axys
  Point2d m_extern_force;  // External strength applyed to the object

protected:
  uint m_last_move;             // Time since last move
  uint m_last_physical_engine_run;             // Time since last physical engine execution
  Double m_phys_width, m_phys_height;

  Point2d m_fix_point_gnd;   // Rope fixation point to the ground.
  Point2d m_fix_point_dxy;   // Rope delta to fixation point to the object
  EulerVector m_rope_angle;       // Rope angle.
  EulerVector m_rope_length;      // Rope length.
  Double m_rope_elasticity;       // The smallest, the more elastic.
  Double m_elasticity_damping;    // 0 means perpetual motion.
  Double m_balancing_damping;     // 0 means perpetual balancing.

  // Define if the rope is elastic or not.
  bool m_elasticity_off;

  // Other physics constants stored there :
  ObjectConfig m_cfg;
public:
  Physics();
  virtual ~Physics () {};

  // Set/Get position
  void SetPhysXY(Double x, Double y);
  void SetPhysXY(const Point2d &position) { SetPhysXY(position.x, position.y); };

  Double GetPhysX() const { return m_pos_x.x0; };
  Double GetPhysY() const { return m_pos_y.x0; };
  Point2d GetPos() const { return Point2d(m_pos_x.x0, m_pos_y.x0); };

  // Set size
  void SetPhysSize(Double width, Double height) { m_phys_width = width; m_phys_height = height; };

  void SetMass(Double mass) { m_mass = mass; };
  Double GetMass() const { return m_mass; }

  void SetWindFactor(Double wind_factor) { m_wind_factor = wind_factor; };
  Double GetWindFactor() const { return m_wind_factor; }

  void SetAirResistFactor(Double factor) { m_air_resist_factor = factor; };
  Double GetAirResistFactor() const{ return m_air_resist_factor; }

  void SetGravityFactor(Double factor) { m_gravity_factor = factor; };
  Double GetGravityFactor() const { return m_gravity_factor; }

  void SetRebounding(bool rebounding) { m_rebounding = rebounding; }
  bool GetRebounding() const { return m_rebounding; }

  void SetAlignParticleState(bool state) { m_align_particle_state = state; }
  bool GetAlignParticleState(void) const { return m_align_particle_state; }

  // Reset the physics constants (mass, air_resistance...) to the default values in the cfg
  void ResetConstants() { *((ObjectConfig*)this) = m_cfg; };

  // Set initial speed.
  void SetSpeedXY(Point2d vector);
  void SetSpeed(Double norm, Double angle) { SetSpeedXY(Point2d::FromPolarCoordinates(norm, angle)); };

  // Add a initial speed to the current speed.
  void AddSpeedXY(Point2d vector);
  void AddSpeed(Double norm, Double angle) { AddSpeedXY(Point2d::FromPolarCoordinates(norm, angle)); };

  // Get current object speed
  void GetSpeed(Double &norm, Double &angle) const;
  Point2d GetSpeedXY() const { return (!IsMoving()) ? Point2d(0.0, 0.0) : Point2d(m_pos_x.x1, m_pos_y.x1); };
  Point2d GetSpeed() const { return GetSpeedXY(); };
  Double GetAngularSpeed() const { return m_rope_angle.x1; };
  Double GetSpeedAngle() const { return GetSpeedXY().ComputeAngle(); };
  bool IsGoingUp() const;
  bool IsGoingDown() const;

  // Add new strength
  void SetExternForceXY(const Point2d& vector);
  void SetExternForce(Double norm, Double angle) { SetExternForceXY(Point2d::FromPolarCoordinates(norm, angle)); };
  Point2d GetExternForce() const { return m_extern_force; };

  // Add / Remove a fixation point.
  void SetPhysFixationPointXY(Double g_x, Double g_y,
                              Double dx, Double dy);
  void UnsetPhysFixationPoint();
  void ChangePhysRopeSize(Double delta);

  Double GetRopeAngle() const { return m_rope_angle.x0; };
  void SetRopeAngle(Double angle) { m_rope_angle.x0 = angle; };

  Double GetRopeLength() const { return m_rope_length.x0; };
  void SetRopeLength(Double length) { m_rope_length.x0 = length; };

  MotionType_t GetMotionType() const { return m_motion_type; };

  // Physical engine : update position (and state) with current time
  void RunPhysicalEngine();
  void ResetLastRunTime();

  // Notify the son class that the object has moved.
  virtual collision_t NotifyMove(Point2d oldPos, Point2d newPos) = 0;

  // Start moving
  virtual void StartMoving();

  // Stop moving
  void StopMoving();

  // The object is moving ?
  bool IsMoving() const;
  // Is this object not moving ?
  virtual bool IsSleeping() const;

  // The object is falling ?
  bool IsFalling() const { return (m_motion_type==FreeFall) && (m_pos_y.x1 > MIN_FALL_SPEED); };

protected:
  // Compute current (x,y) position
  Point2d ComputeNextXY(Double delta_t);

  virtual void SignalDeath() { };
  virtual void SignalGhostState(bool) { };
  virtual void SignalDrowning() { };
  virtual void SignalGoingOutOfWater() { };
  virtual void SignalRebound() { };

  // Make the object rebound
  void Rebound(Point2d contactPos, Double contact_angle);
private:

  void ComputeFallNextXY(Double delta_t);

  void ComputePendulumNextXY(Double delta_t);

  void UpdateTimeOfLastMove();
};

#endif
