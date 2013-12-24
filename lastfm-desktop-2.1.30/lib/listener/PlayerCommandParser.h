/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#ifndef PLAYER_COMMAND_PARSER_H
#define PLAYER_COMMAND_PARSER_H

#include "common/HideStupidWarnings.h"
#include "PlayerCommand.h"
#include <lastfm/Track.h>
#include <stdexcept>

// ignore the warning about the exception specification
#ifdef Q_OS_WIN
#pragma warning( disable : 4290 )
#endif

class PlayerCommandParser
{
public:
    PlayerCommandParser( QString line ) throw( std::invalid_argument );

    PlayerCommand command() const { return m_command; }
    QString playerId() const { return m_playerId; }
    Track track() const { return m_track; }
    QString username() const { return m_username; }
	/** we use this to get a pretty name for the player, and its icon 
	  * Use the full path for the .exe file on Windows and Linux, and the bundle
	  * directory on Mac OS X */
    QString applicationPath() const { return m_applicationPath; }

    QString playerName()
    {
        QString& id = m_playerId;
        
        if (id == "osx") return "iTunes";
        if (id == "itw") return "iTunes";
        if (id == "foo") return "foobar2000";
        if (id == "wa2") return "Winamp";
        if (id == "wmp") return "Windows Media Player";
        if (id == "ass") return "Last.fm Radio";
        if (id == "bof") return "Last.fm Boffin";
        return QObject::tr( "unknown media player" );
    }    
    
private:
    PlayerCommand extractCommand( QString& line );
    QMap<QChar, QString> extractArgs( const QString& line );
    QString requiredArgs( PlayerCommand );
    Track extractTrack( const QMap<QChar, QString>& args );

    PlayerCommand m_command;
    QString m_playerId;
    Track m_track;
    QString m_username;
    QString m_applicationPath;
};

#endif // PLAYER_COMMAND_PARSER_H
