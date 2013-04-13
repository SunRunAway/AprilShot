#include "utils.h"
#include <QStringList>
#include <QProcess>

void ParsePath(QString &path)
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

    QString ret = "";
    bool envScope = false;
    QString envString = "";

    for (int i = 0; i < path.size(); i++)
    {
        if (path[i] == '%') {
            if (envScope) {
                envScope = false;
                ret.append(environment.value(envString));
            } else {
                envScope = true;
                envString = "";
            }
        } else if (envScope) {
            envString.append(path[i]);
        } else {
            ret.append(path[i]);
        }
    }
    path = ret;
}

QString GenerateRandomString(const int len) {
    QString s = "";
    for (int i=0; i<len; i++)
        s += rand() % 26 + 'a';
    for (int i=0; i<len; i++)
        s += rand() % 10 + '0';
    return QString(s);
}
