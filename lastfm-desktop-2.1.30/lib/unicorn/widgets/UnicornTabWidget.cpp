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

#include <QMouseEvent>
#include <QPainter>
#include <QTabBar>
#include <QHBoxLayout>
#include <QApplication>

#include "UnicornTabWidget.h"

const int unicorn::TabBar::k_startTearDistance = 30;

unicorn::TabBar::TabBar()
        :m_spacing( 0 ),
         m_leftMargin( 5 ),
         m_active( ":/DockWindow/tab/active.png" )
{
#ifndef WIN32
    QFont f = font();
    f.setPixelSize( 10 );
    f.setBold( true );
    setFont( f );
#endif

    QPalette p = palette();
    
    QLinearGradient window( 0, 0, 0, sizeHint().height());
    window.setColorAt( 0, 0x3c3939 );
    window.setColorAt( 1, 0x282727 );
    p.setBrush( QPalette::Window, window );
    
    QPixmap pm = m_active.copy( (m_active.width() / 2)-1, 0, 2, m_active.height());
    p.setBrush( QPalette::Button, pm );
    
    QLinearGradient buttonHighlight( 0, 0, 0, sizeHint().height() - 14 );
    buttonHighlight.setColorAt( 0, Qt::black );
    buttonHighlight.setColorAt( 1, 0x474243 );
    p.setBrush( QPalette::Midlight, buttonHighlight );
    
    p.setColor( QPalette::Active, QPalette::Text, 0x848383 );
    p.setColor( QPalette::Inactive, QPalette::Text, 0x848383 );
    setPalette( p );
    
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    setFixedHeight( 33 );
    setMinimumHeight( 33 );
    
    new QHBoxLayout( this );
    layout()->setContentsMargins( 0, 0, 5, 0 );
    ((QHBoxLayout*)layout())->addStretch( 1 );
   
    setAutoFillBackground( true );
}


QSize
unicorn::TabBar::sizeHint() const
{
    return QSize( minimumWidth(), 33 );
}


void
unicorn::TabBar::mousePressEvent( QMouseEvent* e )
{
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    
    m_mouseDownPos = e->pos();
    
    int w = (minimumWidth() - layout()->minimumSize().width() - m_leftMargin) / count();
    
    int index = ( (e->pos().x() - m_leftMargin ) / (w + m_spacing ));

    if( index < count() )
        setCurrentIndex( index );
}


void 
unicorn::TabBar::mouseReleaseEvent( QMouseEvent* )
{
    m_mouseDownPos = QPoint();
}

void 
unicorn::TabBar::mouseMoveEvent( QMouseEvent* e )
{
    if( !m_tearable )
        return;
        
    if( !(e->buttons() & Qt::LeftButton) || m_mouseDownPos.isNull() )
        return;
        
    if( (e->pos() - m_mouseDownPos ).manhattanLength() < 
        k_startTearDistance)
        return;
        
    TabWidget* tabWidget = qobject_cast<TabWidget*>( parentWidget() );
    if( !tabWidget )
        return;
        
    int index = currentIndex();
    removeTab( index );

    QWidget* curWidget = tabWidget->widget( index );
    if( !curWidget )
        return;
        
    QPoint offset = curWidget->mapToGlobal(curWidget->pos()) - QCursor::pos();
    curWidget->setParent( window(), Qt::Tool );
    curWidget->move( QCursor::pos() + QPoint(offset.x(), 0));
    curWidget->resize( tabWidget->size());
    curWidget->show();
    m_tearable = false;
    while( QApplication::mouseButtons() & Qt::LeftButton )
    {
        if( curWidget->pos() != QCursor::pos())
            curWidget->move( QCursor::pos() + QPoint(offset.x(), 0));
        QApplication::processEvents( QEventLoop::WaitForMoreEvents );
    }
    curWidget->installEventFilter( this );
    m_tearable = true;
    m_mouseDownPos = QPoint();
}


void
unicorn::TabBar::tabInserted( int )
{
    int w = 0;
    for (int i = 0; i < count(); ++i)
        w = qMax( fontMetrics().width( tabText( i ) ), w );
    setMinimumWidth( (m_leftMargin + (count() * ( w + 20 ))) + 10 + layout()->minimumSize().width());
}


void
unicorn::TabBar::tabRemoved( int i )
{
    tabInserted( i );
}


void 
unicorn::TabBar::addWidget( QWidget* wi )
{
    layout()->addWidget( wi );
    int w = 0;
    for (int i = 0; i < count(); ++i)
        w = qMax( fontMetrics().width( tabText( i ) ), w );
    setMinimumWidth(  (m_leftMargin + (count() * ( w + 20 ))) + 10 + layout()->minimumSize().width());
}


void
unicorn::TabBar::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    
    p.setClipRect( e->rect());
    if( count() <= 0 )
        return;
        
    int w = (minimumWidth() - layout()->minimumSize().width() - m_leftMargin) / count();
    for (int i = 0; i < count(); ++i)
    {
        int const x = m_leftMargin + (i * ( w + m_spacing ));
        
        if (i == count() - 1)
            w += (minimumWidth() - layout()->minimumSize().width() - m_leftMargin + 10) % w;
        
        if (currentIndex() == i)
        {
            p.setBrush( palette().brush( QPalette::Button ) );
            p.drawPixmap( x, 7, 8, m_active.height(), m_active, 0, 0, 8, m_active.height() );
            p.drawPixmap( x + 8, 7, w -16, m_active.height(), palette().brush( QPalette::Button ).texture());
            p.drawPixmap( x + w - 8, 7, 9, m_active.height(), m_active, m_active.width() / 2, 0, 9, m_active.height() );
            p.setPen( palette().color( QPalette::Active, QPalette::Text ) );
        }
        else
        {
            p.setPen( palette().color( QPalette::Inactive, QPalette::Text ) );
        }
                
        p.drawText( x, -1, w, height(), Qt::AlignCenter, tabText( i ) );
    }
    
    const int h = height() - 1;
    p.setPen( QPen( Qt::black, 0 ) );
    p.setRenderHint( QPainter::Antialiasing, false );
    p.drawLine( 0, h, width(), h );   
}


void 
unicorn::TabBar::setSpacing( int spacing )
{
    m_spacing = spacing;
}


bool 
unicorn::TabBar::eventFilter( QObject* o, QEvent* e )
{
    if( e->type() != QEvent::Close )
        return false;
    
    QWidget* w;
    if( !( w = qobject_cast< QWidget* >( o )))
        return false;
        
    if( w->windowTitle().isEmpty())
        return false;
        
    ((TabWidget*)parentWidget())->addTab( w );
    w->removeEventFilter( this );
    return true;
}



unicorn::TabWidget::TabWidget()
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( m_bar = new TabBar );
//    m_bar->setSpacing( 3 );
    v->addWidget( m_stack = new QStackedWidget );
    v->setSpacing( 0 );
    v->setMargin( 0 );
    
    setAutoFillBackground( true );
    
    QPalette p = palette();
    p.setBrush( QPalette::Window, QBrush( 0x0e0e0e ) );
    setPalette( p );
    
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
    
    connect( m_bar, SIGNAL(currentChanged( int )), m_stack, SLOT(setCurrentIndex( int )) );
    connect( m_bar, SIGNAL(currentChanged( int )), SIGNAL(currentChanged(int)));
}


void
unicorn::TabWidget::addTab( const QString& title, QWidget* w )
{
    m_bar->addTab( title );
    setMinimumWidth( m_bar->minimumWidth());
    m_stack->addWidget( w );
    w->setAttribute( Qt::WA_MacShowFocusRect, false );
}


void 
unicorn::TabWidget::addTab( QWidget* w )
{
    Q_ASSERT( !w->windowTitle().isEmpty());
    addTab( w->windowTitle(), w );
}


void 
unicorn::TabWidget::setTabEnabled( int index, bool b )
{
    m_bar->setTabEnabled( index, b );
}


QWidget* 
unicorn::TabWidget::widget( int index ) const
{
    return m_stack->widget( index );
}
