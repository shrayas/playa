#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "manager.h"

namespace Ui {
class settingswindow;
}

class settingswindow : public QDialog
{
    Q_OBJECT

public:
    explicit settingswindow(QWidget *parent = 0);
    ~settingswindow();

private slots:
    void on_settings_btbx_accepted();

private:
    Ui::settingswindow *ui;
};

#endif // SETTINGSWINDOW_H
