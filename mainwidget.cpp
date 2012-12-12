#include <QDebug>
#include <QClipboard>
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "rectselector.h"
#include "utils.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    loadSettings();

    ts = new RectSelector();
    connect(this, SIGNAL(setPixmap(QPixmap)), ts, SLOT(loadBackgroundPixmap(QPixmap)));
    connect(ts, SIGNAL(finishPixmap(QPixmap)), this, SLOT(triangleReceiver(QPixmap)));
    connect(ts, SIGNAL(quitPixmap()), this, SLOT(triangleReceiver()));

    shootFullScreen();
    updateScreenshotLabel();
}

MainWidget::~MainWidget()
{
    delete ui;
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
    savePath = settings.value("DefaultOptions/folder", "%USERPROFILE%/Pictures/AprilShot").toString();
    ParsePath(savePath);
    count = settings.value("Count/num", 0).toUInt();

    qDebug() << "delay:" << delay << ", mode:" << mode;
    if (!(delay <= 60 && delay >= 0))
        delay = 2;
    if (!(mode <= 1 && mode >= 0))
        mode = 1;
    ui->delayBox->setValue(delay);
    ui->modeBox->setCurrentIndex(mode);
}

void MainWidget::saveSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.beginGroup("DefaultOptions");
        settings.setValue("delay", ui->delayBox->value());
        settings.setValue("mode", ui->modeBox->currentIndex());
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
    int mode = ui->modeBox->currentIndex();
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
    for (;;)
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
