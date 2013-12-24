#include "AvatarWidget.h"

AvatarWidget::AvatarWidget( QWidget *parent ) :
    HttpImageWidget(parent)
{
    setPixmap( QPixmap( ":/user_default.png" ) );
}

void
AvatarWidget::setUser( const lastfm::User& user )
{
    m_user = user;
    update();
}


void
AvatarWidget::paintEvent( QPaintEvent* paintEvent )
{
    HttpImageWidget::paintEvent( paintEvent );

    if ( m_user.isSubscriber() )
    {
        QPainter p( this );

        QString text = tr( "Subscriber" );
        QColor brush( Qt::black );

        switch ( m_user.type() )
        {
        case User::TypeModerator:
            text = tr( "Moderator" );
            brush = QColor( 0xFFA500 );
            break;
        case User::TypeStaff:
            text = tr( "Staff" );
            brush = QColor( 0xD51007 );
            break;
        case User::TypeAlumni:
            text = m_user.gender().female() ? tr( "Alumna" ) : tr( "Alumnus" );
            brush = QColor( 0x5336BD );
            break;
        default:
            break;
        }

        QFont font = p.font();
        font.setPixelSize( 10 );
        font.setWeight( QFont::Bold );
        p.setFont( font );

        // the 8 and the 20 are from the stylesheet
        // (horrible, but there's no way to find those values out and this works)
        int totalHeight = rect().height() - 8;
        int totalWidth = rect().width() - 20;

        QFontMetrics fm( font );
        int width = fm.width( text ) + 4;
        int height = fm.height() + 4;
        QRect rect( (totalWidth / 2) - (width / 2), totalHeight - (height / 2), width, height ) ;

        p.setBrush( brush );
        p.drawRoundedRect( rect, 4, 4 );

        p.setPen( QColor( Qt::white ) );
        p.drawText( rect, Qt::AlignCenter, text );
    }
}
