#ifndef WINAMP_PLUGIN_INFO_H_
#define WINAMP_PLUGIN_INFO_H_

#include "../Plugins/IPluginInfo.h"

class WinampPluginInfo : public IPluginInfo
{
    Q_OBJECT
public:
    WinampPluginInfo( QObject* parent = 0 );

    Version version() const;
    QString name() const;
    
    QString displayName() const;
    QString processName() const;

    QString id() const;
    BootstrapType bootstrapType() const;

    bool isAppInstalled() const;
    QString pluginInstallPath() const;
    QString pluginInstaller() const;
};

#endif //WINAMP_PLUGIN_INFO_H_

