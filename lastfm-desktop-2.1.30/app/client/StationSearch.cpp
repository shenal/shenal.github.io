/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include "StationSearch.h"
#include <lastfm/ws.h>
#include <lastfm/XmlQuery.h>

#include <QDebug>
#include "Services/RadioService.h"

using namespace lastfm;

void
StationSearch::startSearch(const QString& name)
{
    if (name.length())
    {
        m_name = name.toLower();
        QMap<QString, QString> params;
        params["method"] = "radio.search";
        params["name"] = name;
        connect(lastfm::ws::get(params), SIGNAL(finished()), SLOT(onFinished()));
    }
}

void
StationSearch::onFinished()
{
    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender()) ) )
    {
        lastfm::XmlQuery station = lfm["stations"]["station"];
        RadioStation rs( QUrl::fromPercentEncoding( station["url"].text().toUtf8() ) );

        if (rs.url().length())
        {
            rs.setTitle( station["name"].text() );
            emit searchResult(rs);
            return;
        }
        else
        {
            emit error( tr("Could not start radio: %1").arg( tr( "no results for \"%1\"" ).arg( m_name ) ), "radio" );
        }
    }
    else
    {
        emit error( tr("Could not start radio: %1").arg( lfm.parseError().message() ), "radio" );
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }

    // no artist or tag result
    // maybe the user wanted to hear a friend's library?
    //lastfm::User you;
    //connect(you.getFriends(), SIGNAL(finished()), SLOT(onUserGotFriends()));
}

void
StationSearch::onUserGotFriends()
{
    sender()->deleteLater();

    lastfm::XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>( sender() ) ) )
    {
        foreach (lastfm::XmlQuery e, lfm["friends"].children("user"))
        {
            if (m_name == e["name"].text().toLower())
            {
                // friend!
                RadioStation rs = RadioStation::library(User(m_name));
                emit searchResult( rs );
                return;
            }
        }
    }
}
    
