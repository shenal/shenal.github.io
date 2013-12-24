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
#ifndef PLAYLIST_MODEL_H_
#define PLAYLIST_MODEL_H_

#include <QAbstractItemModel>
#include <types/Track.h>

class PlaylistModel : public QAbstractItemModel
{
Q_OBJECT
public:
    enum ItemDataRole{ UrlRole = Qt::UserRole };

    PlaylistModel( QObject* p = 0 );
    virtual int columnCount( const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;

    virtual QVariant headerData( int section, Qt::Orientation, int role = Qt::DisplayRole ) const;
    virtual QVariant data( const QModelIndex& index, int role ) const;

    virtual QModelIndex index( int row, int column, const QModelIndex& p = QModelIndex()) const;

    virtual QModelIndex parent( const QModelIndex& ) const
    { return QModelIndex(); }


public slots:
    void addTracks( QList< Track > tracks );
    void addTrack( Track t );
    void clear();


private:
    QList< Track > m_tracks;
};

#endif //PLAYLIST_MODEL_H_
