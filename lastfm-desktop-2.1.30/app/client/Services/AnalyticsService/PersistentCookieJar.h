/*
   Copyright 2005-2012 Last.fm Ltd.
      - Primarily authored by Frantz Joseph

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
#pragma once

#include <QNetworkCookieJar>

class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    PersistentCookieJar(QObject *parent);
    ~PersistentCookieJar();

    void load();

private:
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

public slots:
    void save();
};
