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
 * Options menu
 *****************************************************************************/
#include <sstream>
#include <string>
#include <iostream>
#include <WARMUX_download.h>
#include "menu/options_menu.h"
#include "include/app.h"
#include "include/constant.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/config.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "gui/button.h"
#include "gui/control_config.h"
#include "gui/label.h"
#include "gui/grid_box.h"
#include "gui/big/button_pic.h"
#include "gui/combo_box.h"
#include "gui/check_box.h"
#include "gui/picture_widget.h"
#include "gui/picture_text_cbox.h"
#include "gui/select_box.h"
#include "gui/spin_button_picture.h"
#include "gui/tabs.h"
#include "gui/text_box.h"
#include "gui/question.h"
#include "gui/vertical_box.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/custom_team.h"
#include "team/custom_teams_list.h"
#include "tool/string_tools.h"
#include "tool/resource_manager.h"

OptionMenu::OptionMenu() :
  Menu("menu/bg_option")
{
  AppWarmux * app = AppWarmux::GetInstance();
  Config * config = Config::GetInstance();
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  int window_w = app->video->window.GetWidth();
  int window_h = app->video->window.GetHeight();

  Point2i option_size;
  float   factor;

  if (window_w<640 || window_h<480) {
    // Everything smaller to try to squeeze it
    option_size.SetValues(104, 104);
    factor = 0.02f;
  } else {
    option_size.SetValues(130, 130);
    factor = 0.05f;
  }

  int border = window_w * factor;
  int max_w  = window_w - 2*border;

  /* Tabs */
  MultiTabs * tabs =
    new MultiTabs(Point2i(max_w, window_h - actions_buttons->GetSizeY() -border));
  tabs->SetPosition(border, border);

  /* Graphic options */
  Box * graphic_options = new GridBox(2, 4, 0, false);

  // Various options
  opt_wind_particles_percentage =
    new SpinButtonWithPicture(_("Wind particles?"), "menu/display_wind_particles",
                              option_size, 100, 20, 0, 100);
  graphic_options->AddWidget(opt_wind_particles_percentage);

#ifndef HAVE_HANDHELD
  opt_display_multisky =
    new PictureTextCBox(_("Multi-layer sky?"), "menu/multisky", option_size);
  graphic_options->AddWidget(opt_display_multisky);
#endif

  opt_display_energy =
    new PictureTextCBox(_("Player energy?"), "menu/display_energy", option_size);
  graphic_options->AddWidget(opt_display_energy);

  opt_display_name =
    new PictureTextCBox(_("Player's name?"), "menu/display_name", option_size);
  graphic_options->AddWidget(opt_display_name);

#ifndef HAVE_TOUCHSCREEN
  full_screen =
    new PictureTextCBox(_("Fullscreen?"), "menu/fullscreen", option_size);
  graphic_options->AddWidget(full_screen);
#endif

  opt_max_fps =
    new SpinButtonWithPicture(_("Maximum FPS"), "menu/fps",
#ifdef DEBUG
                              option_size, 30, 5, 20,1000);
#else
                              option_size, 30, 5, 20,  60);
#endif
  graphic_options->AddWidget(opt_max_fps);
  std::vector< std::pair<std::string, std::string> > qualities;
  qualities.push_back(std::pair<std::string, std::string>("0", _("Low memory")));
  qualities.push_back(std::pair<std::string, std::string>("1", _("Medium")));
#ifndef HAVE_HANDHELD
  qualities.push_back(std::pair<std::string, std::string>("2", _("High")));
#endif
  opt_quality = new ComboBox(_("Quality"), "menu/fps", option_size,
                             qualities, qualities[config->GetQuality()].first);
  graphic_options->AddWidget(opt_quality);

#ifndef HAVE_TOUCHSCREEN
  // Get available video resolution
  const std::list<Point2i>& video_res = app->video->GetAvailableConfigs();
  std::list<Point2i>::const_iterator mode;
  std::vector<std::pair<std::string, std::string> > video_resolutions;
  std::string current_resolution;

  for (mode = video_res.begin(); mode != video_res.end(); ++mode) {
    std::ostringstream ss;
    std::string text;
    ss << mode->GetX() << "x" << mode->GetY() ;
    text = ss.str();
    if (window_w == mode->GetX() && window_h == mode->GetY())
      current_resolution = text;

    video_resolutions.push_back (std::pair<std::string, std::string>(text, text));
  }
  cbox_video_mode =
    new ComboBox(_("Resolution"), "menu/resolution", option_size,
                 video_resolutions, current_resolution);
  graphic_options->AddWidget(cbox_video_mode);
#endif

  tabs->AddNewTab("unused", _("Graphics"), graphic_options);

#ifdef ENABLE_NLS
  /* Language selection */
  lbox_languages = new ItemBox(tabs->GetSize() - 10);
  tabs->AddNewTab("unused", _("Language"), lbox_languages);
#endif

  /* Controls editing */
  controls = new ControlConfig(tabs->GetSize() - 10, false);
  tabs->AddNewTab("unused", _("Key bindings"), controls);

  /* Team editor */

  // bug #12193 : Missed assertion in game option (custom team editor) while playing
  if (!GameIsRunning()) {
    Box * teams_editor = new HBox(option_size.y, false, false, true);

    lbox_teams = new ItemBox(option_size, false);
    teams_editor->AddWidget(lbox_teams);

    uint lwidth = max_w - option_size.x - 30;
    Box * teams_editor_inf = new VBox(lwidth, true, true, false);
    Box * box_team_name = new HBox(30, false, false, true);

    team_name = new Label(Format("%s:", _("Head commander")), 100,
                          Font::FONT_SMALL, Font::FONT_BOLD);
    box_team_name->AddWidget(team_name);

    tbox_team_name = new TextBox("", lwidth - 100 - 40,
                                 Font::FONT_SMALL, Font::FONT_BOLD);
    box_team_name->AddWidget(tbox_team_name);

    teams_editor_inf->AddWidget(box_team_name);

    Label* label_ch_names = new Label(_("Character names:"), 0,
                                      Font::FONT_SMALL, Font::FONT_BOLD);
    teams_editor_inf->AddWidget(label_ch_names);

    Box * teams_editor_names = new GridBox(5, 2, 2, false);

    for (uint i=0; i < 10; i++) {
      std::ostringstream oss;
      oss << i+1 << ":";
      tbox_character_name_list.push_back(new TextBox("", lwidth/2 - 40,
                                                     Font::FONT_SMALL, Font::FONT_BOLD));
      Label * lab = new Label(oss.str(), 30, Font::FONT_SMALL, Font::FONT_BOLD);

      Box * name_box = new HBox(20, false, false, true);
      name_box->SetNoBorder();

      name_box->AddWidget(lab);
      name_box->AddWidget(tbox_character_name_list[i]);

      teams_editor_names->AddWidget(name_box);
    }

    teams_editor_inf->AddWidget(teams_editor_names);

    Box * team_action_box = new HBox(35, false, false, true);
    team_action_box->SetNoBorder();

    add_team = new Button(res, "menu/add_custom_team");
    team_action_box->AddWidget(add_team);

    delete_team = new Button(res, "menu/del_custom_team");
    team_action_box->AddWidget(delete_team);

    teams_editor_inf->AddWidget(team_action_box);

    teams_editor_inf->Pack();
    teams_editor->AddWidget(teams_editor_inf);
    tabs->AddNewTab("unused", _("Teams editor"), teams_editor);
    selected_team = NULL;
    ReloadTeamList();
  } else {
    lbox_teams = NULL;
    add_team = NULL;
    delete_team = NULL;
    selected_team = NULL;
    tbox_team_name = NULL;
    team_name = NULL;
  }

#if USE_MISC_TAB
  /* Misc options */
  Box * misc_options = new GridBox(3, 3, 0, false);

  opt_updates =
    new PictureTextCBox(_("Check updates online?"),
                        "menu/ico_update", option_size);
  misc_options->AddWidget(opt_updates);

#ifndef HAVE_TOUCHSCREEN
  opt_lefthanded_mouse =
    new PictureTextCBox(_("Left-handed mouse?"),
                        "menu/ico_lefthanded_mouse", option_size);
  misc_options->AddWidget(opt_lefthanded_mouse);

  opt_scroll_on_border =
    new PictureTextCBox(_("Scroll on border"),
                        "menu/scroll_on_border", option_size);
  misc_options->AddWidget(opt_scroll_on_border);

  opt_scroll_border_size =
    new SpinButtonWithPicture(_("Scroll border size"), "menu/scroll_on_border",
                              option_size, 50, 5, 5, 80);
  misc_options->AddWidget(opt_scroll_border_size);
#endif

  tabs->AddNewTab("unused", _("Misc"), misc_options);
#endif

  /* Sound options */
  Box * sound_options = new GridBox(3, 3, 0, false);

  music_cbox =
    new PictureTextCBox(_("Music?"), "menu/music_enable", option_size);
  sound_options->AddWidget(music_cbox);

  initial_vol_mus = config->GetVolumeMusic();
  volume_music =
    new SpinButtonWithPicture(_("Music volume"), "menu/music_enable",
                              option_size,
                              fromVolume(initial_vol_mus), 5, 0, 100);
  sound_options->AddWidget(volume_music);

  effects_cbox =
    new PictureTextCBox(_("Sound effects?"),
                        "menu/sound_effects_enable", option_size);
  sound_options->AddWidget(effects_cbox);

  initial_vol_eff = config->GetVolumeEffects();
  volume_effects =
    new SpinButtonWithPicture(_("Effects volume"), "menu/sound_effects_enable",
                              option_size, fromVolume(initial_vol_eff), 5, 0, 100);
  sound_options->AddWidget(volume_effects);

  // Generate sound mode list
  uint current_freq = Config::GetInstance()->GetSoundFrequency();
  std::vector<std::pair<std::string, std::string> > sound_freqs;
  std::string current_sound_freq;
  sound_freqs.push_back(std::pair<std::string, std::string>("11025", "11 kHz"));
  sound_freqs.push_back(std::pair<std::string, std::string>("22050", "22 kHz"));
  sound_freqs.push_back(std::pair<std::string, std::string>("44100", "44 kHz"));

  if (current_freq == 44100)
    current_sound_freq = "44100";
  else if (current_freq == 22050)
    current_sound_freq = "22050";
  else
    current_sound_freq = "11025";

#ifndef HAVE_HANDHELD
  cbox_sound_freq = new ComboBox(_("Sound frequency"), "menu/sound_frequency",
                                 option_size, sound_freqs, current_sound_freq);
  sound_options->AddWidget(cbox_sound_freq);
#endif

  warn_cbox = new PictureTextCBox(_("New player warning?"),
                                  "menu/warn_on_new_player", option_size);
  sound_options->AddWidget(warn_cbox);

  tabs->AddNewTab("unused", _("Sound"), sound_options);

  // Values initialization
  opt_max_fps->SetValue(app->video->GetMaxFps());
  opt_wind_particles_percentage->SetValue(config->GetWindParticlesPercentage());
#ifndef HAVE_HANDHELD
  opt_display_multisky->SetValue(config->GetDisplayMultiLayerSky());
#endif
  opt_display_energy->SetValue(config->GetDisplayEnergyCharacter());
  opt_display_name->SetValue(config->GetDisplayNameCharacter());
#ifndef HAVE_TOUCHSCREEN
  full_screen->SetValue(app->video->IsFullScreen());
#endif
  music_cbox->SetValue(config->GetSoundMusic());
  effects_cbox->SetValue(config->GetSoundEffects());
  warn_cbox->SetValue(config->GetWarnOnNewPlayer());

#ifdef ENABLE_NLS
  // Setting language selection
    AddLanguageItem(_("(system language)"),"");
    AddLanguageItem("Български (bg)",      "bg");
    AddLanguageItem("Bosanski",            "bs");
    AddLanguageItem("Castellano",          "es");
    AddLanguageItem("Català",              "ca");
    AddLanguageItem("čeština (Czech)",     "cs");
    AddLanguageItem("Créole",              "cpf");
    AddLanguageItem("Dansk",               "da");
    AddLanguageItem("Deutsch",             "de");
    AddLanguageItem("Esperanto",           "eo");
    AddLanguageItem("English",             "en");
    AddLanguageItem("Ελληνικά",            "el");
    AddLanguageItem("Eesti keel",          "et");
    AddLanguageItem("ارسی (Farsi)",        "fa");
    AddLanguageItem("Français",            "fr");
    AddLanguageItem("Galego",              "gl");
    AddLanguageItem("עברית (Hebrew)",      "he");
    AddLanguageItem("Magyar",              "hu");
    AddLanguageItem("Italiano",            "it");
    AddLanguageItem("日本語 (japanese)",   "ja_JP");
    AddLanguageItem("Kernewek",            "kw");
    AddLanguageItem("latviešu valoda",     "lv");
    AddLanguageItem("Norsk (bokmål)",      "nb");
    AddLanguageItem("Norsk (nynorsk)",     "nn");
    AddLanguageItem("Nederlands",          "nl");
    AddLanguageItem("Polski",              "pl");
    AddLanguageItem("Português",           "pt");
    AddLanguageItem("Português do Brasil", "pt_BR");
    AddLanguageItem("Română",              "ro");
    AddLanguageItem("Pусский язык (ru)",   "ru");
    AddLanguageItem("Slovenčina",          "sk");
    AddLanguageItem("Slovenščina",         "sl");
    AddLanguageItem("Suomi",               "fi");
    AddLanguageItem("Svenska",             "sv");
    AddLanguageItem("Türkçe",              "tr");
    AddLanguageItem("украї́нська мо́ва",   "uk");
    AddLanguageItem("中文（简体）Simplified Chinese",  "zh_CN");
    AddLanguageItem("中文（正體）Traditional Chinese", "zh_TW");
#endif

#if USE_MISC_TAB
  opt_updates->SetValue(config->GetCheckUpdates());
# ifndef HAVE_TOUCHSCREEN
  opt_lefthanded_mouse->SetValue(config->GetLeftHandedMouse());
  opt_scroll_on_border->SetValue(config->GetScrollOnBorder());
  opt_scroll_border_size->SetValue(config->GetScrollBorderSize());
# endif
#endif

  widgets.AddWidget(tabs);
  widgets.Pack();
}

void OptionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* w = widgets.ClickUp(mousePosition, button);

  // Now that the click has been processed by the underlying widgets,
  // make use of their newer values in near-realtime!
  if (w == volume_music)
    Config::GetInstance()->SetVolumeMusic(toVolume(volume_music->GetValue()));
  else if (w == volume_effects) {
    Config::GetInstance()->SetVolumeEffects(toVolume(volume_effects->GetValue()));
    JukeBox::GetInstance()->Play("default", "menu/clic");
  }
  else if (w == music_cbox) {
    JukeBox::GetInstance()->ActiveMusic(music_cbox->GetValue());
  }
  else if (w == effects_cbox) {
    Config::GetInstance()->SetSoundEffects(effects_cbox->GetValue());
  }
  else if (w == add_team) {
    AddTeam();
  }
  else if (w == delete_team) {
    DeleteTeam();
  }
  else if (w == NULL && lbox_teams->Contains(mousePosition)) {
    SelectTeam();
  }
}

void OptionMenu::SaveOptions()
{
  Config * config = Config::GetInstance();

  // Graphic options
  config->SetWindParticlesPercentage(opt_wind_particles_percentage->GetValue());
  // bug #11826 : Segmentation fault while exiting the menu.
  if (GameIsRunning())
    Wind::GetRef().Reset();

#ifndef HAVE_HANDHELD
  config->SetDisplayMultiLayerSky(opt_display_multisky->GetValue());
#endif
  config->SetDisplayEnergyCharacter(opt_display_energy->GetValue());
  config->SetDisplayNameCharacter(opt_display_name->GetValue());

  // Misc options
#if USE_MISC_TAB
  config->SetCheckUpdates(opt_updates->GetValue());
# ifndef HAVE_TOUCHSCREEN
  config->SetLeftHandedMouse(opt_lefthanded_mouse->GetValue());
  config->SetScrollOnBorder(opt_scroll_on_border->GetValue());
  config->SetScrollBorderSize(opt_scroll_border_size->GetValue());
# endif
#endif

  // Sound settings - volume already saved
#ifndef HAVE_HANDHELD
  config->SetSoundFrequency(cbox_sound_freq->GetIntValue());
#endif
  config->SetSoundMusic(music_cbox->GetValue());
  config->SetSoundEffects(effects_cbox->GetValue());
  int quality = 0;
  sscanf(opt_quality->GetValue().c_str(), "%i", &quality);
  config->SetQuality((Quality)quality);

  AppWarmux * app = AppWarmux::GetInstance();
  app->video->SetMaxFps(opt_max_fps->GetValue());
  Surface &window = app->video->window;

#ifdef HAVE_TOUCHSCREEN
  app->video->SetConfig(window.GetWidth(), window.GetHeight(), true);
#else
  // Video mode
  std::string s_mode = cbox_video_mode->GetValue();

  int w, h;
  sscanf(s_mode.c_str(),"%dx%d", &w, &h);

  app->video->SetConfig(w, h, full_screen->GetValue());

  uint x = (window.GetWidth() - actions_buttons->GetSizeX())/2;
  uint y = window.GetHeight() - actions_buttons->GetSizeY();

  SetActionButtonsXY(x, y);
#endif

#if ENABLE_NLS
  // Language
  config->SetLanguage((const char*)lbox_languages->GetSelectedValue());
#endif

  // Controls
  controls->SaveControlConfig();

  // Sound
#ifndef HAVE_HANDHELD
  std::string sfreq = cbox_sound_freq->GetValue();
  int freq;
  if (str2int(sfreq,freq))
    JukeBox::GetInstance()->SetFrequency(freq);
#endif
  config->SetWarnOnNewPlayer(warn_cbox->GetValue());

  JukeBox::GetInstance()->Init(); // commit modification on sound options

  //Save options in XML
  config->Save();

  //Team editor
  if (!GameIsRunning() && TeamInfoValid()) {
    if (!lbox_teams->IsItemSelected()) {
      AddTeam();
    }
    SaveTeam();
  }
}

bool OptionMenu::signal_ok()
{
  SaveOptions();
  CheckUpdates();
  return true;
}

bool OptionMenu::signal_cancel()
{
  Config::GetInstance()->SetVolumeMusic(initial_vol_mus);
  Config::GetInstance()->SetVolumeEffects(initial_vol_eff);
  return true;
}

void OptionMenu::CheckUpdates()
{
#if USE_MISC_TAB
  if (!Config::GetInstance()->GetCheckUpdates()
      || Constants::WARMUX_VERSION.find("svn") != std::string::npos)
    return;

  Downloader *dl = Downloader::GetInstance();
  std::string latest_version;
  if (dl->GetLatestVersion(latest_version)) {
      const char  *cur_version   = Constants::GetInstance()->WARMUX_VERSION.c_str();
      if (latest_version != cur_version) {
        Question new_version;
        std::string txt = Format(_("A new version %s is available, while your version is %s. "
                                   "You may want to check whether an update is available for your OS!"),
                                 latest_version.c_str(), cur_version);
        new_version.Set(txt, true, 0);
        new_version.Ask();
      }
  } else {
    AppWarmux::DisplayError(Format(_("Version verification failed because: %s"),
                                   dl->GetLastError().c_str()));
  }
#endif
}

uint OptionMenu::toVolume(uint level)
{
  return (level * Config::GetMaxVolume() + 50) / 100;
}

uint OptionMenu::fromVolume(uint vol)
{
  uint max = Config::GetMaxVolume();
  return (vol*100 + max/2) / max;
}


// Team editor function
bool OptionMenu::TeamInfoValid()
{
  if (tbox_team_name->GetText().empty())
    return false;

  for (uint i = 0; i < tbox_character_name_list.size(); i++) {
    if (tbox_character_name_list[i]->GetText().empty()) {
      return false;
    }
  }

  return true;
}

void OptionMenu::AddTeam()
{
  if (GameIsRunning())
    return;

  if (!TeamInfoValid()) {
    Question question(Question::NO_TYPE);
    question.Set(_("You must fill Head Commander and all player names"), true, 0);
    question.Ask();
    return;
  }

  CustomTeam *new_team = new CustomTeam(tbox_team_name->GetText());

  selected_team = new_team;
  SaveTeam();

  Question question(Question::NO_TYPE);
  question.Set(_("Team saved"), true, 0);
  question.Ask();

  ReloadTeamList();
  lbox_teams->NeedRedrawing();
  widgets.Pack();
}

void OptionMenu::DeleteTeam()
{
  if (GameIsRunning())
    return;

  if (selected_team) {
    selected_team->Delete();
    selected_team = NULL;
    if (lbox_teams->IsItemSelected()) {
      lbox_teams->Deselect();
    }
    Question question(Question::NO_TYPE);
    question.Set(_("Team deleted"), true, 0);
    question.Ask();
    ReloadTeamList();
    LoadTeam();
    lbox_teams->NeedRedrawing();
    widgets.Pack();
  }
}

void OptionMenu::LoadTeam()
{
  if (GameIsRunning())
    return;

  if (selected_team) {
    tbox_team_name->SetText(selected_team->GetName());
    std::vector<std::string> character_names = selected_team->GetCharactersNameList();

    for (uint i = 0; i < character_names.size() && i < tbox_character_name_list.size(); i++) {
      tbox_character_name_list[i]->SetText(character_names[i]);
    }

  } else {
    tbox_team_name->SetText("");

    for (uint i = 0; i < tbox_character_name_list.size(); i++) {
      tbox_character_name_list[i]->SetText("");
    }
  }
}

void OptionMenu::ReloadTeamList()
{
  if (GameIsRunning())
    return;

  lbox_teams->Clear();
  std::string selected_team_name ="";
  if (selected_team) {
    selected_team_name = selected_team->GetName();
  }

  GetCustomTeamsList().LoadList();
  std::vector<CustomTeam *> custom_team_list = GetCustomTeamsList().GetList();

  for (uint i = 0; i < custom_team_list.size(); i++) {
    if (custom_team_list[i]->GetName() == selected_team_name) {
      selected_team = custom_team_list[i];
      LoadTeam();
    }

    lbox_teams->AddLabelItem((selected_team == custom_team_list[i]),
                             custom_team_list[i]->GetName(),
                             custom_team_list[i]->GetName().c_str());

  }
}

bool OptionMenu::SaveTeam()
{
  if (GameIsRunning())
    return false;

  if (!TeamInfoValid())
    return false;

  if (selected_team) {
    bool is_name_changed = (selected_team->GetName().compare(tbox_team_name->GetText()) != 0);
    selected_team->SetName(tbox_team_name->GetText());
    for (uint i = 0; i < tbox_character_name_list.size(); i++) {
      selected_team->SetCharacterName(i,tbox_character_name_list[i]->GetText());
    }
    selected_team->Save();
    return is_name_changed;
  }

  return false;
}

void OptionMenu::SelectTeam()
{
  if (GameIsRunning())
    return;

  if (lbox_teams->IsItemSelected()) {
    bool is_changed_name = SaveTeam();
    const std::string& s_selected_team = (const char*)lbox_teams->GetSelectedValue();
    selected_team = GetCustomTeamsList().GetByName(s_selected_team);
    LoadTeam();
    if (is_changed_name) {
      ReloadTeamList();
    }
  }
}

#ifdef ENABLE_NLS
void OptionMenu::AddLanguageItem(const char* label, const char* value)
{
  lbox_languages->AddItem(Config::GetConstInstance()->GetLanguage() == value,
                          new Label(label, 400, Font::FONT_MEDIUM,
                                    Font::FONT_BOLD, white_color, Text::ALIGN_LEFT_TOP, true),
                          value);
}
#endif
