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

#include <AppKit/NSWorkspace.h>

#include <lastfm/Track.h>
#include <lastfm/misc.h>

#include "lib/unicorn/mac/AppleScript.h"

#include "../PlayerConnection.h"
#include "SpotifyListener.h"

struct SpotifyConnection : public PlayerConnection
{
    SpotifyConnection() : PlayerConnection( "spt", "Spotify" )
    {}

    void start( const Track& t )
    {
        MutableTrack mt( t );
        mt.setSource( Track::Player );
        mt.setExtra( "playerId", this->id() );
        mt.setExtra( "playerName", name() );
        mt.stamp();
        handleCommand( CommandStart, t );
    }

    void pause() { handleCommand( CommandPause ); }
    void resume() { handleCommand( CommandResume ); }
    void stop() { handleCommand( CommandStop ); }
};


SpotifyListenerMac::SpotifyListenerMac( QObject* parent )
    :QObject( parent )
{
    if ( [[NSWorkspace sharedWorkspace] absolutePathForAppBundleWithIdentifier:@"com.spotify.client"] != nil )
    {
        QTimer* timer = new QTimer( this );
        timer->start( 1000 );
        connect( timer, SIGNAL(timeout()), SLOT(loop()));
    }
}

SpotifyListenerMac::~SpotifyListenerMac()
{
    delete m_connection;
}

void
SpotifyListenerMac::loop()
{
    static AppleScript playerStateScript( "tell application id \"com.spotify.client\" to if running then return player state" );
    QString playerState = playerStateScript.exec();

    if ( playerState == "playing"
         || playerState == "stopped"
         || playerState == "paused" )
    {
        if ( !m_connection )
            emit newConnection( m_connection = new SpotifyConnection );

        static AppleScript titleScript( "tell application id \"com.spotify.client\" to return name of current track" );
        static AppleScript albumScript( "tell application id \"com.spotify.client\" to return album of current track" );
        static AppleScript artistScript( "tell application id \"com.spotify.client\" to return artist of current track" );
        static AppleScript albumArtistScript( "tell application id \"com.spotify.client\" to return album artist of current track" );
        static AppleScript durationScript( "tell application id \"com.spotify.client\" to return duration of current track" );

        if ( playerState == "playing" )
        {
            lastfm::MutableTrack t;
            t.setTitle( titleScript.exec() );
            t.setAlbum( albumScript.exec() );
            t.setArtist( artistScript.exec() );
            t.setAlbumArtist( albumArtistScript.exec() );
            t.setDuration( durationScript.exec().toInt() );

            if ( t != m_lastTrack )
            {
                if ( t.album().title().startsWith( "http://" )
                     || t.album().title().startsWith( "https://" )
                     || t.album().title().startsWith( "spotify:" ) )
                {
                    // this is an advert so don't try to display metadata
                    // and stop any currently playing track
                    if ( m_lastPlayerState == "playing" || m_lastPlayerState == "paused" )
                        m_connection->stop();
                }
                else
                {
                    m_connection->start( t );
                }
            }
            else if ( m_lastPlayerState == "paused" )
                m_connection->resume();

            m_lastTrack = t;
        }
        else if ( m_lastPlayerState != playerState )
        {
            if ( playerState == "stopped" )
            {
                m_connection->stop();
                m_lastTrack = Track();
            }
            else if ( playerState == "paused" )
                m_connection->pause();
        }
    }
    else
    {
        if ( m_lastPlayerState == "playing" || m_lastPlayerState == "paused" )
            m_connection->stop();

        delete m_connection;
    }

    m_lastPlayerState = playerState;
}


