#ifndef ADDLISTDIALOG_H
#define ADDLISTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class AddListDialog;
}

class AddListDialog : public QDialog {
    Q_OBJECT
public:
    AddListDialog(QWidget *parent = 0);
    ~AddListDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddListDialog *m_ui;

private slots:
    void okPressed();

signals:
    void dialogOk(const QStringList &urls, bool startImmediately);
};

#endif // ADDLISTDIALOG_H
