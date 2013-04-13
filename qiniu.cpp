#include <QString>
#include <QStringList>
#include <QProcess>

const QString access = "ZCrGxxp1ki4mHUCKOPj9f0uLHP0owN--BdQMOdEF";
const QString secret = "1r9J1v7UEW5qFrDsh6r3TpNJ3u68CT5BLNuTtKO5";

void qiniuup(const QString fileName)
{
    QString program = "qiniuup";
    QStringList arguments;
    arguments << access << secret << fileName;
    QProcess *p = new QProcess;
    p->start(program, arguments);


    p->waitForFinished();
    delete p;
}
