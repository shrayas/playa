#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

#include <random>

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
    void open_browser_to_auth(string);
    ~MainWindow();
    void new_media_to_list(map<string, string>);
    settingswindow *settings;
    
    void time_changed(float);
    void play_toggled(int);
    void find_next_track(int);

   private slots:
    void on_play_bt_clicked();
    void on_media_item_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_settings_bt_clicked();
    void on_time_slider_valueChanged(int value);
    void on_time_slider_sliderPressed();
    void on_time_slider_sliderReleased();
    void on_prev_bt_clicked();
    void on_next_bt_clicked();

  private:
    Ui::MainWindow *ui;
    int current_item_row_id = 0;
    int current_row_id = 0;
    int previous_row_id = 0;
    int new_seek_value = 0;
    bool slider_pressed = false;
};

#endif // MAINWINDOW_H
