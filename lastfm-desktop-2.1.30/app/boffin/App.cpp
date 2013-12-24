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
#include <boost/bind.hpp>
#include <QFileDialog>
#include <QMenu>
#include <QLabel>
#include <QTimer>
#include <QShortcut>
#include <QComboBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include <lastfm/NetworkAccessManager>
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "App.h"
#include "MainWindow.h"
#include "MediaPipeline.h"
#include "PickDirsDialog.h"
#include "ScanProgressWidget.h"
#include "ScrobSocket.h"
#include "TrackSource.h"
#include "Shuffler.h"
#include "LocalCollectionScanner.h"
#include "playdar/PlaydarConnection.h"
#include "PlaydarTagCloudModel.h"
#include "Playlist.h"
#include "XspfDialog.h"


#define OUTPUT_DEVICE_KEY "OutputDevice"
#define PLAYDAR_AUTHTOKEN_KEY "PlaydarAuth"
#define PLAYDAR_URLBASE_KEY "PlaydarUrlBase"

App::App( int& argc, char** argv )
   : unicorn::Application( argc, argv )
   , m_mainwindow( 0 )
   , m_tagcloud( 0 )
   , m_scrobsocket( 0 )
   , m_pipe( 0 )
   , m_playlist( 0 )
   , m_audioOutput( 0 )
   , m_playing( false )
   , m_req( 0 )
   , m_api( 
        unicorn::UserSettings().value(PLAYDAR_URLBASE_KEY, "http://localhost:8888").toString(), 
        unicorn::UserSettings().value(PLAYDAR_AUTHTOKEN_KEY, "").toString() )
{
    m_wam = new lastfm::NetworkAccessManager( this );
    m_playdar = new PlaydarConnection(m_wam, m_api);
    connect(m_playdar, SIGNAL(authed(QString)), SLOT(onPlaydarAuth(QString)));

    m_shuffler = new Shuffler(this);
    m_tracksource = new TrackSource(m_shuffler, this);
}


App::~App()
{
    cleanup();
    if (m_audioOutput) QSettings().setValue( OUTPUT_DEVICE_KEY, m_audioOutput->outputDevice().name() );
    delete m_pipe;
}


void
App::cleanup()
{
}


void
App::init( MainWindow* window ) throw( int /*exitcode*/ )
{
    m_mainwindow = window;

    window->ui.play->setEnabled( false );
    window->ui.pause->setEnabled( false );
    window->ui.skip->setEnabled( false );
    window->ui.wordle->setEnabled( false );

////// radio
    QString const name = QSettings().value( OUTPUT_DEVICE_KEY ).toString();
    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );

    QActionGroup* actiongroup = new QActionGroup( window->ui.outputdevice );

    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
    {
        QAction* a = window->ui.outputdevice->addAction( d.name() );
        a->setCheckable( true );
        if (name == d.name())
            m_audioOutput->setOutputDevice( d );
        if (m_audioOutput->outputDevice().name() == d.name())
            a->setChecked( true );

        actiongroup->addAction( a );
    }

	m_audioOutput->setVolume( 1.0 /* Settings().volume() */ );

    connect( actiongroup, SIGNAL(triggered( QAction* )), SLOT(onOutputDeviceActionTriggered( QAction* )) );

	m_pipe = new MediaPipeline( m_audioOutput, this );
    connect( m_pipe, SIGNAL(preparing()), SLOT(onPreparing()) );
    connect( m_pipe, SIGNAL(started( Track )), SLOT(onStarted( Track )) );
    connect( m_pipe, SIGNAL(paused()), SLOT(onPaused()) );
    connect( m_pipe, SIGNAL(resumed()), SLOT(onResumed()) );
    connect( m_pipe, SIGNAL(stopped()), SLOT(onStopped()) );
    connect( m_pipe, SIGNAL(error( QString )), SLOT(onPlaybackError( QString )) );

    m_scrobsocket = new ScrobSocket( this );
    connect( m_pipe, SIGNAL(started( Track )), m_scrobsocket, SLOT(start( Track )) );
    connect( m_pipe, SIGNAL(paused()), m_scrobsocket, SLOT(pause()) );
    connect( m_pipe, SIGNAL(resumed()), m_scrobsocket, SLOT(resume()) );
    connect( m_pipe, SIGNAL(stopped()), m_scrobsocket, SLOT(stop()) );

/// parts of the scanning stuff
    //m_trackTagUpdater = TrackTagUpdater::create(
    //        "http://musiclookup.last.fm/trackresolve",
    //        100,        // number of days track tags are good
    //        5);         // 5 minute delay between web requests

/// connect
    connect( window->ui.play, SIGNAL(triggered()), SLOT(play()) );
    connect( window->ui.pause, SIGNAL(toggled( bool )), m_pipe, SLOT(setPaused( bool )) );
    connect( window->ui.skip, SIGNAL(triggered()), m_pipe, SLOT(skip()) );
    connect( window->ui.rescan, SIGNAL(triggered()), SLOT(onRescan()) );
    connect( window->ui.xspf, SIGNAL(triggered()), SLOT(xspf()) );
    connect( m_mainwindow->ui.wordle, SIGNAL( triggered()), SLOT( onWordle()));

    QShortcut* cut = new QShortcut( Qt::Key_Space, window );
    connect( cut, SIGNAL(activated()), SLOT(playPause()) );
    cut->setContext( Qt::ApplicationShortcut );

    m_mainwindow->ui.play->setEnabled( true );
    m_mainwindow->ui.pause->setEnabled( false );
    m_mainwindow->ui.skip->setEnabled( false );
    m_mainwindow->ui.rescan->setEnabled( true );
    m_mainwindow->ui.wordle->setEnabled( true );
    
    connect(m_playdar->hostsModel(), SIGNAL(modelReset()), m_mainwindow, SLOT(onSourcesReset()));
    connect(m_playdar, SIGNAL(changed(QString)), m_mainwindow->ui.playdarStatus, SLOT(setText(QString)));
    connect(m_playdar, SIGNAL(connected()), SLOT(newTagcloud()));
    m_playdar->start();
}


void
App::onOutputDeviceActionTriggered( QAction* a )
{
    //FIXME for some reason setOutputDevice just returns false! :(

    QString const name = a->text();

    foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
        if (d.name() == name) {
            qDebug() << m_audioOutput->setOutputDevice( d );
            qDebug() << m_audioOutput->outputDevice().name();
            return;
        }
}

#include "TagBrowserWidget.h"
//#include "TagCloudView.h"
//#include "TagDelegate.h"
//#include "PlaydarTagCloudModel.h"
//#include "PlaydarStatRequest.h"
//#include "PlaydarConnection.h"


void
App::onScanningFinished()
{
    if (sender())
        disconnect( sender(), 0, this, 0 ); //only once pls

}

void
App::newTagcloud()
{
    delete m_tagcloud;
    m_tagcloud = new TagBrowserWidget( m_playdar );
    connect( m_tagcloud, SIGNAL( selectionChanged()), SLOT( tagsChanged() ));
    m_mainwindow->setCentralWidget( m_tagcloud );
}

void
App::play()
{
    delete m_req;

    if (m_tagcloud) {
        m_req = m_playdar->boffinRql(m_tagcloud->rql());
        if (m_req) {
            m_shuffler->clear();
            connect(m_req, SIGNAL(playableItem(BoffinPlayableItem)), m_shuffler, SLOT(receivePlayableItem(BoffinPlayableItem)));
            connect(m_req, SIGNAL(playableItem(BoffinPlayableItem)), SLOT(onPlaydarTracksReady(BoffinPlayableItem)));
            onPreparing();
        }
    }
}

void
App::tagsChanged()
{
}


void
App::onPlaydarTracksReady( BoffinPlayableItem )
{
    // just interested in the first one
    disconnect(m_req, SIGNAL(playableItem(BoffinPlayableItem)), this, SLOT(onPlaydarTracksReady(BoffinPlayableItem)));
    // then delay for a little bit (to let a few more tracks dribble in) before playing
    QTimer::singleShot(500, this, SLOT(onReadyToPlay()));
}


void
App::onReadyToPlay()
{
    m_pipe->play(m_tracksource);
}


void
App::playPause()
{
    if (m_playing)
        m_mainwindow->ui.pause->toggle();
    else
        play();
}


void
App::xspf()
{
    QString path = QFileDialog::getOpenFileName( m_mainwindow, "Open XSPF File", "*.xspf" );
    if (path.size())
    {
        XspfDialog *pDlg = new XspfDialog(path, m_playdar, m_mainwindow);
        pDlg->show();
 //       m_mainwindow->QMainWindow::setWindowTitle( "Resolving XSPF..." );
 //       m_pipe->playXspf( path );
    }
}


void
App::onPreparing() //MediaPipeline is preparing to play a new station
{
    m_mainwindow->QMainWindow::setWindowTitle( "Boffing up..." );

    QAction* a = m_mainwindow->ui.play;
    a->setIcon( QPixmap(":/stop.png") );
    disconnect( a, SIGNAL(triggered()), this, 0 );
    connect( a, SIGNAL(triggered()), m_pipe, SLOT(stop()) );
}


void
App::onStarted( const Track& t )
{
    m_playing = true; // because phonon is shit and we can't rely on its state

    m_mainwindow->setWindowTitle( t );
    m_mainwindow->ui.play->blockSignals( true );
    m_mainwindow->ui.play->setChecked( true );
    m_mainwindow->ui.play->blockSignals( false );
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->setEnabled( true );
    m_mainwindow->ui.pause->blockSignals( false );

    m_mainwindow->ui.skip->setEnabled( true );
}


void
App::onPaused()
{
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( true );
    m_mainwindow->ui.pause->blockSignals( false );
}


void
App::onResumed()
{
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->blockSignals( false );
}


void
App::onStopped()
{
    m_mainwindow->setWindowTitle( Track() );
    m_mainwindow->ui.play->blockSignals( true );
    m_mainwindow->ui.play->setChecked( false );
    m_mainwindow->ui.play->blockSignals( false );
    m_mainwindow->ui.pause->blockSignals( true );
    m_mainwindow->ui.pause->setChecked( false );
    m_mainwindow->ui.pause->setEnabled( false );
    m_mainwindow->ui.pause->blockSignals( false );

    m_mainwindow->ui.skip->setEnabled( false );

    QAction* a = m_mainwindow->ui.play;
    a->setIcon( QPixmap(":/play.png") );
    disconnect( a, SIGNAL(triggered()), m_pipe, 0 );
    connect( a, SIGNAL(triggered()), SLOT(play()) );

    m_playing = false;
}


void
App::onPlaybackError( const QString& msg )
{
    //TODO: need to make this more like client 2's subtle yellow box.
    QMessageBoxBuilder( m_mainwindow )
            .setTitle( "Playback Error" )
            .setText( msg )
            .exec();
}


#include "WordleDialog.h"
void
App::onWordle()
{
    static OneDialogPointer<WordleDialog> d;
    if(!d) {
        d = new WordleDialog( m_mainwindow );
        QString output;
        //TagCloudModel model( this, 0 );
        //for(int i = 0; i < model.rowCount(); ++i) {
        //    QModelIndex index = model.index( i, 0 );
        //    QString weight = index.data( TagCloudModel::WeightRole ).toString();
        //    QString tag = index.data().toString().trimmed().simplified().replace( ' ', '~' );
        //    output += tag + ':' + weight + '\n';
        //}
        d->setText( output );
    }
    d.show();
}

void
App::onPlaydarAuth(const QString& auth)
{
    unicorn::UserSettings().setValue(PLAYDAR_AUTHTOKEN_KEY, auth);
}

void 
App::onRescan()
{
    PickDirsDialog* dlg = new PickDirsDialog(m_mainwindow);
    if (QDialog::Accepted == dlg->exec()) {
        QStringList directories = dlg->dirs();
        if (directories.size()) {
            LocalCollectionScanner *scanner = new LocalCollectionScanner(this);
            m_scanWidget = new ScanProgressWidget();
            connect(scanner, SIGNAL(track(Track)), m_scanWidget, SLOT(onNewTrack(Track)));
            connect(scanner, SIGNAL(directory(QString)), m_scanWidget, SLOT(onNewDirectory(QString)));
            connect(scanner, SIGNAL(finished()), m_scanWidget, SLOT(onFinished()));
            connect(scanner, SIGNAL(finished()), SLOT(newTagcloud()));
            connect(m_scanWidget, SIGNAL(statusMessage(QString)), m_mainwindow->statusBar(), SLOT(showMessage(QString)));

            // TODO: fix hard coded paths here!
            scanner->run(
                QDir("c:\\cygwin\\home\\doug\\src\\playdar\\win32\\debug\\bin\\"), 
                "c:\\cygwin\\home\\doug\\src\\playdar\\win32\\collection.db", 
                directories);

            m_mainwindow->setCentralWidget(m_scanWidget);
        }
    }
}
