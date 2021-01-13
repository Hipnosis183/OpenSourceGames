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
 * Virtual class to handle weapon in warmux.
 * Weapon projectile are handled in WeaponLauncher (see launcher.cpp and launcher.h).
 *****************************************************************************/

#ifndef WEAPON_H
#define WEAPON_H
#include <string>
#include <WARMUX_base.h>
#include "sound/sound_sample.h"
#include <WARMUX_debug.h>
#include <WARMUX_point.h>

class Character;
class Sprite;
class EmptyWeaponConfig;
typedef struct _xmlNode xmlNode;

// Infinite ammos constant
extern const int INFINITE_AMMO;

extern const uint BUTTON_ICO_WIDTH;
extern const uint BUTTON_ICO_HEIGHT;

extern const uint WEAPON_ICO_WIDTH;
extern const uint WEAPON_ICO_HEIGHT;

//-----------------------------------------------------------------------------

class Weapon
{
public:
  typedef enum
  {
    FIRST,
    WEAPON_BAZOOKA=FIRST, WEAPON_AUTOMATIC_BAZOOKA, WEAPON_CLUZOOKA, WEAPON_RIOT_BOMB,
    WEAPON_GRENADE,       WEAPON_DISCO_GRENADE,     WEAPON_CLUSTER_BOMB, WEAPON_FOOTBOMB,
    WEAPON_GUN,           WEAPON_SHOTGUN,           WEAPON_SUBMACHINE_GUN,
    WEAPON_BASEBALL,      WEAPON_FLAMETHROWER,      WEAPON_SLAP,

    WEAPON_DYNAMITE,      WEAPON_MINE,

    WEAPON_SUPERTUX,      WEAPON_AIR_ATTACK,  WEAPON_ANVIL, WEAPON_GNU,
    WEAPON_POLECAT,       WEAPON_BOUNCE_BALL,

    WEAPON_TELEPORTATION, WEAPON_GRAPPLE,  WEAPON_LOWGRAV,   WEAPON_SUICIDE,
    WEAPON_SKIP_TURN,     WEAPON_JETPACK,     WEAPON_PARACHUTE, WEAPON_AIR_HAMMER,
    WEAPON_CONSTRUCT,     WEAPON_SNIPE_RIFLE, WEAPON_RAIL_GUN, WEAPON_BLOWTORCH, WEAPON_SYRINGE,
    LAST = WEAPON_SYRINGE
  } Weapon_type;
  typedef enum {
    INVALID = 0,
    HEAVY,
    RIFLE,
    THROW,
    SPECIAL,
    DUEL,
    MOVE,
    TOOL
  } category_t;

private:
  // Angle in radian between -PI to PI
  Double min_angle, max_angle;
  // display crosshair ?
  bool m_display_crosshair;

protected:
  Weapon::Weapon_type m_type;
  Weapon::category_t  m_category;

  std::string m_id;
  std::string m_name;
  std::string m_help;
  Sprite *m_image;
  Sprite *m_weapon_fire;
  uint m_fire_remanence_time;

  typedef enum {
    weapon_origin_HAND,
    weapon_origin_OVER
  } weapon_origin_t;
  weapon_origin_t origin;

  Point2i hole_delta; // relative position of the hole of the weapon
  Point2i position;   // Position of the weapon

  // GameTime when the weapon is selected for the animation
  uint m_time_anim_begin;

  // Actual strength of the weapon
  Double m_strength;

  // time of beginning to load (for choosing the strength)
  uint m_first_time_loading;

  // time of the last fire
  uint m_last_fire_time;

  // time between 2 shot
  uint m_time_between_each_shot;

  // change weapon after ? (for the grapple = true)
  bool m_can_change_weapon;

  // Extra parameters
  EmptyWeaponConfig *extra_params;

  // Visibility
  const bool drawable;

  // how many times can we use this weapon (since the beginning of the game) ?
  int m_available_after_turn; // -1 means NEVER
  int m_initial_nb_ammo;
  int m_initial_nb_unit_per_ammo;
  int ammo_per_drop;
  Double drop_probability;
  bool use_unit_on_first_shoot;
  bool can_be_used_on_closed_map;
  SoundSample loading_sound;

public:
  // weapon's icon
  Sprite * icon;

  // if max_strength != 0, display the strength bar
  Double max_strength;

  bool use_flipping;
  const category_t& Category() const { return m_category; };

protected:
  virtual void p_Select() { m_last_fire_time = 0; };
  virtual void p_Deselect();
  virtual void Refresh();
  virtual bool p_Shoot() = 0;

  /* This method offer sub classes a way to hide the weapon. */
  virtual bool ShouldBeDrawn() { return true; };
  /* This method offer sub classes a way to hide the ammo units. */
  virtual bool ShouldAmmoUnitsBeDrawn() const { return true; };
  virtual void DrawWeaponFire();
  void DrawAmmoUnits() const;

  // Begin the shooting animation of the character
  void PrepareShoot();

  void RepeatShoot();

  void StartMovingLeftForAllPlayers();
  void StopMovingLeftForAllPlayers();

  void StartMovingRightForAllPlayers();
  void StopMovingRightForAllPlayers();

  void StartMovingUpForAllPlayers();
  void StopMovingUpForAllPlayers();

  void StartMovingDownForAllPlayers();
  void StopMovingDownForAllPlayers();

public:
  Weapon(Weapon_type type,
         const std::string &id,
         EmptyWeaponConfig * params,
         bool drawable = true);
  virtual ~Weapon();

  // Select or deselect the weapon
  void Select();
  void Deselect();

  // Gestion de l'arme
  void Manage();
  bool CanChangeWeapon() const ;

  // Draw the weapon
  virtual void Draw();

  Sprite & GetIcon() const { return *icon; };
  // Manage the numbers of ammunitions
  bool EnoughAmmo() const;
  void UseAmmo() const;
  bool EnoughAmmoUnit() const;
  void UseAmmoUnit() const;

  int AvailableAfterTurn() const { return m_available_after_turn; };
  int ReadInitialNbAmmo() const { return m_initial_nb_ammo; };
  void WriteInitialNbAmmo(int nb) { m_initial_nb_ammo = nb; };
  int ReadInitialNbUnit() const { return m_initial_nb_unit_per_ammo; };
  int GetAmmoPerDrop() const { return ammo_per_drop; }
  Double GetDropProbability() const { return drop_probability; }

  bool CanBeUsedOnClosedMap() const { return can_be_used_on_closed_map; };
  bool UseCrossHair() const { return min_angle != max_angle; };

  // Calculate weapon position
  virtual void PosXY (int &x, int &y) const;

  // Shot with the weapon
  // Return true if we have been able to trigger the weapon
  bool Shoot();

  // the weapon is ready to use ? (is there bullets left ?)
  virtual bool IsReady() const;

  virtual bool IsOnCooldownFromShot() const;

  // Begin to load, to choose the strength
  virtual void InitLoading() ;

  // Are we loading to choose the strength
  virtual bool IsLoading() const { return m_first_time_loading ? true : false; };

  // Stop loading
  virtual void StopLoading() ;

  // update strength (so the strength bar can be updated)
  virtual void UpdateStrength();

  Double GetStrength() const { return m_strength; };

  Double GetMaxStrength() const { return max_strength; };

  const Point2i GetGunHolePosition() const;

  // Choose a target.
  virtual void ChooseTarget (Point2i /*mouse_pos*/) { };

  // Notify a move. It is usefull only for weapon which have strong
  // interactions with the physical engine such as grapple
  virtual void NotifyMove(bool /*collision*/){};


  // While the method returns true the character can not start moving left nor right.
  virtual bool IsPreventingLRMovement() { return false; };
  // While the method returns true the character will not jump when the user press the jump key.
  virtual bool IsPreventingJumps() { return false; };
  // While the method returns true the character will not change the weapon angle when the user tries to do so.
  virtual bool IsPreventingWeaponAngleChanges() { return false; };

  // Handle a keyboard event.

  // Key Shoot management
  void HandleKeyPressed_Shoot();
  void HandleKeyReleased_Shoot();

  // To override standard moves of character
  virtual void HandleKeyPressed_MoveRight(bool /*slowly*/) {};
  virtual void HandleKeyReleased_MoveRight(bool /*slowly*/) {};

  virtual void HandleKeyPressed_MoveLeft(bool /*slowly*/) {};
  virtual void HandleKeyReleased_MoveLeft(bool /*slowly*/) {};

  virtual void HandleKeyPressed_Up(bool /*slowly*/) {};
  virtual void HandleKeyReleased_Up(bool /*slowly*/) {};

  virtual void HandleKeyPressed_Down(bool /*slowly*/) {};
  virtual void HandleKeyReleased_Down(bool /*slowly*/) {};

  virtual void HandleKeyPressed_Jump() {};
  virtual void HandleKeyReleased_Jump() {};

  virtual void HandleKeyPressed_HighJump() {};
  virtual void HandleKeyReleased_HighJump() {};

  virtual void HandleKeyPressed_BackJump() {};
  virtual void HandleKeyReleased_BackJump() {};

  // Other keys
  virtual void HandleKeyReleased_Num1(){};
  virtual void HandleKeyReleased_Num2(){};
  virtual void HandleKeyReleased_Num3(){};
  virtual void HandleKeyReleased_Num4(){};
  virtual void HandleKeyReleased_Num5(){};
  virtual void HandleKeyReleased_Num6(){};
  virtual void HandleKeyReleased_Num7(){};
  virtual void HandleKeyReleased_Num8(){};
  virtual void HandleKeyReleased_Num9(){};
  virtual void HandleKeyReleased_Less(){};
  virtual void HandleKeyReleased_More(){};

  // Handle a mouse event
  virtual void HandleMouseLeftClicReleased(bool){};
  virtual void HandleMouseWheelUp(bool /*shift*/){};
  virtual void HandleMouseWheelDown(bool /*shift*/){};

  // Get informed that the turn is over.
  virtual void SignalTurnEnd() { StopLoading(); };

  // Load parameters from the xml config file
  // Return true if xml has been succesfully load
  bool LoadXml(const xmlNode*  weapon);

  // return the strength of the weapon
  Double ReadStrength() const { return m_strength; };

  // Data access
  const std::string& GetName() const { return m_name; }
  const std::string& GetID() const { return m_id; }
  const std::string& GetHelp() const { return m_help; }
  Weapon_type GetType() const { return m_type; };

  // For localization purposes, called when changing language
  virtual void UpdateTranslationStrings() = 0;

  // For localization purposes, each weapon needs to have its own
  // "%s team has won %d <weapon>" function
  virtual std::string GetWeaponWinString(const char *TeamName, uint items_count) const = 0;

  // Allows or not the character selection with mouse click (tab is allowed)
  // This is used in weapons like the automated bazooka, where it's required
  // a target. Default is true.
  bool mouse_character_selection;

  inline void SetMinAngle(Double min) {min_angle = min;}
  inline const Double &GetMinAngle() const {return min_angle;}
  inline void SetMaxAngle(Double max) {max_angle = max;}
  inline const Double &GetMaxAngle() const {return max_angle;}
  bool IsAngleValid(Double angle) const;

  virtual void StartMovingLeft() {};
  virtual void StopMovingLeft() {};

  virtual void StartMovingRight() {};
  virtual void StopMovingRight() {};

  virtual void StartMovingUp() {};
  virtual void StopMovingUp() {};

  virtual void StartMovingDown() {};
  virtual void StopMovingDown() {};

  virtual void StartShooting();
  virtual void StopShooting();

  // Functions to avoid dynamic_cast
  virtual void SetProjectileTimeOut(int) { };
  virtual void SetAngle(Double) { }

  static void Message(const std::string& msg);
};

//-----------------------------------------------------------------------------
#endif
