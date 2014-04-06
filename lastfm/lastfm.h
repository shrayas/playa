#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>


class lastfm : public QObject
{
public:
	lastfm(QObject *parent);
	bool tracklove();
	bool scrobble();
	~lastfm();

private:
	QString _api_key
		,_api_secret
		,_username;

	QString _gen_qstr();
	QString _gen_api_sign();
	
};

#endif // LASTFM_H
