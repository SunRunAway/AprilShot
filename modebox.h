#ifndef MODEBOX_H
#define MODEBOX_H

#include <QVector>
#include <QPushButton>
#include <QWidget>

class ModeBox
{

public:
    ModeBox(QVector<QPushButton*> modeButtons);
    void setCurrentIndex(int index);
    int currentIndex();
private:
    int index;
    QVector<QPushButton*> modeButtons;
};

#endif // MODEBOX_H
