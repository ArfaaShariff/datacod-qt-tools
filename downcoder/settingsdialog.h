#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include "global.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void changeDirPressed();

public slots:
    void settingsDialogAccepted();
    void setSettings(SETTINGS settings);

signals:
    void settingsChanged(SETTINGS settings);

private:
    Ui::SettingsDialog *m_ui;
};

#endif // SETTINGSDIALOG_H
