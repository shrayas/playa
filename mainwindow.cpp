#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"

static int c_new_media_to_list(void *param, int argc, char **argv, char **azColName)
{
  MainWindow* mw = reinterpret_cast<MainWindow*>(param);
  map<string, string> media_data;
  for(int i=0; i<argc; i++){
    //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    media_data[azColName[i]] = argv[i] ? argv[i] : NULL;
  }
  mw->new_media_to_list(media_data);
  return 0;
}


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  using namespace std::placeholders;

  manager::manager_init();
  player::player_init();

  //manager::get_new_media_files("/media/karthik/Out/Music");
  player::on_time_changed_cb = std::bind(&MainWindow::time_changed, this, _1);

  ui->media_item_tableWidget->setColumnCount(4);

  ui->media_item_tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Title"),QTableWidgetItem::Type));
  ui->media_item_tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(QString("Artist"),QTableWidgetItem::Type));
  ui->media_item_tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(QString("Album"),QTableWidgetItem::Type));
  ui->media_item_tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem(QString("Id"),QTableWidgetItem::Type));
  //ui->media_item_tableWidget->horizontalHeader()->setStretchLastSection(true);
  //manager::get_display_files();
  manager::db_EXECUTE("SELECT title,album,artist,rowid FROM tracks",c_new_media_to_list,this);
  //ui->media_item_tableWidget->setRowCount(0);
}

MainWindow::~MainWindow()
{
  delete ui;
  manager::manager_shutdown();
  player::player_shutdown();
}

void MainWindow::on_play_bt_clicked()
{
  if (player::is_playing == 1)
    player::pause();
  else
    player::play();
}

void MainWindow::time_changed(float time){
  //qDebug() << time;
  ui->time_slider->setValue(10000000 * time);
}

void MainWindow::new_media_to_list(map<string, string> media_data){
  int row_no = ui->media_item_tableWidget->rowCount();
  //ui->media_item_tableWidget->setRowCount(10);
  ui->media_item_tableWidget->insertRow(row_no);
  //ui->media_item_tableWidget->setItem(row_no,0,new QTableWidgetItem("bOOM!!"));
  ui->media_item_tableWidget->setItem(row_no,0,new QTableWidgetItem(media_data["title"].c_str()));
  ui->media_item_tableWidget->setItem(row_no,1,new QTableWidgetItem(media_data["artist"].c_str()));
  ui->media_item_tableWidget->setItem(row_no,2,new QTableWidgetItem(media_data["album"].c_str()));
  ui->media_item_tableWidget->setItem(row_no,3,new QTableWidgetItem(media_data["rowid"].c_str()));
  //std::cout << media_data["title"] << media_data["album"]<< media_data["artist"] << media_data["rowid"] << std::endl;
}

void MainWindow::on_media_item_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
  int row_id = ui->media_item_tableWidget->item(item->row(),3)->text().toInt();
  player::set_media(row_id);
}
