#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButtonSelectDir, SIGNAL(clicked()), this, SLOT(changeDirPressed()));
    connect(this, SIGNAL(accepted()), this, SLOT(settingsDialogAccepted()));
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

///////////////////////private slots:///////////////////////////////
void SettingsDialog::changeDirPressed()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Directory for downloaded files"), m_ui->lineEditDir->text());
    if (!dir.isEmpty())
    {
        m_ui->lineEditDir->setText(dir);
    }
}

////////////////////////////public slots://////////////////////////////
void SettingsDialog::settingsDialogAccepted()
{
    SETTINGS settings;
    settings.folder = m_ui->lineEditDir->text();
    settings.overwrite = m_ui->checkBoxOverWrite->isChecked();
    settings.trayicon = m_ui->checkBoxTrayIcon->isChecked();
    settings.minimizeonclose = m_ui->checkBoxMinimize->isChecked();
    emit settingsChanged(settings);
}

void SettingsDialog::setSettings(SETTINGS settings)
{
    m_ui->lineEditDir->setText(settings.folder);
    m_ui->checkBoxOverWrite->setChecked(settings.overwrite);
    m_ui->checkBoxTrayIcon->setChecked(settings.trayicon);
    m_ui->checkBoxMinimize->setChecked(settings.minimizeonclose);
}
