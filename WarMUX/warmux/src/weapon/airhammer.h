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
 * AirHammer - Use it to dig
 *****************************************************************************/

#ifndef AIRHAMMER_H
#define AIRHAMMER_H
//-----------------------------------------------------------------------------
#include <WARMUX_base.h>
#include "graphic/surface.h"
#include "weapon.h"
//-----------------------------------------------------------------------------

class AirhammerConfig;

//-----------------------------------------------------------------------------

class Airhammer : public Weapon
{
  private:
    Surface impact;
    void RepeatShoot();
    SoundSample drill_sound;
    SoundSample select_sound;
    bool active;
    bool deactivation_requested;

    void PlaySoundSelect();

  protected:
    void p_Select();
    void p_Deselect();
    bool p_Shoot();
    void Refresh();

  public:
    Airhammer();
    AirhammerConfig &cfg();
    void StartShooting();
    void StopShooting();
    virtual bool ShouldAmmoUnitsBeDrawn() const;
    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

//-----------------------------------------------------------------------------
#endif /* AIRHAMMER_H */
