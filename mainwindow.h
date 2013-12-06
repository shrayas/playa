#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "player.h"
#include "manager.h"

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
    ~MainWindow();
    void new_media_to_list(map<string, string>);

   private slots:
    void on_play_bt_clicked();
    void on_media_item_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

  private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
