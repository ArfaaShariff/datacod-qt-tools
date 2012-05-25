#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}
/////////////public/////////////////////////
void SettingsDialog::showDialog(SETTINGS sets)
{
    m_ui->checkBoxAuth->setChecked(sets.authentification);
    m_ui->lineEditEmail->setText(sets.email);
    m_ui->lineEditPassword->setText(sets.password);

    m_ui->comboBoxRegion->setEditText(sets.region);
    m_ui->lineEditPasswordForDownload->setText(sets.passForDown);
    m_ui->lineEditComment->setText(sets.comment);

    m_ui->checkBoxTrayIcon->setChecked(sets.trayicon);
    m_ui->checkBoxMinimizeToTray->setChecked(sets.mintotray);
    this->show();
}
////////////////protected/////////////////////
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

////////////////private slots///////////////////
void SettingsDialog::dialogAccepted()
{
    SETTINGS sets;
    sets.authentification = m_ui->checkBoxAuth->isChecked();
    sets.email = m_ui->lineEditEmail->text();
    sets.password = m_ui->lineEditPassword->text();

    QString reg = m_ui->comboBoxRegion->currentText();
    if (reg.right(1)!="." && (!reg.isEmpty()))
        reg.append(".");
    sets.region = reg;
    sets.passForDown = m_ui->lineEditPasswordForDownload->text();
    sets.comment = m_ui->lineEditComment->text();

    sets.trayicon = m_ui->checkBoxTrayIcon->isChecked();
    sets.mintotray = m_ui->checkBoxMinimizeToTray->isChecked();
    emit settingsChanged(sets);
}
