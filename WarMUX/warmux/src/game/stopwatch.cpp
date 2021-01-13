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
 *  The stopwatch can be used to messure how much time elapses.
 *****************************************************************************/

#include "game/stopwatch.h"
#include <SDL.h>
#include <WARMUX_error.h>

Stopwatch::Stopwatch()
{
  Reset();
}

void Stopwatch::Reset(Double speed_value)
{
  speed       = speed_value;
  paused      = false;
  start_time  = SDL_GetTicks();
  offset_time = 0;
}

void Stopwatch::Stop()
{
  ASSERT(!paused);
  // Save last valid time
  offset_time = GetValue();
  paused      = true;
}

void Stopwatch::Resume()
{
  ASSERT(paused);
  paused     = false;
  // Start new time segment
  start_time = SDL_GetTicks();
}

void Stopwatch::SetPause(bool value)
{
  if (paused == value)
    return;
  if (paused) {
    Resume();
  } else {
    Stop();
  }
}

uint Stopwatch::GetValue() const
{
  if (paused)
    return offset_time;
  else
    return static_cast<long>(speed * (SDL_GetTicks() - start_time)) + offset_time;
}

void Stopwatch::SetSpeed(const Double& sp)
{
  // Save current time as a new segment is about to start
  offset_time = GetValue();
  // Start new time segment
  start_time  = SDL_GetTicks();
  speed       = sp;
}

void Stopwatch::Resynch(uint value)
{
  offset_time = value;
  start_time  = SDL_GetTicks();
}
