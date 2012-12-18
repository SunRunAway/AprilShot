#include "titlebar.h"
#include "ui_titlebar.h"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar)
{
    ui->setupUi(this);
    mainWidget = (MainWidget*)parent;
    QObject::connect(this, SIGNAL(doubleClicked()), mainWidget, SLOT(on_maximizeButton_clicked()));
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        if (!mainWidget->showMaximizedState())
            beginPosition = mainWidget->geometry();
        dragPosition = event->globalPos() - mainWidget->frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        if (event->globalY() <= 0) {
            mainWidget->showMaximizedThroughDrag(beginPosition);
        }
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (mainWidget->showMaximizedState()) {
            mainWidget->showNormalThroughDrag(event->globalPos());
            dragPosition = event->globalPos() - mainWidget->frameGeometry().topLeft();
        }

        mainWidget->move(event->globalPos() - dragPosition);

        int x = mainWidget->geometry().x();
        int y = mainWidget->geometry().y();
        int height = QApplication::desktop()->availableGeometry().size().height();
        const int fix = 12;
        if (y < 0)
            mainWidget->move(x, 0);
        if (y > height - fix)
            mainWidget->move(x, height - fix);
        event->accept();
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        emit doubleClicked();
        event->accept();
    }
}
