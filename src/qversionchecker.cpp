#include <QApplication>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include "qversionchecker.h"

QVersionChecker::QVersionChecker(QString url, QObject *parent): QObject(parent)
{
	connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
	m_url = url;
	if (url.isEmpty())
		uploadFile("http://elfin-pe.pl/firmware/index.php");
	else
		uploadFile(url);
}
//=====================================================================================================================

void QVersionChecker::uploadFile(QString url)
{
	QNetworkRequest request;
	request.setUrl(QUrl(url));
	request.setRawHeader("User-Agent", "QVersionChecker 1.0");

	QNetworkReply *reply = m_manager.get(request);
	m_currentDownloads.append(reply);
}
//=====================================================================================================================

void QVersionChecker::downloadFinished(QNetworkReply *reply)
{
	QUrl url = reply->url();
	if (reply->error()) {
		qDebug("Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
	} else {
		//qDebug()<<reply->readAll();
		checkVersion(reply);
	}
	m_currentDownloads.removeAll(reply);
	reply->deleteLater();
}
//=====================================================================================================================

int QVersionChecker::rateVersion(QString v)
{
	QRegExp rx("(\\d+)[\\.|\\_](\\d+)[\\.|\\_](\\d+)");
	int pos;
	int rate = 0;
	pos = 0;
	while ((pos = rx.indexIn(v, pos)) != -1) {
		rate = rx.cap(1).toInt()*100000 + rx.cap(2).toInt()*1000 + rx.cap(3).toInt();
		pos += rx.matchedLength();
	}
	return rate;
}
//=====================================================================================================================

void QVersionChecker::checkVersion(QIODevice *data)
{

	QTextStream in(data);
	QRegExp rx(qApp->applicationName() + "-(\\d+(\\.|\\_)\\d+(\\.|\\_)\\d+).*");
	int pos;
	int rate, maxrate;
	QString version;

	maxrate = rateVersion(qApp->property("version").toString());

	while (!in.atEnd()) {
		QString line = in.readLine();
		pos = 0;
		while ((pos = rx.indexIn(line, pos)) != -1) {
			rate = rateVersion(rx.cap(1));
			if (rate > maxrate) {
				maxrate = rate;
				version = rx.cap(1);
			}
			pos += rx.matchedLength();
		}
	}
	if (!version.isNull()) {
		QMessageBox::information(0, tr("New update available"), tr("<center>A new version is available.<br><br>Visit: <a href=\"http://elfin-pe.pl/firmware/\">ELFIN homepage</a> for downloads.</center>"));
	}
}
//=====================================================================================================================
