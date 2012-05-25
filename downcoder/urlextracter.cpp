#include <QRegExp>
#include "urlextracter.h"

Urlextracter::Urlextracter(QObject *parent) : QObject(parent)
{
    starting = "<p align=\"center\"><a href=\"";
    ending = "\"><img src=\"/img/btn_download.gif\" border=\"0\"></a></p>";
    reply = 0;
    manager = new QNetworkAccessManager(this);
}

Urlextracter::~Urlextracter()
{

}

///////////////////////////public slots:///////////////////////////////////////////////
void Urlextracter::getUrl(const QString &pageurl)
{
    QRegExp reg("http://*data.cod.ru/*");
    reg.setPatternSyntax(QRegExp::Wildcard);
    if (!reg.exactMatch(pageurl))
    {
        emit done(pageurl);
        return;
    }
    isFirst = true;
    isAborted = false;
    QUrl url;
    url.setEncodedUrl(pageurl.toAscii());
    if (!url.isValid())
    {
        emit done(QString());
        return;
    }
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Wget");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "Keep-Alive");
    reply = manager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
}

void Urlextracter::abort()
{
    isAborted = true;
    if (reply)
        reply->abort();
}

////////////////////////////private slots://///////////////////////////////////////////
void Urlextracter::readyRead()
{
    if (reply->error()>0)
    {
        emit done(QString());
        pagedata.clear();
        return;
    }
    QByteArray bytes = reply->readAll();
    pagedata.append(bytes);
    if (pagedata.size()>=15000)
    {
        isAborted = true;
        reply->abort();
    }
}

void Urlextracter::finished()
{
    if ((reply->error()>0) && (!isAborted))
    {
        emit done(QString());
        pagedata.clear();
        return;
    }
    int start = pagedata.indexOf(starting, 0);
    if (start>=0)
    {
        start += starting.length();
        int end = pagedata.indexOf(ending, start);
        QString href = pagedata.mid(start, end-start);
        if (!href.isEmpty())
        {
            emit done(href);
            pagedata.clear();
            return;
        }
    }
    emit done(QString());
    pagedata.clear();
    return;
}
