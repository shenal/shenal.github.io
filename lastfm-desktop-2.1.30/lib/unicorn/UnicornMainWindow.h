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
#ifndef UNICORN_MAIN_WINDOW
#define UNICORN_MAIN_WINDOW


#include "lib/DllExportMacro.h"
#include <QMainWindow>
#include <QPointer>
#include <QMenuBar>
#include <QDialog>
#include <QMap>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <shobjidl.h>
#endif

class AboutDialog;
class UpdateDialog;
class QNetworkReply;


template <typename D> struct OneDialogPointer : public QPointer<D>
{    
    OneDialogPointer& operator=( QDialog* d )
    {
        QPointer<D>::operator=( (D*)d );
    	d->setAttribute( Qt::WA_DeleteOnClose ); 
    	d->setWindowFlags( Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint ); 
    	d->setModal( false );
        return *this;
    }
    
    void show()
    {
        QDialog* d = (QDialog*)QPointer<D>::data();
        d->show();
        d->raise();
        d->activateWindow();
    }
};

namespace lastfm { class User; }

namespace unicorn
{
    class Session;

    class UNICORN_DLLEXPORT MainWindow : public ::QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow( QMenuBar*, QWidget* parent = 0 );
        ~MainWindow();

        /** call this to add the account menu and about menu action, etc. */
        void finishUi();
        void addDragHandleWidget( QWidget* );

        QMenuBar* appMenuBar() const { return m_menuBar; }

    public slots:
        void openLog();
        void refreshStyleSheet();

    private:
        void storeGeometry() const;

        virtual bool eventFilter( QObject*, QEvent* );
        virtual void hideEvent( QHideEvent* );
        virtual void showEvent( QShowEvent* );
        virtual void moveEvent( QMoveEvent* );
        virtual void resizeEvent( QResizeEvent* );

        virtual void addWinThumbBarButtons( QList<QAction*>& ) {;}

        QMenuBar* menuBar() const { return 0; }

    protected:
        QPointer<QMenuBar> m_menuBar;

    private:
        QList<QAction*> m_thumbButtonActions;

#ifdef Q_OS_WIN32
        bool canUseTaskBar;
        UINT taskBarCreatedMessage;
        ITaskbarList3* taskbar;

    private:
        bool winEvent(MSG* message, long* result);
#endif

        QMap<QWidget*, QPoint> m_dragHandleMouseDownPos;

    private slots:
        void cleverlyPosition();
#ifdef Q_OS_WIN32
        void updateThumbButtons();
#endif
    
    signals:
        void hidden( bool );   
        void shown( bool );
    };
}

#endif
