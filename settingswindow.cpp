#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QDebug>

using std::string;

settingswindow::settingswindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::settingswindow)
{
  ui->setupUi(this);
  ui->lastfm_api_key->setText(QString::fromStdString(lastfm_helper::api_key));
  ui->lastfm_api_secret->setText(QString::fromStdString(lastfm_helper::api_secret));
  ui->watch_dir->setText(QString::fromStdString(manager::watch_dir));

  if(lastfm_helper::username.empty())
    ui->lastfm_loggedinstatus->setText("Not logged in");
  else
    ui->lastfm_loggedinstatus->setText(QString::fromStdString("Logged in as "+lastfm_helper::username));
}

settingswindow::~settingswindow()
{
  delete ui;
}

void settingswindow::on_settings_btbx_accepted()
{
  manager::db_EXECUTE("INSERT OR REPLACE INTO config_vars(key,value) VALUES" \
      "('lastfm_api_key','"+ \
      string(ui->lastfm_api_key->text().toUtf8().constData())+"')" \
      ",('lastfm_api_secret','"+ \
      string(ui->lastfm_api_secret->text().toUtf8().constData())+"')" \
      ",('watch_dir','" + \
      string(ui->watch_dir->text().toUtf8().constData())+"')" \
      ";");
  lastfm_helper::api_key = string(ui->lastfm_api_key->text().toUtf8().constData());
  lastfm_helper::api_secret = string(ui->lastfm_api_secret->text().toUtf8().constData());
  manager::watch_dir = string(ui->watch_dir->text().toUtf8().constData());
  std::async(std::launch::async,manager::get_new_media_files);
}

void settingswindow::on_folder_browse_clicked(){
  ui->watch_dir->setText(QFileDialog::getExistingDirectory(this,tr("Select Dir"),"/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void settingswindow::on_lastfm_auth_bt_clicked()
{
  lastfm_helper::authenticate();
  if(lastfm_helper::username.empty())
    ui->lastfm_loggedinstatus->setText("Not logged in");
  else
    ui->lastfm_loggedinstatus->setText(QString::fromStdString("Logged in as "+lastfm_helper::username));
}
