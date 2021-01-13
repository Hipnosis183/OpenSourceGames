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
 * Game mode : duration of a turn, weapons configurations, max number of character
 * per team, etc.
 *****************************************************************************/

#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <string>
#include <vector>
#include <WARMUX_singleton.h>
#include <WARMUX_base.h>
#include "weapon/weapon_cfg.h"
#include "tool/xml_document.h"

typedef struct _xmlNode xmlNode;

class GameMode : public Singleton<GameMode>
{
public:
  std::string rules;

  uint nb_characters;
  uint max_teams;
  uint duration_turn;
  uint duration_move_player;
  uint duration_exchange_player;
  uint duration_before_death_mode;
  uint damage_per_turn_during_death_mode;
  Double gravity;
  Double safe_fall ;
  Double damage_per_fall_unit ;
  ExplosiveWeaponConfig death_explosion_cfg;
  ExplosiveWeaponConfig barrel_explosion_cfg;
  ExplosiveWeaponConfig bonus_box_explosion_cfg;

  struct s_character
  {
    uint init_energy;
    uint max_energy;
    uint mass;
    Double air_resist_factor;
    Double jump_strength;
    Double jump_angle;
    Double super_jump_strength;
    Double super_jump_angle;
    Double back_jump_strength;
    Double back_jump_angle;
    uint walking_pause;
  } character;

  bool auto_change_character;

  typedef enum {
    ALWAYS = 0,
    BEFORE_FIRST_ACTION,
    NEVER
  } manual_change_character_t;

  manual_change_character_t allow_character_selection;
  XmlReader doc;

private:
  std::string m_current;

  XmlReader* doc_objects;
  const xmlNode* weapons_xml;

  void LoadDefaultValues();

  bool LoadXml (const xmlNode* xml);
  bool ExportFileToString(const std::string& filename, std::string& contents) const;

  std::string GetFilename() const;

  std::string GetDefaultObjectsFilename() const;
  std::string GetObjectsFilename() const;

public:
  const std::string& GetName() const { return m_current; }

  const xmlNode* GetWeaponsXml() { return weapons_xml; }
  int GetMaxTeamsPerNetworkPlayer() { return max_teams -1; }

  bool Load(void);

  // mode: xml text of data/game_mode/<mode>.xml
  // mode_objects: xml text of data/game_mode/<mode>_objects.xml
  bool LoadFromString(const std::string& game_mode_name,
                      const std::string& mode,
                      const std::string& mode_objects);

  bool ExportToString(std::string& mode,
                      std::string& mode_objects) const;

  const XmlReader* GetXmlObjects() const { return doc_objects; }

  bool AllowCharacterSelection() const;

  static std::vector<std::pair<std::string, std::string> > ListGameModes();

protected:
  friend class Singleton<GameMode>;
  GameMode();
  ~GameMode();
};

#endif /* GAME_MODE_H */
