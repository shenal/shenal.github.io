#ifndef ANIMATED_STATUS_BAR_H
#define ANIMATED_STATUS_BAR_H

#include <lib/DllExportMacro.h>
#include <QStatusBar>

class UNICORN_DLLEXPORT AnimatedStatusBar: public QStatusBar {
    Q_OBJECT
public:
    AnimatedStatusBar( QWidget* parent = 0 );

public slots:
    void showAnimated();
    void hideAnimated();

private slots:
    void onFrameChanged(int);
    void onFinished();

private:
    class QTimeLine* m_timeline;
    int m_windowHeight;
};

#endif //ANIMATED_STATUS_BAR_H
