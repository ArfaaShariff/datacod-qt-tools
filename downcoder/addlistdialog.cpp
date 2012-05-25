#include "addlistdialog.h"
#include "ui_addlistdialog.h"

AddListDialog::AddListDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AddListDialog)
{
    m_ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(okPressed()));
}

AddListDialog::~AddListDialog()
{
    delete m_ui;
}

void AddListDialog::changeEvent(QEvent *e)
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

///////////////////////private slots://////////////////////////////////
void AddListDialog::okPressed()
{
    QString text(m_ui->plainTextEdit->toPlainText());
    text.append(" ");
    QStringList strings;
    int pos = 0;
    while (pos>=0)
    {
        pos = text.indexOf("http://", pos);
        if (pos>=0)
        {
            int end = text.indexOf(QRegExp("[\\s\\t\\n\\r]"), pos);
            if (end>0)
            {
                strings << text.mid(pos, end-pos);
            }
            pos = end;
        }
    }
    emit dialogOk(strings, m_ui->checkBoxImmediately->isChecked());
}
