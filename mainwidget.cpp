#include <QDebug>
#include <QClipboard>
#include <QVector>
#include <QPoint>
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "rectselector.h"
#include "utils.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    ts(new RectSelector),
    maximizedState(false)
{
    ui->setupUi(this);

    QVector<QPushButton*> modeButtons;
    modeButtons.push_back(ui->mode0Button);
    modeButtons.push_back(ui->mode1Button);
    modeBox = new ModeBox(modeButtons);

    loadSettings();
    //storePositionInfo();

    connect(this, SIGNAL(setPixmap(QPixmap)), ts, SLOT(loadBackgroundPixmap(QPixmap)));
    connect(ts, SIGNAL(finishPixmap(QPixmap)), this, SLOT(triangleReceiver(QPixmap)));
    connect(ts, SIGNAL(quitPixmap()), this, SLOT(triangleReceiver()));

    shootFullScreen();
    updateScreenshotLabel();
}

MainWidget::~MainWidget()
{
    delete ui;
    delete ts;
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWidget::resizeEvent(QResizeEvent *)
{
    QSize scaledSize = originalPixmap.size();
    scaledSize.scale(ui->imageLabel->size(), Qt::KeepAspectRatio);
    if (!ui->imageLabel->pixmap()
            || scaledSize != ui->imageLabel->pixmap()->size())
        updateScreenshotLabel();
}

void MainWidget::loadSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    int delay = settings.value("DefaultOptions/delay", 2).toInt();
    int mode = settings.value("DefaultOptions/mode", 1).toInt();
    savePath = settings.value("DefaultOptions/folder", "%HOME%/Pictures/AprilShot").toString();
    ParsePath(savePath);
    count = settings.value("Count/num", 0).toUInt();

    qDebug() << "delay:" << delay << ", mode:" << mode;
    if (!(delay <= 60 && delay >= 0))
        delay = 2;
    if (!(mode <= 1 && mode >= 0))
        mode = 1;
    ui->delayBox->setValue(delay);
    modeBox->setCurrentIndex(mode);
}

void MainWidget::saveSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.beginGroup("DefaultOptions");
        settings.setValue("delay", ui->delayBox->value());
        settings.setValue("mode", modeBox->currentIndex());
    settings.endGroup();
    settings.beginGroup("Count");
        settings.setValue("num", count);
    settings.endGroup();
}

void MainWidget::triangleReceiver()
{
    show();
}

void MainWidget::triangleReceiver(QPixmap pixmap)
{
    originalPixmap = pixmap;
    updateScreenshotLabel();
    show();
}

void MainWidget::on_shotButton_clicked()
{
    hide();
    QTimer::singleShot(ui->delayBox->value() * 1000, this, SLOT(shootScreen()));
}

void MainWidget::shootScreen()
{
    int mode = modeBox->currentIndex();
    if (ui->delayBox->value() != 0)
        qApp->beep();

    originalPixmap = QPixmap(); // clear image for low memory situations
                                // on embedded devices.
    switch (mode) {
    case 0:
        shootFullScreen();
        qDebug("0");
        break;
    case 1:
        shootTriangleScreen();
        qDebug("1");
        break;
    }
}

void MainWidget::shootFullScreen()
{
    originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    updateScreenshotLabel();
    show();
}

void MainWidget::shootTriangleScreen()
{
    QPixmap bgPixmap = ts->getFullScreenPixmap();
    ts->show();

    emit setPixmap(bgPixmap); //发送信号，使用当前的屏幕的图片作背景图片
}

void MainWidget::updateScreenshotLabel()
{
    ui->imageLabel->setPixmap(originalPixmap.scaled(ui->imageLabel->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
}

void MainWidget::on_copyButton_clicked()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(originalPixmap);
}

void MainWidget::on_saveButton_clicked()
{
    QString format = "png";
    QString folder = savePath;
    QDir temp(folder);
    if (!temp.exists()) {
        qDebug() << folder;
        bool ok = temp.mkpath(folder);
        qDebug() << ok;
        if (!ok) {
            folder = QDir::currentPath();
        }
    }

    QString initialPath;
    while (true)
    {
        initialPath = folder + ("/screenshot_"+ QString::number(count, 10) +".") + format;
        QFile tmpf(initialPath);
        if (tmpf.exists())
            count++;
        else
            break;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(format.toUpper())
                               .arg(format));
    if (!fileName.isEmpty()) {
        bool ok = originalPixmap.save(fileName, format.toAscii());
        if (ok)
            count++;
    }
}

void MainWidget::on_mode0Button_clicked()
{
    modeBox->setCurrentIndex(0);
}

void MainWidget::on_mode1Button_clicked()
{
    modeBox->setCurrentIndex(1);
}

void MainWidget::on_shutButton_clicked()
{
    close();
}

void MainWidget::storePositionInfo(const QRect &position) {
    qDebug() << showMaximizedState();
    if (showMaximizedState()){
        return;
    }
    qDebug() << "storePositionInfo" << position;

    posInfo = position;
}

void MainWidget::normal(const QPoint &topLeft, const QSize &size) {
    this->resize(size);
    this->move(topLeft);
    maximizedState = false;

    this->hide();
    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->show();
}

void MainWidget::showNormal() {
    qDebug() << "showNormal";

    normal(posInfo.topLeft(), posInfo.size());
    /*this->resize(posInfo.size());
    this->move(posInfo.topLeft());
    maximizedState = false;*/
}

void MainWidget::showNormalThroughDrag(const QPoint &mouse) {
    qDebug() << "showNormalThroughDrag";

    int maxWidth = QApplication::desktop()->availableGeometry().size().width();
    int new_x = mouse.x() - int(double(mouse.x()) / maxWidth * posInfo.size().width());
    if (new_x < 0) {
        new_x = 0;
    }
    QPoint p = QPoint(new_x, 0);
    normal(p, posInfo.size());

}

void MainWidget::maximized() {
    QSize s = QApplication::desktop()->availableGeometry().size();
    this->hide();
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->show();
    this->resize(s.width(), s.height());
    this->move(0, 0);

    maximizedState = true;
}

void MainWidget::showMaximized() {
    storePositionInfo(this->geometry());
    qDebug() << "showMaximized";

    maximized();
}


void MainWidget::showMaximizedThroughDrag(const QRect &beginPosition) {
    storePositionInfo(beginPosition);
    qDebug() << "showMaximizedThroughDrag";

    maximized();
}


bool MainWidget::showMaximizedState() {
    return maximizedState;
}

void MainWidget::on_maximizeButton_clicked()
{
    qDebug() << "on_maximizeButton_clicked";
    if (maximizedState) {
        showNormal();
    } else {
        showMaximized();
    }
}

void MainWidget::on_minimizeButton_clicked()
{
    showMinimized();
}
