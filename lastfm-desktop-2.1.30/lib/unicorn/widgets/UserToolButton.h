/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef USER_TOOL_BUTTON_H
#define USER_TOOL_BUTTON_H

#include <QToolButton>

#include "lib/DllExportMacro.h"

namespace lastfm { class User; }
namespace unicorn { class Session; }

class UNICORN_DLLEXPORT UserToolButton : public QToolButton
{
    Q_OBJECT
public:
    UserToolButton();

protected slots:
    void onSessionChanged( const unicorn::Session& session );
    void onUserGotInfo( const lastfm::User& user );
    void onImageDownloaded();
    void onRosterUpdated();
};

#endif //USER_TOOL_BUTTON_H
