#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>


#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    srand(time(NULL));
    Q_INIT_RESOURCE(systray);

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    MainWidget w;
    w.setWindowFlags(Qt::CustomizeWindowHint);
    w.show();
    
    return a.exec();
}
