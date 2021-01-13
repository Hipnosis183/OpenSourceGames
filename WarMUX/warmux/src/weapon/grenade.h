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
 * Weapon grenade: throw a grenade with given engle and strength. Explode
 *                 after a small while.
 *****************************************************************************/

#ifndef GRENADE_H
#define GRENADE_H

#include "weapon/weapon_launcher.h"

// La Grenade
class Grenade : public WeaponProjectile
{
public:
  Grenade(ExplosiveWeaponConfig& cfg,
          WeaponLauncher * p_launcher);
  void Refresh();
protected:
  void SignalOutOfMap();
};

class GrenadeLauncher : public WeaponLauncher
{
 public:
  GrenadeLauncher();

  void UpdateTranslationStrings();
  std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
 protected:
  WeaponProjectile * GetProjectileInstance();
};

#endif
