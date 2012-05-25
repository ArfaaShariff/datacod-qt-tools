#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setListFile();
    model = new TableModel(this);
    model->setSettings(&settings);
    model->readFromFile(listfile);
    ui->tableView->setModel(model);
    readSettings();

    if (settings.trayicon)
    {
        trayicon = new QSystemTrayIcon(this->windowIcon(), this);
        connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                    this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

        traymenu = new QMenu(this);
        traymenu->addAction(ui->actionStart);
        traymenu->addAction(ui->actionStop);
        traymenu->addAction(ui->actionExit);

        trayicon->setContextMenu(traymenu);
        trayicon->setToolTip(tr("Upcoder"));
        trayicon->show();
    } else
    {
        trayicon = 0;
        traymenu = 0;
    }

    time = new QTime();

    settingsdialog = 0;
    resdialog = 0;

    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingsPressed()));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(addPressed()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(deletePressed()));
    connect(ui->actionClear, SIGNAL(triggered()), model, SLOT(clear()));
    connect(ui->actionStart, SIGNAL(triggered()), model, SLOT(startUploading()));
    connect(ui->actionStop, SIGNAL(triggered()), model, SLOT(stopUploading()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionShow_links_to_files, SIGNAL(triggered()), this, SLOT(showLinks()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutPressed()));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showLinks()));

    connect(model, SIGNAL(statusChanged(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(model, SIGNAL(progress(qint64,qint64)), this, SLOT(uploadProgress(qint64,qint64)));
    connect(model, SIGNAL(uploadingStarted()), this, SLOT(uploadingStarted()));
    connect(model, SIGNAL(allUploadsStarted()), this, SLOT(allUploadsStarted()));
    connect(model, SIGNAL(allUploadsFinished()), this, SLOT(allUploadsFinished()));
}

MainWindow::~MainWindow()
{
    delete time;
    delete ui;
}
///////////////////private///////////////////////
void MainWindow::readSettings()
{
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "upcoder", "upcoder", this);
    sets.beginGroup("window");
    resize(sets.value("size", QSize(500, 400)).toSize());
    move(sets.value("position", QPoint(100, 100)).toPoint());
    if (sets.value("ismaximized", false).toBool())
    {
        setWindowState(Qt::WindowMaximized);
    }
    ui->tableView->setColumnWidth(0, sets.value("colwidth0", 300).toInt());
    ui->tableView->setColumnWidth(1, sets.value("colwidth1", 80).toInt());
    ui->tableView->setColumnWidth(2, sets.value("colwidth2", 80).toInt());
    ui->tableView->setColumnWidth(3, sets.value("colwidth3", 150).toInt());
    dialogdir = sets.value("dialogdir", QString()).toString();
    sets.endGroup();
    sets.beginGroup("application");
    settings.authentification = sets.value("auth", false).toBool();
    settings.email = sets.value("email", QString()).toString();
    settings.password = sets.value("password", QString()).toString();
    settings.region = sets.value("region", QString()).toString();
    settings.passForDown = sets.value("passfordown", QString()).toString();
    settings.comment = sets.value("comment", QString()).toString();
    settings.trayicon = sets.value("trayicon", false).toBool();
    settings.mintotray = sets.value("mintotray", false).toBool();
    sets.endGroup();
}
void MainWindow::writeSettings()
{
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "upcoder", "upcoder", this);
    sets.beginGroup("window");
    sets.setValue("size", size());
    sets.setValue("position", pos());
    sets.setValue("ismaximized", (windowState()==Qt::WindowMaximized));
    sets.setValue("colwidth0", ui->tableView->columnWidth(0));
    sets.setValue("colwidth1", ui->tableView->columnWidth(1));
    sets.setValue("colwidth2", ui->tableView->columnWidth(2));
    sets.setValue("colwidth3", ui->tableView->columnWidth(3));
    sets.setValue("dialogdir", dialogdir);
    sets.endGroup();
    sets.beginGroup("application");
    sets.setValue("auth", settings.authentification);
    sets.setValue("email", settings.email);
    sets.setValue("password", settings.password);

    sets.setValue("region", settings.region);
    sets.setValue("passfordown", settings.passForDown);
    sets.setValue("comment", settings.comment);

    sets.setValue("trayicon", settings.trayicon);
    sets.setValue("mintotray", settings.mintotray);
    sets.endGroup();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (settings.mintotray)
    {
        event->ignore();
        hide();
        return;
    }
    if (okToExit())
    {
        model->writeToFile(listfile);
        writeSettings();
        if (trayicon)
            trayicon->hide();
    }
    else
    {
        event->ignore();
    }
}
bool MainWindow::okToExit()
{
    if (!ui->actionAdd->isEnabled())
    {
        return (QMessageBox::warning(this, tr("Warning"), tr("Uploading still in progress. Do you really want to exit?"),
                                     QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes);
    }
    return true;
}
void MainWindow::setListFile()
{
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "upcoder", "upcoder", this);
    listfile = QFileInfo(sets.fileName()).absolutePath() + "/uploads.lst";
}
//////////////////////////////////////////public slots/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////private slots///////////////////////
void MainWindow::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);

    int timeelapsed = time->elapsed();
    if (timeelapsed==0)
        timeelapsed = 1;
    int speed = (bytesSent * 1000) / timeelapsed;
    ui->labelSpeed->setText(bytesToSize(speed).append(tr("/sec")));
}
void MainWindow::allUploadsStarted()
{
    ui->actionAdd->setEnabled(false);
    ui->actionClear->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    ui->actionSettings->setEnabled(false);
    ui->actionStart->setEnabled(false);
}
void MainWindow::allUploadsFinished()
{
    ui->actionAdd->setEnabled(true);
    ui->actionClear->setEnabled(true);
    ui->actionDelete->setEnabled(true);
    ui->actionSettings->setEnabled(true);
    ui->actionStart->setEnabled(true);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->labelSpeed->setText(tr("0 B/sec"));

    if (!resdialog)
    {
        resdialog = new ResultDialog(this);
    }
    resdialog->showWithText(model->urlsList());
}
void MainWindow::settingsPressed()
{
    if (!settingsdialog)
    {
        settingsdialog = new SettingsDialog(this);
        connect(settingsdialog, SIGNAL(settingsChanged(SETTINGS)), this, SLOT(settingsChanged(SETTINGS)));
    }
    settingsdialog->showDialog(settings);
}
void MainWindow::settingsChanged(SETTINGS sets)
{
    this->settings = sets;
    if (settings.trayicon)
    {
        if (!trayicon)
        {
            trayicon = new QSystemTrayIcon(this->windowIcon(), this);
            connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                    this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
            traymenu = new QMenu(this);
            traymenu->addAction(ui->actionAdd);
            traymenu->addAction(ui->actionStart);
            trayicon->setContextMenu(traymenu);
            trayicon->setToolTip(tr("Upcoder"));
            trayicon->show();
        }
    } else
    {
        if (trayicon)
        {
            trayicon->hide();
            delete trayicon;
            delete traymenu;
            trayicon = 0;
            traymenu = 0;
        }
    }

    writeSettings();
}
void MainWindow::addPressed()
{
    QStringList filelist = QFileDialog::getOpenFileNames(this, tr("Add files"), dialogdir);
    if (filelist.size()>0)
    {
        dialogdir = QFileInfo(filelist[0]).absolutePath();
        for (int i=0; i<filelist.size(); i++)
        {
            model->addFile(filelist.at(i));
        }
    }
}
void MainWindow::deletePressed()
{
    int indexlistsize;
    do
    {
        QModelIndexList indexlist = ui->tableView->selectionModel()->selectedRows();
        indexlistsize = indexlist.size();
        if (indexlistsize>0)
            model->deleteFile(indexlist.at(0).row());
    } while (indexlistsize>0);
}

void MainWindow::uploadingStarted()
{
    time->start();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason==QSystemTrayIcon::Trigger)
    {
        if (isHidden())
        {
            show();
            activateWindow();
        }
        else
            hide();
    }
}

void MainWindow::exitApplication()
{
    if (okToExit())
    {
        model->writeToFile(listfile);
        writeSettings();
        trayicon->hide();
        qApp->quit();
    }
}
void MainWindow::showLinks()
{
    QStringList urls = model->urlsList();
    if (!urls.isEmpty())
    {
        if (!resdialog)
        {
            resdialog = new ResultDialog(this);
        }
        resdialog->showWithText(urls);
    }
    else
    {
        QMessageBox::information(this, tr("Information"), tr("There is no uploaded files"));
    }
}
void MainWindow::aboutPressed()
{
    QMessageBox::about(this, QString("Upcoder"), tr("Program for upload files to *data.cod.ru service\n"
                                                    "Author - Stiff <stiff.ru@gmail.com>\n"
                                                    "Program page: http://code.google.com/p/datacod-qt-tools/\n"
                                                    "Version: %1").arg(version));

}
