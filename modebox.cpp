#include "modebox.h"
#include <QDebug>

ModeBox::ModeBox(QVector<QPushButton *> modeButtons)
{
    this->modeButtons = modeButtons;
}

void ModeBox::setCurrentIndex(int index)
{
    this->index = index;
    for (int i = modeButtons.size() - 1; i >= 0; i--)
    {
        modeButtons[i]->setEnabled(true);
    }
    modeButtons[index]->setDisabled(true);
    qDebug() << "index" << index;
}

int ModeBox::currentIndex()
{
    return this->index;
}
