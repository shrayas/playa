#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QDebug>

using std::string;

/**
 * set the default text values when the window opens
 */
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

/**
 * bye bye
 */
settingswindow::~settingswindow()
{
  delete ui;
}

/**
 * on clicking ok in dialog
 */
void settingswindow::on_settings_btbx_accepted()
{
  // insert data into sql
  manager::db_EXECUTE("INSERT OR REPLACE INTO config_vars(key,value) VALUES" \
                      "('lastfm_api_key','"+ \
                      string(ui->lastfm_api_key->text().toUtf8().constData())+"')" \
                      ",('lastfm_api_secret','"+ \
                      string(ui->lastfm_api_secret->text().toUtf8().constData())+"')" \
                      ",('watch_dir','" + \
                      string(ui->watch_dir->text().toUtf8().constData())+"')" \
                      ";");
  // update vars
  lastfm_helper::api_key = string(ui->lastfm_api_key->text().toUtf8().constData());
  lastfm_helper::api_secret = string(ui->lastfm_api_secret->text().toUtf8().constData());
  manager::watch_dir = string(ui->watch_dir->text().toUtf8().constData());

  // rescan for new media
  std::async(std::launch::async,manager::get_new_media_files);
}

/**
 * show directory for selection for watch_dir
 */
void settingswindow::on_folder_browse_clicked(){
  ui->watch_dir->setText(QFileDialog::getExistingDirectory(this,tr("Select Dir"),"/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

/**
 * on authenticate button clicked for lastfm
 */
void settingswindow::on_lastfm_auth_bt_clicked()
{
  lastfm_helper::authenticate();
  if(lastfm_helper::username.empty())
    ui->lastfm_loggedinstatus->setText("Not logged in");
  else
    ui->lastfm_loggedinstatus->setText(QString::fromStdString("Logged in as "+lastfm_helper::username));
}
