#ifndef MGR_H
#define MGR_H

#include <QObject>
#include <QDir>
#include <QSql>

class mgr : public QObject
{
	Q_OBJECT

public:
	mgr(QObject *parent);
	bool get_new_files();
	bool db_EXECUTE();
	~mgr();

private:
	QString _set_search_friendly();
	QString _sanitize_trackdata();
	static QString _dir_filter_S;
	static QStringList _dir_filter_ext;
	
};

QString mgr::_dir_filter_S ="*.mp3,*.m4a,*.wma,*.aac";
QStringList mgr::_dir_filter_ext = mgr::_dir_filter_S.split(",");

#endif // MGR_H
