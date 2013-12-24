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
#ifndef ITUNESCOMTHREAD_H
#define ITUNESCOMTHREAD_H

#ifdef WIN32

#include "ITunesTrack.h"
#include "ITunesEventInterface.h"

#include <windows.h>

#include <vector>
#include <string>

/// Used to pass the track info we get from the visual plugin in one chunk
struct VisualPluginTrack
{
    std::wstring artist;
	std::wstring albumArtist;
    std::wstring track;
    std::wstring album;
    std::wstring path;
    unsigned long playCount;
};


/** @author Erik Jalevik <erik@last.fm>
  * @brief Sets up a thread which internally uses the ITunesComWrapper.
  *        This is because we can't make COM calls on the iTunes main thread
  *        whilst inside an iTunes plugin. It also acts as the event sink
  *        for any events iTunes emits.
  */
class ITunesComThread : public ITunesEventInterface
{
public:
    ITunesComThread();
    ~ITunesComThread();

	void callbackIfStopped( void (*callback)() );

    // But because the COM interface has a bug which delivers wonky events
    // when looping a track, we use this function to sync instead, which is
    // driven by the visualizer plugin stop/start events.
    void syncTrack( const VisualPluginTrack& vpt );
	void stop();

private:
	// These are the COM event handlers
    virtual void
    onDatabaseChanged( std::vector<ITunesEventInterface::ITunesIdSet> deletedObjects,
                       std::vector<ITunesEventInterface::ITunesIdSet> changedObjects );
    virtual void onPlay( ITunesTrack );
    virtual void onStop( ITunesTrack );
    virtual void onTrackChanged( ITunesTrack );
    virtual void onAboutToPromptUserToQuit();
    virtual void onComCallsDisabled();
    virtual void onComCallsEnabled();

	void startComThread();
    
    static unsigned __stdcall
    threadMain( LPVOID p )
    {
        return reinterpret_cast<ITunesComThread*>(p)->eventLoop();
    }

    /// Eternal event loop for handling action messages from other threads
    unsigned eventLoop();

    /// We need a window with a WndProc for the iTunes event sink to work
    bool setupWndProc();
    static LRESULT CALLBACK wndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    void syncComTrackInThread( const ExtendedITunesTrack& track );
    void syncVisTrackInThread();
	bool isPlaying();
	static VOID CALLBACK isPlayingCallback( HWND hwnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult );

    /// Syncs tracks that were put on the queue while COM was disabled
    void syncQueue();
	void doStop();

    class ITunesComWrapper* m_com;
    class ITunesPlaysDatabase* m_db;
    
    ExtendedITunesTrack m_lastTrack;
    bool m_comEnabled;

    DWORD m_timeOfLastTrackChange;
    bool m_retrying;
    int m_retryAttempts;

    CRITICAL_SECTION m_critSect;
    VisualPluginTrack m_visTrackToSync;
    std::vector<VisualPluginTrack> m_visTrackQueue;

    HANDLE m_threadHandle;
    HWND m_hWnd;
};

#endif // WIN32

#endif // ITUNESCOMTHREAD_H