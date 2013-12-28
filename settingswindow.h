#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <future>
#include <QDialog>
#include <QFileDialog>
#include "manager.h"
#include "lastfm_helper.h"

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
    void on_folder_browse_clicked();
    void on_lastfm_auth_bt_clicked();

  private:
    Ui::settingswindow *ui;
};

#endif // SETTINGSWINDOW_H
