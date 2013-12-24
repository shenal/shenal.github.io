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

#ifndef SCROBBLE_CONTROLS_H
#define SCROBBLE_CONTROLS_H

#include <QFrame>

#include <lastfm/Track.h>
#include <lastfm/XmlQuery.h>

namespace unicorn{ class Session; }
namespace lastfm{ class User; }

class QPushButton;

class ScrobbleControls : public QFrame
{
    Q_OBJECT
public:
    struct {
        QPushButton* love;
        QPushButton* tag;
        QPushButton* share;
        QPushButton* buy;
    } ui;

public:
    ScrobbleControls( QWidget* parent = 0 );

    void setTrack( const Track& track );

public slots:
    void setLoveChecked( bool checked );

private slots:
    void onLoveChanged( bool checked );

    void onShareLastFm();
    void onShareTwitter();
    void onShareFacebook();

    void onTag();

private:
    Track m_track;
};

#endif //SCROBBLE_CONTROLS_H
