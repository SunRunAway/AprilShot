#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "rectselector.h"
#include "modebox.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

signals:
    void setPixmap(QPixmap pixmap); //设置pixmap信息

private slots:
    void on_shotButton_clicked();

    void on_copyButton_clicked();

    void on_saveButton_clicked();

    void shootScreen();

    void updateScreenshotLabel();

    void triangleReceiver();
    void triangleReceiver(QPixmap);

    void on_mode0Button_clicked();

    void on_mode1Button_clicked();

private:
    Ui::MainWidget *ui;
    RectSelector *ts;
    ModeBox *modeBox;
    QPixmap originalPixmap;
    QString savePath;
    unsigned int count;

    void shootFullScreen();
    void shootTriangleScreen();
    void loadSettings();
    void saveSettings();

};

#endif // MAINWIDGET_H
