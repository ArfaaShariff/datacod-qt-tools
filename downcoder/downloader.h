#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include "urlextracter.h"

class DownLoader : public QObject
{
    Q_OBJECT

public:
    DownLoader(QObject *parent = 0);
    ~DownLoader();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    QString filename, path;
    bool isAborted;
    bool isPaused;
//    bool isFirst;
    QString errorStr;
    Urlextracter *extracter;
    QString papka, fileurl;
    bool overwriteFiles;
    QString redirectLocation;

private slots:
    void headersChanged();
    void dataReady();
    void requestFinished();
    void dataCodUrlReceived(const QString &urlstr);

public slots:
    void download(const QString &urlstr, const QString &folder, bool overwrite);
    void downloadFromDataCod(const QString &urlstr, const QString &folder, bool overwrite);
    void cancelDownload();
    void pauseDownload();

signals:
    void started(const QString &filename, qint64 size, const QString &fileurl);
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void finished(const QString &error, bool aborted, bool paused);
};

#endif // DOWNLOADER_H
