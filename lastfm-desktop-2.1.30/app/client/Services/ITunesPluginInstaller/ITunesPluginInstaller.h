/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd <client@last.fm>                      *
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

#ifndef ITUNES_PLUGIN_INSTALLER_H
#define ITUNES_PLUGIN_INSTALLER_H

#include <QApplication>
#include <QString>


class ITunesPluginInstaller : public QObject
{
    Q_OBJECT
public:
    ITunesPluginInstaller( QObject* parent = 0 );
    
    void uninstall();

    // NOTE this is only valid after calling install()
    bool needsTwiddlyBootstrap() const { return m_needsTwiddlyBootstrap; }

public slots:
    void install();

private:
    bool isPluginInstalled();
    QString pListVersion( const QString& file );

    bool removeInstalledPlugin();
    bool installPlugin();

    // Legacy code: removes old LastFmHelper for updates
    void disableLegacyHelperApp();

    QString const k_shippedPluginDir;
    QString const k_iTunesPluginDir;
    bool m_needsTwiddlyBootstrap;
};

#endif
