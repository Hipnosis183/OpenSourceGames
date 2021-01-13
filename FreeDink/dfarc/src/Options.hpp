/**
 * Options dialog

 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2008  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "Options_Base.h"
#include "Config.hpp"

enum
{
    ID_WRITE_INI = 0,
    ID_PLAY_MOVIE,
    ID_LIST_ALL,

    ID_OVERRIDE_CHECK,
    ID_OVERRIDE_EDIT,
    ID_OVERRIDE_BROWSE,
    
    ID_DINK_EXE,
    ID_EDIT_EXE,

    ID_OKAY,
    ID_OP_CANCEL
};

class Options : public Options_Base
{
public:
  Options(Config* aConfig);
private:
  void prepareDialog();
  void onOkay(wxCommandEvent& aEvent);
  void onBrowseOverrideDinkrefDir(wxCommandEvent& aEvent);
  void onBrowseDModDir(wxCommandEvent& aEvent);
  void onSelectLanguageList(wxCommandEvent& aEvent);

  Config *mConfig;

  DECLARE_EVENT_TABLE()
};

#endif
