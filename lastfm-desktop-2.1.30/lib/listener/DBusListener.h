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
#include <QtGlobal>
#ifdef QT_DBUS_LIB

#include "lib/DllExportMacro.h"
#include <QMap>
#include <QObject>
class PlayerConnection;


/** listens for dbus connections */
class LISTENER_DLLEXPORT DBusListener : public QObject
{
    Q_OBJECT

public:
    DBusListener( QObject* parent );
    
signals:
    void newConnection( class PlayerConnection* );
    
private slots:
    void start( const QString& id ) {}
    void pause( const QString& id ) {}
    void resume( const QString& id ) {}
    void stop( const QString& id ) {}

private:
    QMap<QString, PlayerConnection*> m_connections;
};

#endif
