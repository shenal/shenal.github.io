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
#ifndef DLL_EXPORT_MACRO_H
#define DLL_EXPORT_MACRO_H

/** Exports symbols when compiled as part of the lib
  * Imports when included from some other target */
#if defined(_WIN32) || defined(WIN32)
    #ifdef _UNICORN_DLLEXPORT
        #define UNICORN_DLLEXPORT __declspec(dllexport)
    #else
        #define UNICORN_DLLEXPORT __declspec(dllimport)
    #endif
    #ifdef _LISTENER_DLLEXPORT
        #define LISTENER_DLLEXPORT __declspec(dllexport)
    #else
        #define LISTENER_DLLEXPORT __declspec(dllimport)
    #endif
	#ifndef ITUNES_PLUGIN
		#ifdef _LOGGER_DLLEXPORT
			#define LOGGER_DLLEXPORT __declspec(dllexport)
		#else
			#define LOGGER_DLLEXPORT __declspec(dllimport)
		#endif
	#else
		#define LOGGER_DLLEXPORT
	#endif
#else
    #define UNICORN_DLLEXPORT
    #define LISTENER_DLLEXPORT
    #define LOGGER_DLLEXPORT
#endif

#endif
