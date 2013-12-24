/***************************************************************************
 *   Copyright 2005 - 2008 Last.fm Ltd.                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Erik Jaelevik <erik@last.fm>
  * @contributor Max Howell <max@last.fm>
  */
#ifndef ITUNES_DEVICE_H
#define ITUNES_DEVICE_H

#include "MediaDeviceInterface.h"
#include <lastfm/Track.h>

/** 
  * @brief The ITunesDevice class parses the iTunes Music Library.xml file
  * and allows iterating over the Track information.
  *
  * @todo Rename this to something more descriptive.
  */
class ITunesDevice : public QObject
{
    Q_OBJECT

public:
    ITunesDevice();

    QString LibraryPath();

    Track firstTrack( const QString& file );
    Track nextTrack();

signals:
    void progress( int percentage, const Track& track );

private:
    QString m_iTunesLibraryPath;
    QString m_database;
    int m_totalSize;

    class QFile* m_file;
    class ITunesParser* m_handler;
    class QXmlSimpleReader* m_xmlReader;
    class QXmlInputSource* m_xmlInput;
};

#endif //ITUNES_DEVICE_H
