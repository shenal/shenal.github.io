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
#include "ScrobSocket.h"
#include <QThread>
#include <QHostAddress>
#include <QUrl>
#include <QTextStream>

static int const kDefaultPort = 33367;
 

ScrobSocket::ScrobSocket( QObject* parent ) : QTcpSocket( parent )
{
    connect( this, SIGNAL(readyRead()), SLOT(onReadyRead()) );    
    connect( this, SIGNAL(error( QAbstractSocket::SocketError )), SLOT(onError( QAbstractSocket::SocketError )) );
    connect( this, SIGNAL(connected()), SLOT(onConnected()) );
    connect( this, SIGNAL(disconnected()), SLOT(onDisconnected()) );
    transmit( "INIT c=bof\n" );
}


ScrobSocket::~ScrobSocket()
{
    if (!m_track.isNull()) stop();
}


void
ScrobSocket::transmit( const QString& data )
{
    m_msgQueue.enqueue( data );
    qDebug() << "Connection state == " << state();
    if( state() == QAbstractSocket::UnconnectedState )
        connectToHost( QHostAddress::LocalHost, kDefaultPort );
}


void 
ScrobSocket::onConnected()
{
    if( !m_msgQueue.empty() )
    {
        qDebug() << m_msgQueue.head().trimmed();
        write( m_msgQueue.takeFirst().toUtf8());
    }
}


void 
ScrobSocket::onDisconnected()
{
    if( !m_msgQueue.empty())
        connectToHost( QHostAddress::LocalHost, kDefaultPort );
}


void
ScrobSocket::onError( SocketError error )
{
    switch (error)
    {
        case SocketTimeoutError:
            // TODO look, really we should store at least one start message forever
            // then if last time we didn't connect and this time it's a pause we 
            // send the start first
            m_msgQueue.clear();
            break;
        
        case RemoteHostClosedError:
            // expected
            break;
        
        default: // may as well
            qDebug() << lastfm::qMetaEnumString<QAbstractSocket>( error, "SocketError" );
        case ConnectionRefusedError: // happens if client isn't running
            break;
    }
}


static inline QString encodeAmp( QString data )
{ 
    return data.replace( '&', "&&" );
}


void
ScrobSocket::start( const Track& t )
{
    m_track = t;
    transmit( "START c=bof" "&"
                    "a=" + encodeAmp( t.artist() ) + "&"
                    "t=" + encodeAmp( t.title() ) + "&"
                    "b=" + encodeAmp( t.album() ) + "&"
                    "l=" + QString::number( t.duration() ) + "&"
                    "p=" + encodeAmp( t.url().path() ) + '\n' );
}


void
ScrobSocket::pause()
{
    transmit( "PAUSE c=bof\n" );
}


void
ScrobSocket::resume()
{
    transmit( "RESUME c=bof\n" );
}


void
ScrobSocket::stop()
{
    transmit( "STOP c=bof\n" );
}


void
ScrobSocket::onReadyRead()
{
    QByteArray bytes = readAll();
    if (bytes != "OK\n") qWarning() << bytes.trimmed();
    disconnectFromHost();
}
