#ifndef TITLEBAR_H
#define TITLEBAR_H


#include "mainwidget.h"
#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT
    
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void doubleClicked();

public:
    explicit TitleBar(QWidget *parent = 0);
    ~TitleBar();
    
private:
    Ui::TitleBar *ui;
    MainWidget *mainWidget;

    QPoint dragPosition;
    QRect beginPosition;
};

#endif // TITLEBAR_H
