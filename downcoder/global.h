#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QObject>

const QString programversion = "0.4 alpha";

struct SETTINGS
{
    QString folder;
    bool overwrite;
    bool trayicon, minimizeonclose;

};

enum DownloadStatus
{
    DownNew = 0,
    DownInProgress = 1,
    DownDone = 2,
    DownError = 3,
    DownAborted = 4,
    DownPaused = 5,
    DownWait = 6
};

QString bytesToSize(qint64 size);
QString downStatusToStr(DownloadStatus status);
#endif // GLOBAL_H
