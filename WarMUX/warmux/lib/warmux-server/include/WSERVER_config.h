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
 ******************************************************************************/

#ifndef WSERVER_CONFIG_H
#define WSERVER_CONFIG_H
#include <list>
#include <map>
#include <string>

class ServerConfig
{
  bool support_versions;
  std::string config_file;

  std::map<std::string, std::string> str_value;
  std::map<std::string, int> int_value;
  std::map<std::string, bool> bool_value;
  std::list<std::string> supported_versions;
  std::list<std::string> hidden_supported_versions;

  static void SplitVersionsString(const std::string& all_versions, std::list<std::string>& versions_lst);
  static const std::string SupportedVersions2Str(const std::list<std::string>& versions_lst);

  void Parse(std::ifstream & fin);
  void LoadConfigFile();

protected:
  void SetDefault(const std::string & name, const std::string & value);
  void SetDefault(const std::string & name, const int & value);
  void SetDefault(const std::string & name, const bool & value);

  /* must be called only once */
  virtual void Load(const std::string & config_file);

  void Display() const;

public:
  ServerConfig(bool support_versions);

  bool Get(const std::string & name, std::string & value) const;
  bool Get(const std::string & name, int & value) const;
  bool Get(const std::string & name, bool & value) const;

  // Return if a version is supported
  // a version can be accepted while not in the supported versions string
  bool IsVersionSupported(const std::string & version) const;

  // Return as string the list of officially supported versions
  // (development versions are hidden)
  const std::string SupportedVersions2Str() const;

  // To squash previous configuration with the new one from the file
  void Reload();
};

extern bool WSERVER_Verbose;

#endif
