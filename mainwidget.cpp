#include <QDebug>
#include <QClipboard>
#include <QVector>
#include <QPoint>
#include <QFile>
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "rectselector.h"
#include "utils.h"
#include "qiniu.h"

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

    createActions();
    createTrayIcon();
    windowVisibility = true;

    loadSettings();
    //storePositionInfo();

    connect(this, SIGNAL(setPixmap(QPixmap)), ts, SLOT(loadBackgroundPixmap(QPixmap)));
    connect(ts, SIGNAL(finishPixmap(QPixmap)), this, SLOT(triangleReceiver(QPixmap)));
    connect(ts, SIGNAL(quitPixmap()), this, SLOT(triangleReceiver()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

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
    copyImage();
}

void MainWidget::copyImage()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(originalPixmap);
    qDebug() << "copied.";
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
    qApp->quit();
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
    this->setGeometry(QRect(topLeft, size));

    maximizedState = false;

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->ui->maximizeButton->setIcon(QIcon(":/tb/images/normal.png"));
    this->show();
}

void MainWidget::showNormal() {
    qDebug() << "showNormal";

    normal(posInfo.topLeft(), posInfo.size());
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

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->ui->maximizeButton->setIcon(QIcon(":/tb/images/max.png"));
    this->show();

    this->setGeometry(0, 0, s.width(), s.height());

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

void MainWidget::on_uploadButton_clicked()
{
    QString format = "png";
    QString fileName = QDate::currentDate().toString("yyMMdd") + GenerateRandomString(4) + "." + format;
    bool ok = originalPixmap.save(fileName, format.toAscii());
    if (ok) {
        qDebug() << fileName;
        qiniuup(fileName);
        QFile::remove(fileName);

        QString url = "http://aprilshot.qiniudn.com/" +  fileName;
        QClipboard *cb = QApplication::clipboard();
        cb->setText(url);
        trayIcon->showMessage("The image URL has copied to your clipboard.", url,
                              QSystemTrayIcon::NoIcon,
                              5 * 1000);

    }
}

void MainWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (windowVisibility) {
            hide();
            windowVisibility = false;
        } else {
            show();
            windowVisibility = true;
        }
    default:
        ;
    }
}

void MainWidget::createActions()
{
    shotAction = new QAction(tr("Sho&t"), this);
    connect(shotAction, SIGNAL(triggered()), this, SLOT(on_shotButton_clicked()));

    saveAction = new QAction(tr("&Save"), this);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_saveButton_clicked()));

    uploadAction = new QAction(tr("&Get &Link"), this);
    connect(uploadAction, SIGNAL(triggered()), this, SLOT(on_uploadButton_clicked()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWidget::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(shotAction);
    trayIconMenu->addAction(saveAction);
    trayIconMenu->addAction(uploadAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/systray/images/AprilShot_santa.ico"));
    trayIcon->setToolTip("AprilShot");
    trayIcon->show();
}
