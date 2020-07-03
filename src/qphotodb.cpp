#include <QDebug>
#include <QImageReader>
#include <QPixmapCache>
#include <QByteArray>
#include "qphotodb.h"
#include "core.h"

void QPhotoDB::populate()
{
	QSqlQuery q(QString("select imgPath, imgHeight, imgWidth, cropX, cropY, cropHeight, cropWidth, icon, attr from images"));
	qDebug()<<"Populate !!!";
	while (q.next()) {
		QImage i;
		QByteArray ba = QByteArray::fromBase64(q.value(7).toByteArray());
		i.loadFromData(ba, "PNG");
		QSize s = QSize(q.value(2).toInt(), q.value(1).toInt());
		QRectF r = QRect(q.value(3).toInt(), q.value(4).toInt(), q.value(6).toInt(), q.value(5).toInt());
		QPhoto *p = m_model->addPhoto(m_dir+'/'+q.value(0).toString(), s, r, QString(""), QPixmap::fromImage(i));
		m_photos<<p;
	 }
}
//=====================================================================================================================


QPhotoDB::QPhotoDB(QObject *parent) : QObject(parent)
{
	QSettingsManager *settings = QSettingsManager::instance();
	m_model = NULL;
	QPixmapCache::setCacheLimit(settings->property(QString("display"),QString("cacheSize"), 100).toInt() * 1024);
	m_conversionWatcher = NULL;
}
//=====================================================================================================================

QPhotoDB::~QPhotoDB()
{
}
//=====================================================================================================================

QSqlError QPhotoDB::createDB(QString name)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

	db.setDatabaseName(name);

	if (!db.open())
		return db.lastError();

	QStringList tables = db.tables();
	if (!tables.contains("images", Qt::CaseInsensitive)) {
		QSqlQuery q;
		if (!q.exec(QLatin1String("create table images (imgPath varchar unique not null, imgHeight integer, imgWidth integer, cropX integer, cropY integer, cropHeight integer, cropWidth integer,icon blob, attr varchar); create unique index idx_images on images (patch)")))
			return q.lastError();
	}
	m_model = new QPhotoModel(this);

	return QSqlError();
}
//=====================================================================================================================

void QPhotoDB::newPhoto(QString path)
{
	QSqlQuery q;
	QByteArray ba;
	QBuffer buffer(&ba);

	qDebug()<<"Adding photo to database <"<<path<<">";
	if (!q.prepare(QLatin1String("insert or ignore into images(imgPath, imgHeight, imgWidth, cropX, cropY, cropHeight, cropWidth, icon, attr) values(?, ?, ?, ?, ?, ?, ?, ?, ?)")))
		return;
	/* File Path */
	q.addBindValue(path);
	/* Size */
	q.addBindValue(0);
	q.addBindValue(0);
	/* cropRect */
	q.addBindValue(0);
	q.addBindValue(0);
	q.addBindValue(0);
	q.addBindValue(0);
	/* Icon */
	q.addBindValue(ba.data());
	/* Atributes */
	q.addBindValue(QString());
	/* Execute */
	if (!q.exec()) {
		qDebug()<<"Error <"<<q.lastError()<<">";
	}
}
//=====================================================================================================================

void QPhotoDB::updateDB(QPointer<QPhoto> p)
{
	QSqlQuery q;
	QBuffer buffer;
	QImageWriter writer(&buffer, "PNG");
	QRect r = p->cropRegion().toRect();
	qDebug()<<"Update photo data <"<<p->getPath()<<"> crop region ="<<r;
	writer.write(p->toIcon().toImage());
	QByteArray ba = buffer.data();
	if (!q.exec(QString("update images set imgHeight='%1', imgWidth='%2', cropX='%3', cropY='%4', cropHeight='%5', cropWidth='%6', icon='%7' where imgPath='%8'").arg(p->size().height()).arg(p->size().width()).arg(r.x()).arg(r.y()).arg(r.height()).arg(r.width()).arg(ba.toBase64().data()).arg(p->getName()))) {
		qDebug()<<"Error <"<<q.lastError()<<">";
	}
}

//=====================================================================================================================

void QPhotoDB::loadDirectiory(QString dirname)
{
	QList<QByteArray> formats = QImageReader::supportedImageFormats();
	QDir dir(dirname);
	QStringList l;

	m_dir = dirname;
	for (int i = 0; i < formats.size(); ++i) {
		QString img = QString("*.%1").arg(QString(formats.at(i)));
		l<<img;
	}

	QFileInfoList entries = dir.entryInfoList(l);
	for (int i = 0; i < entries.size(); ++i) {
		newPhoto(entries.at(i).fileName());
	}
	populate();
}
//=====================================================================================================================

QPhoto *QPhotoDB::getPhoto(QModelIndex i)
{
	return model()->getItem(i);
}
//=====================================================================================================================

void bgCropPhoto(QPhoto *p)
{
	p->crop();
}
//=====================================================================================================================
void QPhotoDB::startCrop(QString dir, QProgressBar *pb)
{
	QDir d;
	if (!m_conversionWatcher) {
		m_conversionWatcher = new fwvoid_t();
		connect(m_conversionWatcher, SIGNAL(finished()), this, SLOT(conversionReady()));
		//connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
		if (pb) {
			connect(m_conversionWatcher, SIGNAL(progressRangeChanged(int, int)), pb, SLOT(setRange(int, int)));
			connect(m_conversionWatcher, SIGNAL(progressValueChanged(int)), pb, SLOT(setValue(int)));
		}
		d.mkpath(dir);
		foreach (QPhoto *p, m_photos) {
			p->setCropFilePath(dir+"/"+p->getName());
		}
		m_conversionWatcher->setFuture(QtConcurrent::map(m_photos, bgCropPhoto));
	}
}
//=====================================================================================================================
void QPhotoDB::conversionReady()
{
	//qDebug()<<"Conv ready";
	m_conversionWatcher->deleteLater();
	m_conversionWatcher = NULL;
	emit cropReady();
}
//=====================================================================================================================
