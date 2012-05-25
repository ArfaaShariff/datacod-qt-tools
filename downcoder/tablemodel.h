#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "downloader.h"
#include "settingsdialog.h"
#include "global.h"


struct TableItem
{
    QString url;
    QString filename;
    qint64 size;
    DownloadStatus status;
    QString link;
};

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    TableModel(QObject *parent = 0);
    ~TableModel();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setSettings(SETTINGS *settings);
    bool readFromFile(const QString &filename);
    bool writeToFile(const QString &filename);

public slots:
    /////////////////managing items////////////////////////
    void addUrl(const QString &url);
    void deleteUrl(int number);
    void clear();
    void deleteItem(int number);
    void markAsNew(int number);
    /////////////////managing downloading///////////////////
    void downloadSelected(const QModelIndexList &indexlist);
    void downloadAll();
    void stopDownloading();
    void pauseDownloading();

private:
    QList<TableItem> list;
//    QList <int> numbers;
    int currentnum;

    QIcon *newIcon;
    QIcon *inprogressIcon;
    QIcon *doneIcon;
    QIcon *errorIcon;
    QIcon *abortedIcon;
    QIcon *pauseIcon;
    QIcon *waitIcon;

    DownLoader *downloader;
    SETTINGS *settings;
    QIcon *statusToIcon(DownloadStatus status) const;
    bool downloadNext();

private slots:
    void downloadStarted(const QString &filename, qint64 size, const QString &fileurl);
    void downloadFinished(const QString &error, bool aborted, bool paused);

signals:
    void statusChanged(const QString &status);
    void progress(qint64 bytesSent, qint64 bytesTotal);
    void downloadingStarted(const QString &filename, const QString &size, const QString &fileurl);
    void downloadingFinished();
    void allDownloadsStarted();
    void allDownloadsFinished();
};

#endif // TABLEMODEL_H
