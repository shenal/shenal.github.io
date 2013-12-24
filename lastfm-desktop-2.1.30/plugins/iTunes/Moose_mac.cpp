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
#define AUDIOSCROBBLER_BUNDLEID "fm.last.Scrobbler"
#include "Moose.h"
#include "common/c++/Logger.h"
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>


std::string
Moose::applicationSupport()
{
    std::string path = std::getenv( "HOME" );
    path += "/Library/Application Support/Last.fm/";
    return path;
}


static std::string
CFStringToStdString( CFStringRef s )
{
    std::string r;

    if (s == NULL)
        return r;

    CFIndex n;
    n = CFStringGetLength( s );
    n = CFStringGetMaximumSizeForEncoding( n, kCFStringEncodingUTF8 );
    char* buffer = new char[n];
    CFStringGetCString( s, buffer, n, kCFStringEncodingUTF8 );

    r = buffer;
    delete[] buffer;
    return r;
}


std::string
Moose::applicationPath()
{
    FSRef appRef;
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( AUDIOSCROBBLER_BUNDLEID ), NULL, &appRef, NULL );
    
    char path[PATH_MAX];
    FSRefMakePath( &appRef, (unsigned char*)path, PATH_MAX );
    
    if ( path == NULL )
        return "/Applications/Last.fm Scrobbler.app/Contents/MacOS/Last.fm Scrobbler";

    std::string s = path;
    s.append( "/Contents/MacOS/Last.fm Scrobbler" );
    return s;
}


bool
Moose::iPodScrobblingEnabled()
{
    Boolean key_exists;
    bool b = CFPreferencesGetAppBooleanValue( 
                    CFSTR( "iPodScrobblingEnabled"),
                    CFSTR( MOOSE_PREFS_PLIST ),
                    &key_exists );
                    
    if (!key_exists) return true;

    return b;
}


std::string
Moose::applicationFolder()
{
    std::string s = applicationPath();
    return s.substr( 0, s.rfind( '/' ) + 1 );
}


bool
Moose::launchWithMediaPlayer()
{
    return true;
    CFBooleanRef v = (CFBooleanRef) CFPreferencesCopyAppValue( 
            CFSTR( "LaunchWithMediaPlayer" ), 
            CFSTR( MOOSE_PREFS_PLIST ) );

    if (v)
    {
        bool b = CFBooleanGetValue( v );
        CFRelease( v );
        return b;
    }
    else
        return true;
}


void
Moose::setFileDescriptorsCloseOnExec()
{
    int n = 0;
    int fd = sysconf( _SC_OPEN_MAX );
    while (--fd > 2)
    {
        int flags = fcntl( fd, F_GETFD, 0 );
        if ((flags != -1) && !(flags & FD_CLOEXEC))
        {
            n++;
            flags |= FD_CLOEXEC;
            fcntl( fd, F_SETFD, flags );
        }
    }
    if (n) LOG( 3, "Set " << n << " file descriptors FD_CLOEXEC" );
}

void 
Moose::launchAudioscrobbler( const std::vector<std::string>& vargs )
{
    FSRef appRef;
    LSFindApplicationForInfo( kLSUnknownCreator, CFSTR( AUDIOSCROBBLER_BUNDLEID ), NULL, &appRef, NULL );
    
    const void* arg[vargs.size()];
    
    int index(0);

    AEDescList argAEList;
    AECreateList( NULL, 0, FALSE, &argAEList );
    
    for( std::vector<std::string>::const_iterator i = vargs.begin(); i != vargs.end(); i++ ) {
        arg[index++] = CFStringCreateWithCString( NULL, i->c_str(), kCFStringEncodingUTF8 );
        AEPutPtr( &argAEList, 0, typeChar, i->c_str(), i->length());
    }
    
    LSApplicationParameters params;
    params.version = 0;
    params.flags = kLSLaunchAndHide | kLSLaunchDontSwitch | kLSLaunchAsync;;
    params.application = &appRef;
    params.asyncLaunchRefCon = NULL;
    params.environment = NULL;
    
    CFArrayRef args = CFArrayCreate( NULL, ((const void**)arg), vargs.size(), NULL);
    params.argv = args;
    
  
    AEAddressDesc target;
    AECreateDesc( typeApplicationBundleID, CFSTR( AUDIOSCROBBLER_BUNDLEID ), 16, &target);
    
    AppleEvent event;
    AECreateAppleEvent ( kCoreEventClass,
                        kAEReopenApplication ,
                        &target,
                        kAutoGenerateReturnID,
                        kAnyTransactionID,
                        &event );
    
    AEPutParamDesc( &event, keyAEPropData, &argAEList );
    
    params.initialEvent = &event;
    
    LSOpenApplication( &params, NULL );
    AEDisposeDesc( &argAEList );
    AEDisposeDesc( &target );
}


bool
Moose::exec( const std::string& command, const std::string& args )
{
    // fixes error -54 bug, where endless dialogs are spawned
    // presumably because the child Twiddly process inherits some key
    // file descriptors from iTunes
    setFileDescriptorsCloseOnExec();
    
    std::string s = "\"" + command + "\" " + args + " &";

    LOG( 3, "Launching `" << s << "'" )
    return ( std::system( s.c_str() ) >= 0 );
}


#include "common/c++/mac/getBsdProcessList.c"


bool
Moose::isTwiddlyRunning()
{
    bool found = false;
    
    kinfo_proc* processList = NULL;
    size_t processCount = 0;

    if ( getBsdProcessList( &processList, &processCount ) )
    {
        LOG( 3, "Failed to get the process list" );
        return false;
    }

    uint const uid = ::getuid();
    for ( size_t processIndex = 0; processIndex < processCount; processIndex++ )
    {
        if ( processList[processIndex].kp_eproc.e_pcred.p_ruid == uid )
        {
            if ( strcmp( processList[processIndex].kp_proc.p_comm, "twiddly" ) == 0 )
            {
                found = true;
                break;
            }
        }
    }

    free( processList );

    if ( found ) LOG( 3, "Twiddly already running!" );

    return found;
}


std::string
Moose::bundleFolder()
{
    std::string path;
    path += ::getenv( "HOME" );
    path += "/Library/iTunes/iTunes Plug-ins/AudioScrobbler.bundle/";
    return path;
}


std::string
Moose::pluginPath()
{
    return bundleFolder() + "Contents/MacOS/AudioScrobbler";
}
