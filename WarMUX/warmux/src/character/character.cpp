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
 * Character of a team.
 *****************************************************************************/

#include <sstream>
#include <iostream>
#include <WARMUX_random.h>
#include "character/body.h"
#include "character/character.h"
#include "character/damage_stats.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/text.h"
#include "gui/energy_bar.h"
#include "include/action_handler.h"
#include "interface/interface.h"
#include "interface/cursor.h"
#include "map/camera.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "particles/particle.h"
#include "particles/fading_text.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/custom_team.h"
#include "team/macro.h"
#include "tool/math_tools.h"
#include "tool/string_tools.h"
#include "weapon/explosion.h"

#ifdef DEBUG
#include "include/app.h"
#include "graphic/video.h"
#endif

const int HEIGHT_FONT_NAME = 13;

// Space between the name, the skin and the energy bar
const int SPACE = 3; // pixels
const uint do_nothing_timeout = 5000;
const Double MIN_SPEED_TO_FLY = 5.0;

// Pause for the animation
#ifdef DEBUG
//#define ANIME_VITE
#endif
#ifdef ANIME_VITE
  const uint ANIM_PAUSE_MIN = 100;
  const uint ANIM_PAUSE_MAX = 150;
#else
  const uint ANIM_PAUSE_MIN = 5*1000;
  const uint ANIM_PAUSE_MAX = 60*1000;
#endif

#ifdef DEBUG
//#define DEBUG_STATS
#endif

// Energy bar
const int WIDTH_ENERGY = 40;
const int HEIGHT_ENERGY = 6;

// Delta angle used to move the crosshair
const Double DELTA_CROSSHAIR = 0.035; /* ~1 degree */

// Pause between changing direction
const uint PAUSE_CHG_DIRECTION = 80; // ms

// Max climbing height walking
const int MAX_CLIMBING_HEIGHT=30;

// Max height for which we do not need to call the Physical Engine with gravity features
const int MAX_FALLING_HEIGHT=20;

/* FIXME This methode is really strange, all this should probably be done in
 * constructor of Body...*/
void Character::SetBody(Body * char_body)
{
  body = char_body;
  body->SetOwner(this);
  SetClothe("normal");
  SetMovement("breathe");

  MSG_DEBUG("random.get", "Character::SetBody(...) direction");
  SetDirection(RandomSync().GetBool() ? DIRECTION_LEFT : DIRECTION_RIGHT);
  MSG_DEBUG("random.get", "Character::SetBody(...) body frame");
  body->SetFrame(RandomSync().GetInt(0, body->GetFrameCount() - 1));
  SetSize(body->GetSize());
}

static uint GetRandomAnimationTimeValue()
{
  MSG_DEBUG("random.get", "Character::SetBody(...) body frame");
  return GameTime::GetInstance()->Read() + RandomSync().GetUint(ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
}

Character::Character(Team& my_team, const std::string &name, Body *char_body) :
  PhysicalObj("character"),
  MovableByUser(),
  character_name(name),
  m_team(my_team),
  step_sound_played(true),
  prepare_shoot(false),
  back_jumping(false),
  death_explosion(true),
  firing_angle(0),
  disease_dealer(NULL),
  disease_damage_per_turn(0),
  disease_duration(0),
  damage_stats(new DamageStatistics(*this)),
  energy_bar(NULL),
  survivals(0),
  name_text(NULL),
  rl_motion_pause(0),
  do_nothing_time(0),
  walking_time(0),
  animation_time(GetRandomAnimationTimeValue()),
  lost_energy(0),
  hidden(false),
  channel_step(-1),
  particle_engine(new ParticleEngine(500)),
  is_playing(false),
  last_direction_change(0),
  previous_strength(0),
  body(NULL)
{
  m_is_character = true;
  SetCollisionModel(true, true, true);
  /* body stuff */
  ASSERT(char_body);
  SetBody(char_body);

  ResetConstants();
  // Allow player to go outside of map by upper bound (bug #10420)
  m_allow_negative_y = true;
  // Name Text object
  if (Config::GetInstance()->GetDisplayNameCharacter())
    name_text = new Text(character_name, m_team.GetColor());
  else
    name_text = NULL;

  // Energy
  m_energy = GameMode::GetInstance()->character.init_energy;

  energy_bar = new EnergyBar(0, 0,
                             WIDTH_ENERGY, HEIGHT_ENERGY,
                             GameMode::GetInstance()->character.init_energy,
                             0,
                             GameMode::GetInstance()->character.init_energy);

  energy_bar->SetBorderColor(black_color);
  energy_bar->SetBackgroundColor(gray_color);

  SetEnergy(GameMode::GetInstance()->character.init_energy, NULL);

  MSG_DEBUG("character", "Load character %s at %p",
            character_name.c_str(), this);
}

Character::Character(const Character& acharacter) :
  PhysicalObj(acharacter),
  MovableByUser(),
  character_name(acharacter.character_name),
  m_team(acharacter.m_team),
  step_sound_played(acharacter.step_sound_played),
  prepare_shoot(acharacter.prepare_shoot),
  back_jumping(acharacter.back_jumping),
  death_explosion(acharacter.death_explosion),
  firing_angle(acharacter.firing_angle),
  disease_dealer(acharacter.disease_dealer),
  disease_damage_per_turn(acharacter.disease_damage_per_turn),
  disease_duration(acharacter.disease_duration),
  damage_stats(new DamageStatistics(*acharacter.damage_stats, *this)),
  energy_bar(NULL),
  survivals(acharacter.survivals),
  name_text(NULL),
  rl_motion_pause(acharacter.rl_motion_pause),
  do_nothing_time(acharacter.do_nothing_time),
  walking_time(acharacter.walking_time),
  animation_time(acharacter.animation_time),
  lost_energy(acharacter.lost_energy),
  hidden(acharacter.hidden),
  channel_step(acharacter.channel_step),
  particle_engine(new ParticleEngine(250)),
  is_playing(acharacter.is_playing),
  last_direction_change(0),
  previous_strength(acharacter.previous_strength),
  body(NULL)
{
  energy_bar = new EnergyBar(acharacter.energy_bar->GetX(),
                             acharacter.energy_bar->GetY(),
                             acharacter.energy_bar->GetWidth(),
                             acharacter.energy_bar->GetHeight(),
                             acharacter.energy_bar->GetCurrentValue(),
                             acharacter.energy_bar->GetMinValue(),
                             acharacter.energy_bar->GetMaxValue());
  energy_bar->SetBorderColor(black_color);
  energy_bar->SetBackgroundColor(gray_color);
  SetEnergy(GameMode::GetInstance()->character.init_energy, NULL);

  if (acharacter.body) {
    Body * newBody = new Body(*acharacter.body);
    SetBody(newBody);
  }

  if (acharacter.name_text) {
    name_text = new Text(*acharacter.name_text);
  }
  MSG_DEBUG("character", "Copying character %s from %p to %p",
            character_name.c_str(), &acharacter, this);
}

Character::~Character()
{
  MSG_DEBUG("character", "Unload character %s at %p",
            character_name.c_str(), this);
  if (body) {
    delete body;
  }
  if (name_text) {
    delete name_text;
  }
  if (particle_engine) {
    delete particle_engine;
  }
  if (energy_bar) {
    delete energy_bar;
  }
  body            = NULL;
  name_text       = NULL;
  particle_engine = NULL;
  energy_bar      = NULL;
}

void Character::SignalDrowning()
{
  // Follow character
  Camera::GetInstance()->FollowObject(this);

  // Set energy
  SetEnergy(0, &ActiveCharacter());
  SetMovement("drowned", true);
  JukeBox::GetInstance()->Play(GetTeam().GetSoundProfile(),"sink");
  Game::GetInstance()->SignalCharacterDeath (this);
}

// Signal the character death (short life as you can notice)
// May you rest in peace young one.
void Character::SignalGhostState(bool was_dead)
{
  // Report to damage performer this character lost all of its energy
  ActiveCharacter().damage_stats->MadeDamage(GetEnergy(), *this);

  MSG_DEBUG("character", "ghost");
  // Signal the death
  if (!was_dead) Game::GetInstance()->SignalCharacterDeath(this);
}

void Character::SetDirection(LRDirection nv_direction)
{
  body->SetDirection(nv_direction);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
  m_team.crosshair.Refresh(GetFiringAngle());
}

bool Character::MustDrawLostEnergy() const
{
  if ((IsActiveCharacter()
       && Game::GetInstance()->ReadState() != Game::END_TURN)
      || IsDead())
    return false;

  return lost_energy != 0;
}

bool Character::MustDrawEnergyBar() const
{
  if (Config::GetInstance()->GetDisplayEnergyCharacter()
      && ((!IsActiveCharacter()
           && Game::GetInstance()->ReadState() != Game::END_TURN
           && !IsDead())
          || MustDrawLostEnergy()))
    return true;

  return false;
}

bool Character::MustDrawName() const
{
  if (!MustBeDrawn())
    return false;

  if (Config::GetInstance()->GetDisplayNameCharacter()
      && !IsActiveCharacter()
      && Game::GetInstance()->ReadState() != Game::END_TURN)
    return true;

  return false;
}

void Character::DrawEnergyBar() const
{
  if (!MustDrawEnergyBar())
    return;

  int dy = -SPACE - HEIGHT_ENERGY;
  energy_bar->DrawXY(Point2i(GetCenterX() - energy_bar->GetWidth()/2, GetY() + dy)
                     - Camera::GetInstance()->GetPosition());
}

void Character::DrawLostEnergy() const
{
  if (!MustDrawLostEnergy())
    return;

  std::ostringstream ss;
  ss << lost_energy;
  int dy = - SPACE - HEIGHT_FONT_NAME - SPACE - HEIGHT_ENERGY;
  if (MustDrawName()) {
    dy = dy - HEIGHT_FONT_NAME - SPACE;
  }
  Text text(ss.str(), m_team.GetColor());
  text.DrawCenterTop(GetPosition() - Camera::GetInstance()->GetPosition() +
                     Point2i(GetWidth()/2, dy));
}

void Character::DrawName() const
{
  if (!MustDrawName())
    return;

  int dy = -SPACE - HEIGHT_FONT_NAME;
  if (MustDrawEnergyBar()) {
    dy = dy - HEIGHT_ENERGY - SPACE;
  }

  const int x = GetCenterX();
  const int y = GetY() + dy;
  name_text->DrawCenterTopOnMap(Point2i(x,y));
}

void Character::ResetDamageStats()
{
  damage_stats->ResetDamage();
}

void Character::ApplyDiseaseDamage()
{
  // Copy a bit SetEnergyDelta
  // If already dead, do nothing
  if (IsDead()) return;

  SetEnergyDelta(-(int)GetDiseaseDamage(), disease_dealer);
}

void Character::SetEnergyDelta(int delta, Character* dealer)
{
  // If already dead, do nothing
  if (IsDead()) return;

  MSG_DEBUG("character.energy", "%s has win %d life points\n",
            character_name.c_str(), delta);

  uint saved_energy = GetEnergy();

  // Update energy
  SetEnergy(GetEnergy() + delta, dealer);

  if (IsDead())
    return;

  // Compute energy lost
  if (delta < 0) {
    lost_energy += (int)GetEnergy() - (int)saved_energy;

    if (lost_energy > -33)
      JukeBox::GetInstance()->Play(GetTeam().GetSoundProfile(), "injured_light");
    else if (lost_energy > -66)
      JukeBox::GetInstance()->Play(GetTeam().GetSoundProfile(), "injured_medium");
    else
      JukeBox::GetInstance()->Play(GetTeam().GetSoundProfile(), "injured_high");

    // If energy was lost, let's stop playing the idle animation
    if (!body->GetMovement().compare(0, 9, "animation") ||
        !body->GetClothe().compare(0, 9, "animation")) {
      SetClothe("normal");
      SetMovement("breathe");
    }
    // Delay a bit the occurrence of the animation
    animation_time = GameTime::GetInstance()->Read()
                   + (ANIM_PAUSE_MAX + ANIM_PAUSE_MIN)/2;
  } else
    lost_energy = 0;

  // Report damage to damage performer
  if (dealer && lost_energy < 0)
      dealer->damage_stats->MadeDamage(-lost_energy, *this);

  // "Friendly fire !!"
  if (dealer!=this && dealer && dealer->GetTeam().IsSameAs(m_team))
    JukeBox::GetInstance()->Play(m_team.GetSoundProfile(), "friendly_fire");
}

void Character::SetEnergy(int new_energy, Character* dealer)
{
  int diff = new_energy - m_energy;
  if (diff < 0) {
    Particle *tmp = new FadingText(int2str(diff));
    tmp->SetXY(GetPosition());
    ParticleEngine::AddNow(tmp);
  }

  if (IsDead()) return;

  // Change energy
  m_energy = InRange_Long((int)new_energy, 0,
                          GameMode::GetInstance()->character.max_energy);
  energy_bar->Actu(m_energy);

  // Dead character ?
  if (GetEnergy() <= 0) Die(dealer);
}

void Character::Die(Character* killer)
{
  ASSERT (m_alive == ALIVE || m_alive == DROWNED);

  MSG_DEBUG("character", "Dying");

  if (m_alive != DROWNED) {
    m_alive = DEAD;

    SetEnergy(0, killer);

    JukeBox::GetInstance()->Play(GetTeam().GetSoundProfile(),"death");
    body->SetRotation(0.0);
    SetClothe("dead");
    SetMovement("breathe");
    SetCollisionModel(true, false, false);

    ud_move_intentions.clear();
    lr_move_intentions.clear();

    if (death_explosion)
      ApplyExplosion(GetCenter(), GameMode::GetInstance()->death_explosion_cfg);
    ASSERT(IsDead());

    // Signal the death
    Game::GetInstance()->SignalCharacterDeath(this, killer);
  }

  damage_stats->SetDeathTime(GameTime::GetInstance()->Read());
  Camera::GetInstance()->StopFollowingObj(this);
}

bool Character::MustBeDrawn() const
{
  if (hidden)
    return false; // Character is teleporting...

  // Gone in another world ?
  if (IsGhost())
    return false;

  // Character is visible on camera? If not, just leave the function
  // WARNING, this optimization is disabled if it is the active character
  // because there could be some tricks in the drawing of the weapon (cf bug #10242)
  if (!IsActiveCharacter()) {
    // Beware that GetTestRect on character returns a smaller rectangle,
    // so you can't use Camera::IsVisible
    Rectanglei rect(GetPosition(), GetSize());
    if (!rect.Intersect(*Camera::GetInstance()))
      return false;
  }

  return true;
}

void Character::Draw()
{
  if (!MustBeDrawn())
    return;

  Point2i pos = GetPosition();
  body->Draw(pos);

  DrawEnergyBar();

  //DrawName();

  DrawLostEnergy();

#ifdef DEBUG
  if (IsLOGGING("body"))
  {
    int dy = -SPACE - HEIGHT_FONT_NAME;
    if (MustDrawEnergyBar())
      dy = -SPACE - HEIGHT_ENERGY;
    if (MustDrawName())
      dy = -SPACE - HEIGHT_FONT_NAME;
    if (MustDrawLostEnergy())
      dy = -SPACE - HEIGHT_FONT_NAME;

    std::string txt = body->GetClothe() + " " + body->GetMovement() + " " + body->GetFrameLoop();
    Text skin_text(txt);
    skin_text.DrawCenterTopOnMap(Point2i(GetX(), GetY() - dy));
  }

  if (IsLOGGING("test_rectangle"))
  {
    Rectanglei test_rect(GetTestRect());
    test_rect.SetPosition(test_rect.GetPosition() - Camera::GetInstance()->GetPosition());
    GetMainWindow().RectangleColor(test_rect, primary_red_color, 1);

    Rectanglei rect(GetPosition() - Camera::GetInstance()->GetPosition(), GetSize());
    GetMainWindow().RectangleColor(rect, primary_blue_color, 1);
  }
#endif
}

void Character::Jump(Double strength, Double angle /*in radian */)
{
  Camera::GetInstance()->FollowObject(this);

  UpdateLastMovingTime();
  walking_time = GameTime::GetInstance()->Read();

  if (!CanJump()) return;

  SetMovement("jump");

  // Jump !
  if (GetDirection() == DIRECTION_LEFT) angle = InverseAngle(angle);
  SetSpeed (strength, angle);
}

void Character::Jump()
{
  MSG_DEBUG("character", "Jump");
  JukeBox::GetInstance()->Play (ActiveTeam().GetSoundProfile(), "jump");
  Jump(GameMode::GetInstance()->character.jump_strength,
       GameMode::GetInstance()->character.jump_angle);
}

void Character::HighJump()
{
  MSG_DEBUG("character", "HighJump");
  JukeBox::GetInstance()->Play (ActiveTeam().GetSoundProfile(), "superjump");
  Jump(GameMode::GetInstance()->character.super_jump_strength,
       GameMode::GetInstance()->character.super_jump_angle);
}

void Character::BackJump()
{
  MSG_DEBUG("character", "BackJump");
  back_jumping = true;
  JukeBox::GetInstance()->Play (ActiveTeam().GetSoundProfile(), "back_jump");
  Jump(GameMode::GetInstance()->character.back_jump_strength,
       GameMode::GetInstance()->character.back_jump_angle);
}

void Character::PrepareShoot()
{
  MSG_DEBUG("weapon.shoot", "-> begin");
  SetMovementOnce("weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot");
  if (body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot")
  {
    MSG_DEBUG("weapon.shoot", "-> call DoShoot");
    // If a movement is defined for this weapon, just shoot
    DoShoot();
    MSG_DEBUG("weapon.shoot", "<- end of call DoShoot");
  }
  else
    prepare_shoot = true;
  MSG_DEBUG("weapon.shoot", "<- end");
}

void Character::DoShoot()
{
  if (Game::GetInstance()->ReadState() != Game::PLAYING) {
    MSG_DEBUG("weapon.shoot", "DoShoot cancelled! time: %u", GameTime::GetInstance()->Read());
    return; // hack related to bugs 8656 and 9462
  }

  MSG_DEBUG("weapon.shoot", "-> begin at time %u", GameTime::GetInstance()->Read());
  SetMovement("weapon-" + ActiveTeam().GetWeapon().GetID() + "-end-shoot");
  body->Build(); // Refresh the body
  damage_stats->OneMoreShot();
  ActiveTeam().AccessWeapon().Shoot();
  MSG_DEBUG("weapon.shoot", "<- end");
}

void Character::UpdateLastMovingTime()
{
  do_nothing_time = GameTime::GetInstance()->Read();
}

void Character::Refresh()
{
  StartOrStopWalkingIfNecessary();
  if (IsWalking())
    MakeSteps();

  UpdateFiringAngle();

  if (IsGhost()) return;

  UpdatePosition();

  if (IsDead()) return;

  GameTime * global_time = GameTime::GetInstance();

  // center on character who is falling
  if (IsFalling()) {
    bool closely = false;
    if (IsActiveCharacter() &&
        (ActiveTeam().GetWeaponType() == Weapon::WEAPON_JETPACK
         || ActiveTeam().GetWeaponType() == Weapon::WEAPON_PARACHUTE))
      closely = true;
    Camera::GetInstance()->FollowObject(this, closely);
  }

  if (IsDiseased()) {
    Point2i bubble_pos = GetPosition();
    if (GetDirection() == DIRECTION_LEFT)
      bubble_pos.x += GetWidth();
    particle_engine->AddPeriodic(bubble_pos, particle_ILL_BUBBLE, false,
                                 - HALF_PI - (Double)GetDirection() * QUARTER_PI, 20.0);
  }

  if (IsActiveCharacter() && Game::GetInstance()->ReadState() == Game::PLAYING) {
    if (do_nothing_time + do_nothing_timeout < global_time->Read())
      CharacterCursor::GetInstance()->FollowActiveCharacter();

    if (walking_time + 1000 < global_time->Read() && body->GetMovement().find("-shoot") == std::string::npos)
      if (body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-select")
        body->SetMovement("weapon-" + ActiveTeam().GetWeapon().GetID() + "-select");
  } else {
    if (body->GetMovement() == "weapon-" + ActiveTeam().GetWeapon().GetID() + "-select")
      body->SetMovement("breathe");
  }

  if (body->IsWalking() && body->GetMovement() == "walk") {
    // Play the step sound only twice during the walking animation
    uint frame_nbr = body->GetFrameCount()>>1;
    uint cur = body->GetFrame();
    cur %= frame_nbr;

    if (cur < frame_nbr>>1 && !step_sound_played) {
      step_sound_played = true;
      JukeBox::GetInstance()->Play (GetTeam().GetSoundProfile(),"step");
    }

    if (cur > frame_nbr>>1)
      step_sound_played = false;
  }

  if (back_jumping) {
    ASSERT(&ActiveCharacter() == this);
    Double rotation;
    static Double speed_init = GameMode::GetInstance()->character.back_jump_strength *
       sin(GameMode::GetInstance()->character.back_jump_angle);

    Point2d speed = GetSpeedXY();
    rotation = PI * speed.y / speed_init;
    body->SetRotation(rotation);
  }

  if (Game::GetInstance()->ReadState() == Game::END_TURN && body->IsWalking())
    body->StopWalking();

  if (GameTime::GetInstance()->Read() > animation_time
      && !IsActiveCharacter()
      && !IsDead()
      && !IsFalling()
      && body->GetMovement().compare(0, 9, "animation")
      && body->GetClothe().compare(0, 9, "animation")) {
    body->PlayAnimation();
    MSG_DEBUG("random.get", "Character::Refresh()");
    animation_time = GameTime::GetInstance()->Read() + body->GetMovementDuration()
                   + RandomSync().GetUint(ANIM_PAUSE_MIN, ANIM_PAUSE_MAX);
  }

  // Stop the animation or the black skin if we are playing
  if (IsActiveCharacter()
      && Game::GetInstance()->ReadState() == Game::PLAYING
      && (!body->GetMovement().compare(0, 9, "animation") ||
          !body->GetClothe().compare(0, 9, "animation") ||
          body->GetClothe() == "black")) {
    SetClothe("normal");
    SetMovement("breathe");
  }

  Double n, a;
  GetSpeed(n, a);
  if (n > MIN_SPEED_TO_FLY && body->GetMovement() != "fly-black"
      && body->GetMovement() != "fall" && body->GetMovement() != "jump"
      && body->GetMovement() != "fly" && body->GetMovement().compare(0, 7, "jetpack")) {
    if (IsGoingUp() || IsGoingDown())
      SetMovement("fall");
    else
      SetMovement("fly");
  }

  // Refresh the body (needed to determine if "weapon-*-begin-shoot" is finnished)
  body->Build();

  if (prepare_shoot) {
    if (body->GetMovement() != "weapon-" + ActiveTeam().GetWeapon().GetID() + "-begin-shoot") {
      // if the movement is finnished, shoot !
      DoShoot();
      prepare_shoot = false;
    }
  }
}

void Character::UpdateFiringAngle()
{
  bool can_change = IsImmobile() && !GetTeam().AccessWeapon().IsPreventingLRMovement();
  const UDMoveIntention * ud_move_intention = GetLastUDMoveIntention();
  if (can_change && ud_move_intention) {
    UpdateLastMovingTime();
    CharacterCursor::GetInstance()->Hide();
    Double delta = DELTA_CROSSHAIR;
    if (ud_move_intention->IsToDoItSlowly())
      delta /= 10;
    if (ud_move_intention->GetDirection() == DIRECTION_UP)
      delta = -delta;
    AddFiringAngle(delta);
  }
}

// Prepare a new turn
void Character::PrepareTurn()
{
  damage_stats->HandleMostDamage();
  lost_energy = 0;
  rl_motion_pause = GameTime::GetInstance()->Read();
}

// Signal the end of a fall
void Character::Collision(const Point2d& speed_vector, const Double& contactAngle)
{
  // Do not manage dead characters.
  if (IsDead()) return;

  rl_motion_pause = GameTime::GetInstance()->Read();

  GameMode * game_mode = GameMode::GetInstance();
  if (body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID()
      && body->GetClothe() != "jetpack"
      && body->GetClothe() != "jetpack-fire"
      /* && body->GetClothe() != "black"*/
      && body->GetClothe() != "helmet")
    SetClothe("normal");

  if (body->IsWalking())
    SetMovement("walk");
  else
    SetMovement("breathe");

  body->SetRotation(0.0);
  back_jumping = false;

  Double norm = speed_vector.Norm()*abs(sin((speed_vector.ComputeAngle()+HALF_PI)-contactAngle));

  if (norm > game_mode->safe_fall) {
    norm -= game_mode->safe_fall;
    Double degat = norm * game_mode->damage_per_fall_unit;
    // If the player was clumsy and felt, he is the active character and the damage dealer
    // If the player was blasted, this is because the weapon holder is the active character and the damage dealer
    SetEnergyDelta(-(int)degat, &ActiveCharacter());
    Game::GetInstance()->SignalCharacterDamage(this);
    SetClothe("normal");

    SetMovementOnce("hard-land");
  } else {
    SetMovementOnce("soft-land");
  }
}

void Character::SignalGroundCollision(const Point2d& speed_before, const Double& contactAngle)
{
  MSG_DEBUG("character.collision", "%s collides on ground with speed %s, %s (norm = %s, angle=%s, contactAngle=%s)",
            character_name.c_str(),
            Double2str(speed_before.x).c_str(),
            Double2str(speed_before.y).c_str(),
            Double2str(speed_before.Norm()).c_str(),
            Double2str(speed_before.ComputeAngle()).c_str(),
            Double2str(contactAngle).c_str());

  Collision(speed_before, contactAngle);
}

void Character::SignalObjectCollision(const Point2d& my_speed_before,
                                      PhysicalObj * /* obj */,
                                      const Point2d& /* obj_speed */)
{
  MSG_DEBUG("character.collision", "%s collides on object with speed %s, %s (norm = %s)",
            character_name.c_str(),
            Double2str(my_speed_before.x).c_str(),
            Double2str(my_speed_before.y).c_str(),
            Double2str(my_speed_before.Norm()).c_str());
  // In case an object collides with the character, we don't want
  // the character to have huge damage because of the speed of the object.
  // Damage should be applied when felt or when hurted by a weapon.
  Collision(my_speed_before, my_speed_before.ComputeAngle());
}

void Character::SignalExplosion()
{
  if (IsDead()) return;

  // Motion can't be Pendulum, so it's fine using this here
  Double n = GetSpeed().Norm();
  SetRebounding(true);

  SetClotheOnce("black");

  if (n > MIN_SPEED_TO_FLY) {
    if (IsGoingUp() || IsGoingDown())
      SetMovement("fall");
    else
      SetMovement("fly-black");
  } else {
    SetMovementOnce("black");
    if (body->GetClothe() == "black" && body->GetMovement() != "black")
      std::cerr << "Error: the clothe \"black\" of the character " << GetName()
                << " is set, but the skin have no \"black\" movement !!!" << std::endl;
  }

  Game::GetInstance()->SignalCharacterDamage(this);
}

LRDirection Character::GetDirection() const
{
  return body->GetDirection();
}

// End of turn or change of character
void Character::StopPlaying()
{
  is_playing = false;

  if (IsDead()) return;
  SetClothe("normal");
  SetMovement("breathe");
  if (IsWalking())
    StopWalking();
  SetRebounding(true);
  lr_move_intentions.clear();
  ud_move_intentions.clear();
}

// Begining of turn or changed to this character
void Character::StartPlaying()
{
  if (is_playing) return;

  is_playing = true;

  ASSERT (!IsGhost());
  SetWeaponClothe();
  ActiveTeam().crosshair.Draw();
 // SetRebounding(false);
  ShowGameInterface();
  m_team.crosshair.Refresh(GetFiringAngle());
}

bool Character::IsActiveCharacter() const
{
  return (this == &ActiveCharacter());
}

// Hand position
void Character::GetRelativeHandPosition(Point2i & result) const
{
  body->GetRelativeHandPosition(result);
}

Double Character::GetFiringAngle() const {
  if (GetDirection() == DIRECTION_LEFT)
    return InverseAngleRad(firing_angle);
  return firing_angle;
}

#include <iostream>
void Character::SetFiringAngle(Double angle) {
  /* angle = RestrictAngle(angle) */
  angle = InRange_Double(angle, -(ActiveTeam().GetWeapon().GetMaxAngle()),
                                -(ActiveTeam().GetWeapon().GetMinAngle()));
  firing_angle = angle;
  m_team.crosshair.Refresh(GetFiringAngle());
  body->Rebuild();
}

void Character::SetWeaponClothe()
{
  SetClothe("weapon-" + m_team.GetWeapon().GetID());
  if (body->GetClothe() != "weapon-" + m_team.GetWeapon().GetID())
    SetClothe("normal");
  SetMovement("breathe");
}

void Character::SetMovement(const std::string& name, bool force)
{
  if (IsDead() && !force) return;
  MSG_DEBUG("body","Character %s -> SetMovement : %s",character_name.c_str(),name.c_str());
  body->SetMovement(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetMovementOnce(const std::string& name, bool force)
{
  if (IsDead() && !force) return;
  MSG_DEBUG("body","Character %s -> SetMovementOnce : %s",character_name.c_str(),name.c_str());
  body->SetMovementOnce(name);
  uint l,r,t,b;
  body->GetTestRect(l,r,t,b);
  SetTestRect(l,r,t,b);
}

void Character::SetClothe(const std::string& name, bool force)
{
  if (IsDead() && name!="dead" && !force) return;
  MSG_DEBUG("body","Character %s -> SetClothe : %s",character_name.c_str(),name.c_str());
  body->SetClothe(name);
}

void Character::SetClotheOnce(const std::string& name, bool force)
{
  if (IsDead() && !force) return;
  MSG_DEBUG("body","Character %s -> SetClotheOnce : %s",character_name.c_str(),name.c_str());
  body->SetClotheOnce(name);
}

uint Character::GetTeamIndex() const
{
  int index;
  GetTeamsList().FindPlayingById(GetTeam().GetId(), index);
  ASSERT(index != -1);
  return (uint)index;
}

uint Character::GetCharacterIndex() const
{
  uint index = 0;
  for (Team::iterator it = m_team.begin(); it != m_team.end() ; ++it, ++index) {
    if (&(*it) == this)
      return index;
  }
  ASSERT(false);
  return 0;
}

// ###################################################################
// ###################################################################
// ###################################################################

void Character::StartOrStopWalkingIfNecessary()
{
  const LRMoveIntention * lr_move_intention = GetLastLRMoveIntention();
  bool should_walk = lr_move_intention
    && !GetTeam().AccessWeapon().IsPreventingLRMovement()
    && HasGroundUnderFeets()
    && !IsDead();
  if (should_walk) {
    if (lr_move_intention->GetDirection() != GetDirection() && !IsChangingDirection()) {
      SetDirection(lr_move_intention->GetDirection());
      last_direction_change = GameTime::GetInstance()->Read();
    }
  }
  if (should_walk && !IsChangingDirection()) {
    bool should_be_slowly = lr_move_intention->IsToDoItSlowly();
    if (IsWalking() && should_be_slowly != walking_slowly)
      StopWalking();
    if (!IsWalking())
      StartWalking(should_be_slowly);
  } else {
    if (IsWalking())
      StopWalking();
  }
}

void Character::StartWalking(bool slowly)
{
  walking_time = GameTime::GetInstance()->Read();
  rl_motion_pause = max(rl_motion_pause , GameTime::GetInstance()->Read());
  step_sound_played = true;
  walking_slowly = slowly;

  Camera::GetInstance()->FollowObject(this);
  if (Network::GetInstance()->IsTurnMaster()) {
    HideGameInterface();
    ActiveTeam().crosshair.Hide();
  }
  CharacterCursor::GetInstance()->Hide();
  UpdateLastMovingTime();
  Game::GetInstance()->SetCharacterChosen(true);

  if (!slowly)
    SetMovement("walk");
  body->StartWalking();
}

void Character::StopWalking()
{
  if (Network::GetInstance()->IsTurnMaster())
    ActiveTeam().crosshair.Show();
  body->StopWalking();
}

bool Character::IsWalking() const
{
  return body->IsWalking();
}

void Character::MakeSteps()
{
  int height;
  bool ghost;
  uint walking_pause = GameMode::GetInstance()->character.walking_pause;

  const LRMoveIntention * lr_move_intention = GetLastLRMoveIntention();
  ASSERT(lr_move_intention);
  if (lr_move_intention->IsToDoItSlowly())
    walking_pause *= 10;
  else
    SetMovement("walk");// otherwise character would slide after dropping a dynamite

  // If character moves out of the world, no need to go further: it is dead
  if (GetDirection() == DIRECTION_LEFT)
    ghost = IsOutsideWorld(Point2i(-1, 0));
  else
    ghost = IsOutsideWorld(Point2i(1, 0));

  if (ghost) {
    MSG_DEBUG("ghost", "%s will be a ghost.", GetName().c_str());
    Ghost();
    return;
  }

  // Check we can move (to go not too fast)
  while (rl_motion_pause+walking_pause < GameTime::GetInstance()->Read() &&
         ComputeHeightMovement(height)) {
    walking_time = GameTime::GetInstance()->Read();
    rl_motion_pause = rl_motion_pause + walking_pause;

    // Eventually moves the character
    SetXY(Point2i(GetX() + GetDirection(), GetY() + height));

    // If no collision, let gravity do its job
    UpdatePosition();
  }
}

bool Character::IsChangingDirection()
{
  return last_direction_change + PAUSE_CHG_DIRECTION >= GameTime::GetInstance()->Read();
}

bool Character::ComputeHeightMovement(int & height)
{
  if (IsInVacuum(Point2i(GetDirection(), 0))
      && !IsInVacuum(Point2i(GetDirection(), +1))){
    //Land is flat, we can move!
    height = 0;
    return true;
  }

  //Compute height of the step:
  if (IsInVacuum(Point2i(GetDirection(), 0))) {
    //Try to go down:
    for (height = 2; height <= MAX_FALLING_HEIGHT ; height++) {
      if (!IsInVacuum(Point2i(GetDirection(), height))) {
        height--;
        return true;
      }
    }

    //We can go down, but the step is too big -> the character will fall
    bool falling = true;
    if (falling) {
      SetX (GetXDouble() + GetDirection());
      UpdatePosition();
      SetMovement("fall");
    }
    return false;
  } else {
    //Try to go up:
    for (height = -1; height >= -MAX_CLIMBING_HEIGHT ; height--) {
      if (IsInVacuum(Point2i(GetDirection(), height))) {
        return true;
      }
    }
  }
  //We can't move!
  return false;
}

void Character::HandleKeyPressed_MoveRight(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_RIGHT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

void Character::HandleKeyReleased_MoveRight(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_RIGHT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

void Character::HandleKeyPressed_MoveLeft(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_LEFT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

void Character::HandleKeyReleased_MoveLeft(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_LEFT);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

// #################### UP

void Character::HandleKeyPressed_Up(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_UP);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

void Character::HandleKeyReleased_Up(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_UP);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

// #################### DOWN

void Character::HandleKeyPressed_Down(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_START_MOVING_DOWN);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

void Character::HandleKeyReleased_Down(bool slowly)
{
  Action *a = new Action(Action::ACTION_CHARACTER_STOP_MOVING_DOWN);
  a->Push(slowly ? 1 : 0);
  ActionHandler::GetInstance()->NewAction(a);
}

// #################### JUMP

void Character::HandleKeyPressed_Jump()
{
  if (ActiveTeam().AccessWeapon().IsPreventingJumps())
    return;
  HideGameInterface();

  ActiveTeam().crosshair.Hide();

  if (IsImmobile()) {
    Action *a = new Action(Action::ACTION_CHARACTER_JUMP);
    ActionHandler::GetInstance()->NewAction(a);
  }
}

// #################### HIGH JUMP
void Character::HandleKeyPressed_HighJump()
{
  if (ActiveTeam().AccessWeapon().IsPreventingJumps())
    return;
  HideGameInterface();

  ActiveTeam().crosshair.Hide();

  if (IsImmobile()) {
    Action *a = new Action(Action::ACTION_CHARACTER_HIGH_JUMP);
    ActionHandler::GetInstance()->NewAction(a);
  }
}

// #################### BACK JUMP
void Character::HandleKeyPressed_BackJump()
{
  if (ActiveTeam().AccessWeapon().IsPreventingJumps())
    return;
  HideGameInterface();

  ActiveTeam().crosshair.Hide();

  if (IsImmobile()) {
    Action *a = new Action(Action::ACTION_CHARACTER_BACK_JUMP);
    ActionHandler::GetInstance()->NewAction(a);
  }
}
