#ifndef PROXYWIDGET_H
#define PROXYWIDGET_H

#include <QWidget>

namespace Ui { class ProxyWidget; }

class ProxyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProxyWidget(QWidget *parent = 0);
    ~ProxyWidget();

    void save();

signals:
    void changed();

private slots:
    void onChanged();

private:
    Ui::ProxyWidget* ui;
};

#endif // PROXYWIDGET_H
