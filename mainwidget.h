#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>
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

    void maximized();
    void showMaximized();
    void showMaximizedThroughDrag(const QRect &beginPosition);

    void normal(const QPoint &topLeft, const QSize &size);
    void showNormal();
    void showNormalThroughDrag(const QPoint &mouse);
    bool showMaximizedState();
    void storePositionInfo(const QRect &position);

    void copyImage();

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);

signals:
    void setPixmap(QPixmap pixmap); //设置pixmap信息

private slots:
    void on_shotButton_clicked();
    void on_saveButton_clicked();
    void on_uploadButton_clicked();

    void shootScreen();

    void updateScreenshotLabel();

    void triangleReceiver();
    void triangleReceiver(QPixmap);

    void on_mode0Button_clicked();

    void on_mode1Button_clicked();

    void on_shutButton_clicked();

    void on_minimizeButton_clicked();

    void on_maximizeButton_clicked();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();

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

    bool maximizedState;
    QRect posInfo;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    bool windowVisibility;

    QAction *shotAction;
    QAction *saveAction;
    QAction *uploadAction;
    QAction *quitAction;
};

#endif // MAINWIDGET_H
