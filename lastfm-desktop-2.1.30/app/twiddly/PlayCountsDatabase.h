/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#ifndef PLAY_COUNT_DATABASE_H
#define PLAY_COUNT_DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QHash>

class QSqlQuery;

class ITunesLibraryTrack;


/** @author Jono Cole <jono@last.fm>
  * @refactor Max Howell <max@last.fm>
  * @contributor Christian Muehlhaeuser <chris@last.fm>
  *
  * Derived by ManualIPodDatabase and AutomaticIPodDatabase 
  */
class PlayCountsDatabase
{
protected:
    /** pass the path for the db, you need to make sure the directory exists */
    PlayCountsDatabase( const QString& path );

public:
    virtual ~PlayCountsDatabase();

    class Track
    {
    public:
        Track() : m_playCount( 0 )
        {}

        Track( const QString& uid, int c )
        {
            m_playCount = c;
            m_id = uid;
        }

        bool isNull() const { return m_id.isEmpty(); }
        QString uniqueId() const { return m_id; }
        int playCount() const { return m_playCount; }
 
    private:
        QString m_id;
        int m_playCount;
    };

    /** the uid is path on Windows, persistentId on mac */
    Track operator[]( const QString& uid ); // gets the snapshot value
    Track track( const QString& uid ); // this actually fetchs the current value

    // NOTE never put these in the ctor/dtor, as if exception is thrown we 
    // mustn't commit the transaction!
    void beginTransaction();
    void endTransaction();

    /** the justification for INSERT is manual ipod scrobbling, since we have
      * no bootstrap step, which is unavoidable, the first diff effectively
      * bootstraps the device */
    bool insert( const ITunesLibraryTrack& track );
    bool remove( const ITunesLibraryTrack& track );
    bool update( const ITunesLibraryTrack& track );

    QString path() const { return m_path; }

protected:
  #ifdef Q_OS_WIN32
    bool exec( const QString& sql, const ITunesLibraryTrack& );
  #endif

protected:
    QSqlDatabase m_db;
    QSqlQuery* m_query;
    QHash<QString,int> m_snapshot;

private:
    Q_DISABLE_COPY( PlayCountsDatabase )

    QString m_path;
};

#endif //PLAY_COUNT_DATABASE_H
