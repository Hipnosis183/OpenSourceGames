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
 * Weapon Syringe
 *****************************************************************************/

#include "weapon/explosion.h"
#include "weapon/syringe.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include <WARMUX_point.h>
#include "tool/xml_document.h"

class SyringeConfig : public WeaponConfig
{
  public:
    Double range;
    uint damage;
    uint turns;
    SyringeConfig();
    void LoadXml(const xmlNode* elem);
};

SyringeConfig& Syringe::cfg() {
  return static_cast<SyringeConfig&>(*extra_params);
}

SyringeConfig::SyringeConfig(){
  range =  45;
  turns = 10;
  damage = 10;
}

void SyringeConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadDouble(elem, "range", range);
  XmlReader::ReadUint(elem, "turns", turns);
  XmlReader::ReadUint(elem, "damage", damage);
}

Syringe::Syringe() : Weapon(WEAPON_SYRINGE, "syringe", new SyringeConfig())
{
  UpdateTranslationStrings();

  m_category = DUEL;
}

void Syringe::UpdateTranslationStrings()
{
  m_name = _("Syringe");
  m_help = _("Go to player\nPress space to inject\nWill cure your teammate or slowly kill your opponent");
}

bool Syringe::p_Shoot ()
{
  Double angle = ActiveCharacter().GetFiringAngle();
  Double radius = 0.0;
  bool end = false;

  JukeBox::GetInstance()->Play ("default","weapon/syringe_shoot");

  Character* player = &ActiveCharacter();
  do {
    // Did we have finished the computation
    radius += ONE;
    if (cfg().range < radius) {
      radius = cfg().range;
      end = true;
    }

    // Compute point coordinates
    Point2i relative_pos(static_cast<int>(radius * cos(angle)),
                         static_cast<int>(radius * sin(angle)) );
    Point2i hand_position;
    ActiveCharacter().GetHandPosition(hand_position);
    Point2i pos_to_check = hand_position + relative_pos;

    FOR_ALL_LIVING_CHARACTERS(team, character) {
      if (&(*character) != player) {
        // Did we touch somebody ?
        if (character->Contain(pos_to_check)) {
          // Apply damage (*ver).SetEnergyDelta (-cfg().damage);
          if (!player->GetTeam().IsSameAs(character->GetTeam()) || !player->IsDiseased())
            character->SetDiseaseDamage(player, cfg().damage, cfg().turns);
          else {
            // The syringe can cure if applied to a teammate!
            character->Cure();
          }
          end = true;
        }
      }
    }
  } while (!end);

  return true;
}

std::string Syringe::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u syringe!",
            "%s team has won %u syringes!",
            items_count), TeamName, items_count);
}
