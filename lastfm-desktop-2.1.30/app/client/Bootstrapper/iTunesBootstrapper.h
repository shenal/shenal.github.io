/***************************************************************************
*   Copyright (C) 2005 - 2007 by                                          *
*      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
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

#ifndef ITUNES_BOOTSTRAPPER_H
#define ITUNES_BOOTSTRAPPER_H

#include "AbstractFileBootstrapper.h"

/** 
  * @author Jono Cole <jono@last.fm>
  * @brief Bootstrap using information from the iTunes Music Library.xml
  * file.
  */
class iTunesBootstrapper : public AbstractFileBootstrapper
{
    Q_OBJECT

public:
    iTunesBootstrapper( QObject* parent = NULL );
    void bootStrap();

signals:
    void trackProcessed( int, Track );

private:
    class ITunesDevice* m_iTunesDatabase;
};

#endif
