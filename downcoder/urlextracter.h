#ifndef URLEXTRACTER_H
#define URLEXTRACTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Urlextracter : public QObject
{
    Q_OBJECT

public:
    Urlextracter(QObject *parent = 0);
    ~Urlextracter();

private:
    QString starting, ending;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QByteArray pagedata;
    bool isFirst;
    bool isAborted;

public slots:
    void getUrl(const QString &pageurl);
    void abort();

private slots:
    void readyRead();
    void finished();

signals:
    void done(const QString &url);
};

#endif // URLEXTRACTER_H
