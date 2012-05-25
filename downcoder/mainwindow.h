#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTime>
#include <QSystemTrayIcon>
#include <QMenu>
#include "addlistdialog.h"
#include "settingsdialog.h"
#include "downloader.h"
#include "global.h"
#include "tablemodel.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    SETTINGS settings;
    Ui::MainWindow *ui;
    TableModel *tablemodel;
    AddListDialog *adddialog;
    SettingsDialog *setsdial;
    virtual void closeEvent(QCloseEvent *event);
    QSystemTrayIcon *trayicon;
    QMenu *traymenu;
    QTime time, time2;
    bool numbers[0];
    bool okToExit();
    QString listfile;

private slots:
    void newItemPressed();
    void deleteItemPressed();
    void addListDialogAccepted(const QStringList &urls, bool startImmediately);
    void readSettings();
    void writeSettings();
    void aboutClicked();
    void newSettings(SETTINGS settings);
    void setActionsEnabled(bool is);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void markAsNewPressed();
    void exitPressed();

    void startPressed();
    ////////////////downloading//////////////////////////////////////
    void downloadStarted(const QString &filename, const QString &size, const QString &fileurl);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void allDownloadsStarted();
    void allDownloadsFinished();
};

#endif // MAINWINDOW_H
