#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include "player.h"
#include "manager.h"
#include "settingswindow.h"
#include "lastfm_helper.h"

//using namespace manager;
using std::string;
using std::map;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    void time_changed(float);
    void open_browser_to_auth(string);
    ~MainWindow();
    void new_media_to_list(map<string, string>);
    settingswindow *settings;

   private slots:
    void on_play_bt_clicked();
    void on_media_item_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_settings_bt_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
