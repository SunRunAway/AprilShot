#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);
    QApplication a(argc, argv);
    MainWidget w;
    w.setWindowFlags(Qt::CustomizeWindowHint);
    //w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();
    
    return a.exec();
}
