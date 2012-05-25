#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QListIterator>
#include <QFileInfo>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, palette.window().color());
    ui->lineEditDescription->setPalette(palette);
    ui->lineEditFilename->setPalette(palette);
    ui->lineEditFileUrl->setPalette(palette);

    tablemodel = new TableModel(this);
    tablemodel->setSettings(&settings);
    ///////////////////////////////////downloads list////////////////////////////////////////////////////////////////
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "downcoder", "downcoder", this);
    listfile = QFileInfo(sets.fileName()).absolutePath() + "/downloads.lst";
    tablemodel->readFromFile(listfile);

    ui->tableView->setModel(tablemodel);

    adddialog = new AddListDialog(this);
    connect(ui->actionAdd_list, SIGNAL(triggered()), adddialog, SLOT(show()));
    connect(adddialog, SIGNAL(dialogOk(QStringList,bool)), this, SLOT(addListDialogAccepted(QStringList, bool)));

    setsdial = new SettingsDialog(this);
    setsdial->setSettings(settings);
    connect(ui->actionSettings, SIGNAL(triggered()), setsdial, SLOT(show()));
    connect(setsdial, SIGNAL(settingsChanged(SETTINGS)), this, SLOT(newSettings(SETTINGS)));

    if (settings.trayicon)
    {
        trayicon = new QSystemTrayIcon(QIcon(":/icons/downcoder.png"), this);
        connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
        traymenu = new QMenu(this);
        traymenu->addAction(ui->actionStart_All);
        traymenu->addAction(ui->actionStop);
        traymenu->addAction(ui->actionExit);
        trayicon->setContextMenu(traymenu);
        trayicon->show();
    } else
    {
        trayicon = 0;
        traymenu = 0;
    }

    //connect(downer, SIGNAL(started(QString,qint64,QString)), this, SLOT(downloadStarted(QString,qint64,QString)));
    //connect(downer, SIGNAL(progress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    //connect(downer, SIGNAL(finished(QString,bool)), this, SLOT(downloadFinished(QString,bool)));

    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newItemPressed()));
    connect(ui->actionMark_as_new, SIGNAL(triggered()), this, SLOT(markAsNewPressed()));
    connect(ui->actionClear_list, SIGNAL(triggered()), tablemodel, SLOT(clear()));
    connect(ui->actionSelect_All, SIGNAL(triggered()), ui->tableView, SLOT(selectAll()));

    connect(ui->actionStart, SIGNAL(triggered()), this, SLOT(startPressed()));
    connect(ui->actionStart_All, SIGNAL(triggered()), tablemodel, SLOT(downloadAll()));
    connect(ui->actionStop, SIGNAL(triggered()), tablemodel, SLOT(stopDownloading()));
    connect(ui->actionPause_downloading, SIGNAL(triggered()), tablemodel, SLOT(pauseDownloading()));

    connect(tablemodel, SIGNAL(downloadingStarted(QString,QString,QString)), this, SLOT(downloadStarted(QString,QString,QString)));
    connect(tablemodel, SIGNAL(downloadingFinished()), this, SLOT(downloadFinished()));
    connect(tablemodel, SIGNAL(progress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(tablemodel, SIGNAL(allDownloadsStarted()), this, SLOT(allDownloadsStarted()));
    connect(tablemodel, SIGNAL(allDownloadsFinished()), this, SLOT(allDownloadsFinished()));
    connect(tablemodel, SIGNAL(statusChanged(QString)), ui->statusBar, SLOT(showMessage(QString)));

    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(deleteItemPressed()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutClicked()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitPressed()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//////////////////////////private://////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (settings.minimizeonclose)
    {
        event->ignore();
        hide();
        return;
    }

    if (!ui->actionStart->isEnabled())
        if (!okToExit())
        {
            event->ignore();
            return;
        }

    if (trayicon)
        trayicon->hide();
    tablemodel->writeToFile(listfile);
    writeSettings();
}

bool MainWindow::okToExit()
{
    return (QMessageBox::warning(this, tr("Warning"), tr("Downloading still in progress. Do you really want to exit?"),
                                 QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes);
}
/////////////////////////private slots://///////////////////////////////////////////
void MainWindow::newItemPressed()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add new download"),
                                         tr("URL:"), QLineEdit::Normal,
                                         "http://", &ok);
    if (ok && !text.isEmpty())
    {
        tablemodel->addUrl(text);
    }
}

void MainWindow::deleteItemPressed()
{
    QModelIndexList indexlist;
    do
    {
        indexlist = ui->tableView->selectionModel()->selectedIndexes();
        if (indexlist.size()>0)
            tablemodel->deleteItem(indexlist[0].row());
    } while (indexlist.size()>0);
}

void MainWindow::addListDialogAccepted(const QStringList &urls, bool startImmediately)
{
    for (int i=0; i<urls.length(); i++)
    {
        tablemodel->addUrl(urls.at(i));
    }
    if (startImmediately)
    {
        tablemodel->downloadAll();
    }
}

void MainWindow::readSettings()
{
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "downcoder", "downcoder", this);
    sets.beginGroup("window");
    resize(sets.value("size", QSize(500, 400)).toSize());
    move(sets.value("position", QPoint(100, 100)).toPoint());
    if (sets.value("ismaximized", false).toBool())
    {
        setWindowState(Qt::WindowMaximized);
    }
    QList<QVariant> varlist = sets.value("splittersizes", QVariant()).toList();
    QList<int> list;
    for (int i=0; i<varlist.size(); i++)
    {
        list << varlist[i].toInt();
    }
    ui->splitter->setSizes(list);
    restoreState(sets.value("state", QByteArray()).toByteArray());
    sets.endGroup();
    sets.beginGroup("application");
    settings.overwrite = sets.value("overwrite", true).toBool();
    settings.folder = sets.value("currentdir", ".").toString();
    settings.trayicon = sets.value("trayicon", false).toBool();
    settings.minimizeonclose = sets.value("minimize", false).toBool();
    sets.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings sets(QSettings::IniFormat, QSettings::UserScope, "downcoder", "downcoder", this);
    sets.beginGroup("window");
    sets.setValue("size", size());
    sets.setValue("position", pos());
    sets.setValue("ismaximized", (windowState()==Qt::WindowMaximized));
    QList<QVariant> list;
    for (int i=0; i<ui->splitter->sizes().size(); i++)
    {
        list << ui->splitter->sizes()[i];
    }
    sets.setValue("splittersizes", list);
    sets.setValue("state", saveState());
    sets.endGroup();
    sets.beginGroup("application");
    sets.setValue("overwrite", settings.overwrite);
    sets.setValue("currentdir", settings.folder);
    sets.setValue("trayicon", settings.trayicon);
    sets.setValue("minimize", settings.minimizeonclose);
    sets.endGroup();
}

void MainWindow::aboutClicked()
{
    QMessageBox::about(this, tr("About"), tr("DownCoder ver. ")+programversion+"\n"+
                                          tr("Downloader for data.cod.ru\n")+
                                          tr("Author - Stiff <stiff.ru@gmail.com>"));
}

void MainWindow::newSettings(SETTINGS settings)
{
    this->settings = settings;
    if (settings.trayicon)
    {
        if (!trayicon)
        {
            trayicon = new QSystemTrayIcon(QIcon(":/icons/downcoder.png"), this);
            connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
            traymenu = new QMenu(this);
            traymenu->addAction(ui->actionStart_All);
            traymenu->addAction(ui->actionStop);
            traymenu->addAction(ui->actionExit);
            trayicon->setContextMenu(traymenu);
            trayicon->show();
        }
    } else
    {
        if (trayicon)
        {
            trayicon->hide();
            delete trayicon;
            trayicon = 0;
            delete traymenu;
            traymenu = 0;
        }
    }
}


void MainWindow::setActionsEnabled(bool is)
{
    ui->actionNew->setEnabled(is);
    ui->actionAdd_list->setEnabled(is);
    ui->actionDelete->setEnabled(is);
    ui->actionClear_list->setEnabled(is);
    ui->actionMark_as_new->setEnabled(is);
    ui->actionStart->setEnabled(is);
    ui->actionStart_All->setEnabled(is);
    ui->actionSettings->setEnabled(is);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason==QSystemTrayIcon::Trigger)
    {
        if (isHidden())
            show();
        else
            hide();
    }
}

void MainWindow::markAsNewPressed()
{
    QModelIndexList indexlist = ui->tableView->selectionModel()->selectedIndexes();
    for (int i=0; i<indexlist.size(); i++)
    {
        QModelIndex index = indexlist.at(i);
        tablemodel->markAsNew(index.row());
    }
}
void MainWindow::exitPressed()
{
    if (!ui->actionStart->isEnabled())
        if (!okToExit())
            return;
    if (trayicon)
        trayicon->hide();
    tablemodel->writeToFile(listfile);
    writeSettings();
    qApp->quit();
}

void MainWindow::startPressed()
{
    tablemodel->downloadSelected(ui->tableView->selectionModel()->selectedIndexes());
}
//////////////////////////////////////////downloading/////////////////////////////////////////

void MainWindow::downloadStarted(const QString &filename, const QString &size, const QString &fileurl)
{
    ui->lineEditFilename->setText(filename);
    ui->lineEditFilename->setCursorPosition(0);
    ui->label_Size->setText(size);
    ui->lineEditFileUrl->setText(fileurl);
    ui->lineEditFileUrl->setCursorPosition(0);

    time.start();
    time2.start();
}

void MainWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if ((time2.elapsed()>200) && (bytesTotal>0))
    {
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesReceived);
        ui->label_Downloaded->setText(bytesToSize(bytesReceived));
        int speed = (bytesReceived * 1000) / time.elapsed();
        ui->label_Speed->setText(bytesToSize(speed).append(tr("/sec")));
        time2.restart();
    }
}

void MainWindow::downloadFinished()
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
}

void MainWindow::allDownloadsStarted()
{
    //setActionsEnabled(false);
}
void MainWindow::allDownloadsFinished()
{
    //setActionsEnabled(true);
}
