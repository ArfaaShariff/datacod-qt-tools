#include "tablemodel.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QIcon>

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    newIcon = new QIcon(":/icons/new.png");
    inprogressIcon = new QIcon(":/icons/downcoder.png");
    doneIcon = new QIcon(":/icons/done.png");
    errorIcon = new QIcon(":/icons/error.png");
    abortedIcon = new QIcon(":/icons/aborted.png");
    pauseIcon = new QIcon(":/icons/pause.png");
    waitIcon = new QIcon(":/icons/wait.png");

    currentnum = -1;

    downloader = new DownLoader(this);

    connect(downloader, SIGNAL(started(QString,qint64,QString)), this, SLOT(downloadStarted(QString,qint64,QString)));
    connect(downloader, SIGNAL(progress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
    connect(downloader, SIGNAL(finished(QString,bool,bool)), this, SLOT(downloadFinished(QString,bool,bool)));
}

TableModel::~TableModel()
{
    delete newIcon;
    delete inprogressIcon;
    delete doneIcon;
    delete errorIcon;
    delete abortedIcon;
    delete pauseIcon;
    delete waitIcon;
}

int TableModel::rowCount(const QModelIndex &) const
{
    return list.size();
}
int TableModel::columnCount(const QModelIndex &) const
{
    return 4;
}
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row()>=list.size())
        return QVariant();

    if (role==Qt::DecorationRole)
        if (index.column()==3)
            return *statusToIcon(list[index.row()].status);

    if (role!=Qt::DisplayRole)
        return QVariant();
    switch (index.column())
    {
        case 0:
            return list[index.row()].url;
            break;
        case 1:
            return list[index.row()].filename;
            break;
        case 2:
            if (list[index.row()].size==0)
                return QString();
            else
                return bytesToSize(list[index.row()].size);
            break;
        case 3:
            return downStatusToStr(list[index.row()].status);
            break;
        //case 4:
            //return list[index.row()].link;
            //break;
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role!=Qt::DisplayRole)
        return QVariant();
    if (orientation!=Qt::Horizontal)
        return QVariant();
    switch (section)
    {
        case 0:
            return tr("Url");
            break;
        case 1:
            return tr("Filename");
            break;
        case 2:
            return tr("Size");
            break;
        case 3:
            return tr("Status");
            break;
        //case 4:
            //return tr("Link to file");
            //break;
    }
    return QVariant();
}

void TableModel::setSettings(SETTINGS *settings)
{
    this->settings = settings;
}

bool TableModel::readFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);
    int i=0;
    while (!in.atEnd())
    {
        TableItem item;
        qint16 statusint;
        in >> item.url >> item.filename >> item.size >> statusint >> item.link;
        item.status = (DownloadStatus)statusint;
        list << item;
        i++;
    }
    emit dataChanged(QModelIndex(), QModelIndex());
    return true;
}
bool TableModel::writeToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_5);
    for (int i=0; i<list.size(); ++i)
    {
        out << list[i].url << list[i].filename << list[i].size << qint16(list[i].status) << list[i].link;
    }
    return true;
}
/////////////////////////////////////////public slots:////////////////////////////////////////////////
////////////////////////////////////////managing items///////////////////////////////////////////////
void TableModel::addUrl(const QString &url)
{
    beginInsertRows(QModelIndex(), list.size(), list.size());
    TableItem item;
    item.url = url;
    item.status = DownNew;
    item.size = 0;
    list << item;
    endInsertRows();
}
void TableModel::deleteUrl(int number)
{
    beginRemoveRows(QModelIndex(), number, number);
    list.removeAt(number);
    endRemoveRows();
}
void TableModel::clear()
{
    if (list.size()>0)
    {
        beginRemoveRows(QModelIndex(), 0, list.size()-1);
        list.clear();
        endRemoveRows();
    }
}
void TableModel::deleteItem(int number)
{
    beginRemoveRows(QModelIndex(), number, number);
    list.removeAt(number);
    endRemoveRows();
}
void TableModel::markAsNew(int number)
{
    list[number].status = DownNew;
    emit dataChanged(index(number, 3), index(number, 3));
}
//////////////////////////////managing downloading////////////////////////////////////////////////////////
void TableModel::downloadSelected(const QModelIndexList &indexlist)
{
    if (indexlist.size()==0)
        return;

    foreach(QModelIndex idx, indexlist)
    {
        if (idx.column()==0)
        {
            list[idx.row()].status = DownWait;
            emit dataChanged(index(idx.row(), 3), index(idx.row(), 3));
        }
    }

    if (currentnum>=0)
        return;

    currentnum = indexlist[0].row();
    emit allDownloadsStarted();
    downloader->downloadFromDataCod(list[currentnum].url, settings->folder, settings->overwrite);
}
void TableModel::downloadAll()
{
    int firstnum = -1;
    for (int i=0; i<list.size(); i++)
    {
        if ((list[i].status!=DownDone) && (list[i].status!=DownInProgress))
        {
            list[i].status = DownWait;
            emit dataChanged(index(i, 3), index(i, 3));
            if (firstnum<0)
                firstnum = i;
        }
    }

    if ((currentnum<0) && (firstnum>=0))
    {
        currentnum = firstnum;
        emit allDownloadsStarted();
        downloader->downloadFromDataCod(list[currentnum].url, settings->folder, settings->overwrite);
    }
}
void TableModel::stopDownloading()
{
    downloader->cancelDownload();
}
void TableModel::pauseDownloading()
{
    downloader->pauseDownload();
}
///////////////////////////////////////private//////////////////////////////////////////////
QIcon *TableModel::statusToIcon(DownloadStatus status) const
{
    switch (status)
    {
        case DownNew:
            return newIcon;
            break;
        case DownInProgress:
            return inprogressIcon;
            break;
        case DownDone:
            return doneIcon;
            break;
        case DownError:
            return errorIcon;
            break;
        case DownAborted:
            return abortedIcon;
            break;
        case DownPaused:
            return pauseIcon;
            break;
        case DownWait:
            return waitIcon;
            break;
    }
    return 0;
}

bool TableModel::downloadNext()
{
    currentnum++;
    //if (currentnum<list.size() && (list[currentnum].status==DownWait))
    for (int i=currentnum; i<list.size(); i++)
        if (list[currentnum].status==DownWait)
        {
            downloader->downloadFromDataCod(list[currentnum].url, settings->folder, settings->overwrite);
            return true;
        }

    for (int i=0; i<list.size(); i++)
    {
        if (list[i].status==DownWait)
        {
            currentnum = i;
            downloader->downloadFromDataCod(list[currentnum].url, settings->folder, settings->overwrite);
            return true;
        }
    }

    currentnum = -1;
    return false;
}

//////////////////////////////////////private slots:////////////////////////////////////////
void TableModel::downloadStarted(const QString &filename, qint64 size, const QString &fileurl)
{
    list[currentnum].filename = filename;
    list[currentnum].size = size;
    list[currentnum].link = fileurl;
    list[currentnum].status = DownInProgress;
    emit dataChanged(index(currentnum, 1), index(currentnum, 4));

    emit statusChanged(tr("Downloading..."));
    emit downloadingStarted(filename, bytesToSize(size), fileurl);
}

void TableModel::downloadFinished(const QString &error, bool aborted, bool paused)
{
    emit downloadingFinished();

    if (aborted)
    {
        emit statusChanged(tr("Downloading %1 aborted").arg(list[currentnum].filename));
        for (int i=0; i<list.size(); i++)
        {
            if ((list[i].status==DownInProgress) || (list[i].status==DownWait))
            {
                list[i].status = DownAborted;
                emit dataChanged(index(i, 3), index(i, 3));
            }
        }
        currentnum = -1;
        emit allDownloadsFinished();
        return;
    }
    if (paused)
    {
        emit statusChanged(tr("Downloading %1 paused").arg(list[currentnum].filename));
        list[currentnum].status = DownPaused;
        emit dataChanged(index(currentnum, 3), index(currentnum, 3));
        return;
    }
    if (!error.isEmpty())
    {
        emit statusChanged(tr("Error downloading %1: %2").arg(list[currentnum].url).arg(error));
        list[currentnum].status = DownError;
    } else
    {
        emit statusChanged(tr("Downloading %1 finished").arg(list[currentnum].url));
        list[currentnum].status = DownDone;
    }
    emit dataChanged(this->index(currentnum, 3), this->index(currentnum, 3));
    emit downloadingFinished();

    if (!downloadNext())
        emit allDownloadsFinished();
}

