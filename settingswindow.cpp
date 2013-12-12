#include "settingswindow.h"
#include "ui_settingswindow.h"

using std::string;

settingswindow::settingswindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::settingswindow)
{
  ui->setupUi(this);
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
      ",('lastfm_love_5star','"+ \
      ((ui->lastfm_love_5star->isChecked() == true) ? "true" : "false")+"')" \
      ",('lastfm_unlove_n5star','"+ \
      ((ui->lastfm_unlove_n5star->isChecked() == true) ? "true" : "false")+"')" \
      ";");
}
