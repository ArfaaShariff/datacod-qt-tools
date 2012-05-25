#include "downloader.h"
#include <QUrl>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextCodec>

DownLoader::DownLoader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    file = 0;
    extracter = 0;
    reply = 0;
    overwriteFiles = false;
}

DownLoader::~DownLoader()
{
}

///////////////////////PRIVATE SLOTS://////////////////////////////////////////////////
void DownLoader::headersChanged()
{
    if (reply->error()>0)
    {
        errorStr = tr("Network error: %1").arg(QString::number(reply->error()));
        return;
    }

    int statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if ((statuscode>=300) && (statuscode<=399))
    {
        redirectLocation = reply->rawHeader(QString("location").toAscii());
        reply->abort();
        return;
    }

    if (reply->hasRawHeader(QString("Content-Disposition").toAscii()))
    {
        QString inf = reply->rawHeader(QString("Content-Disposition").toAscii());
        int startpos = inf.indexOf("filename=", 0, Qt::CaseInsensitive);
        if (startpos>=0)
        {
            int len = inf.length()-startpos-QString("filename=").length();
            if (len>0)
            {
                filename = inf.right(len);
                filename.remove(QChar('\"'));
            }
        }
    }

    qint64 size = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();

    if (!overwriteFiles)
    {
        while (QFile::exists(path+filename))
        {
            filename.append("_1");
        }
    }

    emit started(filename, size, fileurl);
    file = new QFile(path+filename+".part");
    if (!file->open(QIODevice::ReadWrite))
    {
        errorStr = tr("Can 't create file %1").arg(filename);
        reply->abort();
        return;
    }
}

void DownLoader::dataReady()
{
    if (reply->error()>0)
    {
        errorStr = tr("Network error: %1").arg(QString::number(reply->error()));
        return;
    }
    QByteArray bytes = reply->readAll();
    if (file->write(bytes)<0)
    {
        errorStr = tr("Error writing file %1").arg(filename);
        reply->abort();
        return;
    }
}

void DownLoader::requestFinished()
{
    if (!redirectLocation.isEmpty())
    {
        download(QString(redirectLocation), papka, overwriteFiles);
        return;
    }
    if (reply->error()>0 && (errorStr.isEmpty()))
    {
        errorStr = tr("Network error: %1").arg(QString::number(reply->error()));
    }
    if (file)
    {
        file->close();
        if ((isAborted || (!errorStr.isEmpty())) && (!isPaused))
        {
            file->remove();
        } else
        {
            QString fn = file->fileName();
            fn = fn.mid(0, fn.size()-5);
            file->rename(fn);
        }
        delete file;
    }
    emit finished(errorStr, isAborted, isPaused);
    file = 0;
}

void DownLoader::dataCodUrlReceived(const QString &urlstr)
{
    if (urlstr.isEmpty())
    {
        emit finished(tr("Error: Can't receive file URL"), isAborted, isPaused);
        return;
    }
    download(urlstr, papka, overwriteFiles);
}
///////////////////////////////////////////////////////////////////////////////////////

//////////////////////PUBLIC SLOTS:////////////////////////////////////////////////////
void DownLoader::download(const QString &urlstr, const QString &folder, bool overwrite)
{
    //isFirst = true;
    overwriteFiles = overwrite;
    isAborted = false;
    isPaused = false;
    errorStr.clear();
    redirectLocation.clear();

    QUrl url;
    fileurl = urlstr;
    url.setEncodedUrl(urlstr.toAscii());
    if (!url.isValid())
    {
        emit finished(tr("Error: Bad file URL"), isAborted, isPaused);
        return;
    }
    //////filename from url///////////////////////////
    QFileInfo fileinfo(url.path());
    filename = fileinfo.fileName();
    if (filename.isEmpty())
    {
        filename = "index.html";
    }
    if (!folder.isEmpty())
    {
        if (folder.right(1)!="/")
        {
            path = folder;
            path.append("/");
        } else
        {
            path = folder;
        }
    }
    /////////////////////////////////////////////////////

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Wget");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Connection", "Keep-Alive");

    reply = manager->get(request);
    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(headersChanged()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(dataReady()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void DownLoader::downloadFromDataCod(const QString &urlstr, const QString &folder, bool overwrite)
{
    isAborted = false;
    overwriteFiles = overwrite;
    if (!extracter)
    {
        extracter = new Urlextracter(this);
        connect(extracter, SIGNAL(done(QString)), this, SLOT(dataCodUrlReceived(QString)));
    }
    papka = folder;
    extracter->getUrl(urlstr);
}

void DownLoader::cancelDownload()
{
    isAborted = true;
    errorStr = tr("Error: Aborted by user");
    if (extracter)
        extracter->abort();
    if (reply)
        reply->abort();
}

void DownLoader::pauseDownload()
{
    isPaused = true;
    errorStr = tr("Paused");
    if (extracter)
        extracter->abort();
    if (reply)
        reply->abort();
}
///////////////////////////////////////////////////////////////////////////////////////
