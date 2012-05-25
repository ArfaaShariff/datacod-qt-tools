#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "settingsdialog.h"
#include "tablemodel.h"
#include <QSystemTrayIcon>
#include <QTime>
#include <QCloseEvent>
#include <QMenu>
#include "resultdialog.h"

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
    Ui::MainWindow *ui;
    SettingsDialog *settingsdialog;
    SETTINGS settings;
    TableModel *model;
    QSystemTrayIcon *trayicon;
    QMenu *traymenu;
    QTime *time;
    ResultDialog *resdialog;
    QString dialogdir;
    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *event);
    bool okToExit();
    QString listfile;
    void setListFile();

public slots:


private slots:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void allUploadsStarted();
    void allUploadsFinished();
    void settingsPressed();
    void settingsChanged(SETTINGS sets);
    void addPressed();
    void deletePressed();
    void uploadingStarted();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void exitApplication();
    void showLinks();
    void aboutPressed();
};

#endif // MAINWINDOW_H
