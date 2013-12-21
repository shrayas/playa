#include "mainwindow.h"
#include "ui_mainwindow.h"

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
  ui(new Ui::MainWindow){
    ui->setupUi(this);
    using namespace std::placeholders;

    manager::manager_init();
    player::player_init();
    lastfm_helper::lastfm_helper_init();
    lastfm_helper::on_user_should_auth = 
      std::bind(&MainWindow::open_browser_to_auth,this,_1);
    //    lastfm_helper::authenticate();

    // move this to settings
    // manager::get_new_media_files("/media/karthik/Out/Music");
    player::time_changed = std::bind(&MainWindow::on_time_changed, this, _1);
    player::play_toggled = std::bind(&MainWindow::on_play_toggled, this, _1);
    player::end_reached = std::bind(&MainWindow::on_end_reached, this, _1);
    player::media_changed = std::bind(&MainWindow::on_media_changed, this, _1);

    ui->media_item_tableWidget->setColumnCount(3);

    ui->media_item_tableWidget->setHorizontalHeaderItem(
        0,new QTableWidgetItem(QString("Title"),QTableWidgetItem::Type));
    ui->media_item_tableWidget->setHorizontalHeaderItem(
        1,new QTableWidgetItem(QString("Artist"),QTableWidgetItem::Type));
    ui->media_item_tableWidget->setHorizontalHeaderItem(
        2,new QTableWidgetItem(QString("Album"),QTableWidgetItem::Type));

    manager::db_EXECUTE("SELECT title,album,artist,rowid FROM tracks"
        ,c_new_media_to_list,this);
    ui->media_item_tableWidget->resizeColumnsToContents();

    title_lbl_def = ui->title_lbl->text();
    artist_lbl_def = ui->artist_lbl->text();
  }

MainWindow::~MainWindow(){
  delete ui;
  manager::manager_shutdown();
  player::player_shutdown();
}

// called when the next track is needed or when current track ends
// move to player?/manager?
void MainWindow::find_next_track(int){
  int selected_row_id = -1;

  if (ui->shuffle_bt->isChecked()){
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(
        1, ui->media_item_tableWidget->rowCount()); // the range

    // make sure its not the same
    do { selected_row_id = distr(eng); } 
    while(current_item_row_id == selected_row_id);
  }
  else
    selected_row_id = current_item_row_id + 1;

  on_media_item_tableWidget_itemDoubleClicked(
      ui->media_item_tableWidget->item(selected_row_id,0));
}

// EVENTS

void MainWindow::new_media_to_list(map<string, string> media_data){
  int row_no = ui->media_item_tableWidget->rowCount();
  ui->media_item_tableWidget->insertRow(row_no);

  // set the title and add set the sqlite rowid as the a data
  QTableWidgetItem *title_itm = new QTableWidgetItem(media_data["title"].c_str());
  title_itm->setData(Qt::UserRole,media_data["rowid"].c_str());
  ui->media_item_tableWidget->setItem(row_no,0,title_itm);

  ui->media_item_tableWidget->setItem(
      row_no,1,new QTableWidgetItem(media_data["artist"].c_str()));
  ui->media_item_tableWidget->setItem(
      row_no,2,new QTableWidgetItem(media_data["album"].c_str()));
}

void MainWindow::open_browser_to_auth(string Url){
  QDesktopServices::openUrl(QUrl(Url.c_str()));
}

// player EVENTS

void MainWindow::on_time_changed(float time){
  if(!slider_pressed)
    ui->time_slider->setValue(ui->time_slider->maximum() * time);
}

void MainWindow::on_play_toggled(int play_state){
  // if playing set the text to pause
  if(play_state == 1){
    ui->play_bt->setText("Pause");
  }
  else{
    ui->play_bt->setText("Play");
  }
}

void MainWindow::on_end_reached(int){
  ui->title_lbl->setText(title_lbl_def);
  ui->artist_lbl->setText(artist_lbl_def);
  find_next_track(-1);
}

void MainWindow::on_media_changed(map<string,string> track_data){
  ui->title_lbl->setText(QString(track_data["title"].c_str()));
  if(track_data["artist"] != "")
    ui->artist_lbl->setText(QString(track_data["artist"].c_str()));
  else
    ui->artist_lbl->setText(artist_lbl_def);
}
// player EVENTS END


// SLOTS

void MainWindow::on_media_item_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
  // get the rowid stored in the title, play the track
  current_item_row_id = item->row();

  previous_row_id = current_row_id; // we'll use this when going to previously played track
  current_row_id = item->data(Qt::UserRole).toInt();
  player::set_media(current_row_id);
}

void MainWindow::on_time_slider_valueChanged(int value){
  if(slider_pressed)
    new_seek_value = value;
}

void MainWindow::on_time_slider_sliderPressed(){
  slider_pressed = true;
}

void MainWindow::on_time_slider_sliderReleased(){
  slider_pressed = false;
  player::seek_to((float)new_seek_value/(float)ui->time_slider->maximum());
}

void MainWindow::on_play_bt_clicked(){
  if (player::is_playing == 1)
    player::pause();
  else
    player::play();
}

void MainWindow::on_prev_bt_clicked(){
  player::set_media(previous_row_id); // previous -> previous plays the same track again, to fix or not to fix ?
}

void MainWindow::on_next_bt_clicked(){
  find_next_track(-1);
}

void MainWindow::on_settings_bt_clicked(){
  settings = new settingswindow(this);
  settings->show();
}

// SLOTS END
