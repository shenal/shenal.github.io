
#include <QStringListModel>
#include <QLineEdit>
#include <QKeyEvent>

#include "ShortcutEdit.h"

ShortcutEdit::ShortcutEdit( QWidget* parent )
    :QComboBox( parent )
{
    setEditable( true );
    QStringList keys;
    keys << QString::fromUtf8("⌃⌘ S")
         << "F1"
         << "F3"
         << "F2"
         << "F4"
         << "F5"
         << "F6"
         << "F7"
         << "F8"
         << "F9";
    QStringListModel* model = new QStringListModel(keys, this);
    setModel(model);
}

void
ShortcutEdit::setTextValue( QString str )
{
    setCurrentIndex( findText(str));
    lineEdit()->setText( str );
}

Qt::KeyboardModifiers
ShortcutEdit::modifiers() const
{
    return m_modifiers;
}

void
ShortcutEdit::setModifiers( Qt::KeyboardModifiers modifiers )
{
    m_modifiers = modifiers;
}


int
ShortcutEdit::key() const
{
    return m_key;
}

void
ShortcutEdit::setKey( int key )
{
    m_key = key;
}

void
ShortcutEdit::keyPressEvent( QKeyEvent* e )
{
    QString text;

    Qt::KeyboardModifiers modifiers;

    //Modifier to symbol
    if( e->modifiers() & Qt::ShiftModifier )
    {
        text += QString::fromUtf8( "⇧" );
        modifiers |= Qt::ShiftModifier;
    }
    if( e->modifiers() & Qt::MetaModifier )
    {
        text += QString::fromUtf8( "⌃" );
        modifiers |= Qt::MetaModifier;
    }
    if( e->modifiers() & Qt::AltModifier )
    {
        text += QString::fromUtf8( "⌥" );
        modifiers |= Qt::AltModifier;
    }
    if( e->modifiers() & Qt::ControlModifier )
    {
        text += QString::fromUtf8( "⌘" );
        modifiers |= Qt::ControlModifier;
    }

    //Backspace key to clear shortcut
    if( !e->modifiers() && e->key() == Qt::Key_Backspace)
    {
        lineEdit()->clear();
        m_modifiers = 0;
        m_key = 0;
        return;
    }

    //Don't allow shortcuts with no modifier unless they're F keys
    if( !e->modifiers() && !( e->key() >= 0x01000030 && e->key() <= 0x01000052 ))
        return;

    //Don't allow pure modifier shortcuts
    switch(e->key())
    {
        case Qt::Key_Control:
        case Qt::Key_Alt:
        case Qt::Key_Meta:
        case Qt::Key_Shift:
            return;
    }

    //Literal Space to Space text
    if( e->key() == Qt::Key_Space)
        text += " Space" ;
    else if( e->key() == Qt::Key_Escape )
        text += QString::fromUtf8(" ⎋");
    else if( e->key() == Qt::Key_Backspace )
        text += QString::fromUtf8(" ⌫");
    else if( e->key() >= 0x01000030 && e->key() <= 0x01000052 )
        text += QString( " F%1" ).arg(((int)e->key() - 0x0100002F) );
    else
        text = text + " " + e->key();

    m_key = e->nativeVirtualKey();
    m_modifiers = e->modifiers();

    setCurrentIndex( findText( text, Qt::MatchFixedString ) );
    lineEdit()->setText( text );
}

QString
ShortcutEdit::textValue() const
{
    return lineEdit()->text();
}

