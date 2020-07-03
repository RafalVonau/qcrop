#ifndef QVERSIONCHECKER_H
#define QVERSIONCHECKER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include "core-config.h"

class CORE_EXPORT QVersionChecker: public QObject
{
	Q_OBJECT
public:
        QVersionChecker(QString url, QObject *parent = 0);
public slots:
	void uploadFile(QString url);

	void downloadFinished(QNetworkReply *);
private:
	void checkVersion(QIODevice *data);
	int rateVersion(QString v);
	QNetworkAccessManager  m_manager;
	QList<QNetworkReply *> m_currentDownloads;
        QString                m_url;
};

#endif // QVERSIONCHECKER_H
