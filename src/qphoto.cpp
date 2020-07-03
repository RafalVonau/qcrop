#include <QPixmapCache>
#include <QDebug>
#include <QSize>
#include <QMap>
#include "qphoto.h"
#include "core.h"
#include "qcropregion.h"



QImage bgLoadPhoto(QPhoto *photo)
{
	QImage pm;
	QSettingsManager *settings = QSettingsManager::instance();
	QSize imgSize = settings->property(QString("display"),QString("imgSize"),QSize(640,480)).toSize();

	qDebug()<<"QPhoto load in QtConcurent <"<<photo->getPath()<<">";
	pm.load(photo->getPath());
	photo->setSize(pm.size());
	pm = pm.scaled(imgSize, Qt::KeepAspectRatio);
	return pm;
}
//=====================================================================================================================

QImage bgCreateIcon(QImage pm)
{
	QSettingsManager *settings = QSettingsManager::instance();
	QSize iconSize = settings->property(QString("display"),QString("iconSize"),QSize(64, 64)).toSize();
	pm = pm.scaled(iconSize, Qt::KeepAspectRatio);
	return pm;
}
//=====================================================================================================================

void QPhoto::crop()
{
	QImage pm;
	QSettingsManager *settings = QSettingsManager::instance();
	QSize imgsize = settings->property(QString("crop"),QString("photoSize"),QSize(1795,1205)).toSize();
	int br = settings->property(QString("crop"),QString("border"),5).toInt();
	QPainter painter;
	QRect r_disp, r_pic;
	QPoint center;
	QColor brcolor =  settings->property(QString("crop"),QString("borderColor"),QColor(Qt::white)).value<QColor>();
	/* load picture */
	pm.load(getPath());
	pm = pm.copy(m_cropRegion.toRect());
	if (m_cropRegion.width() > m_cropRegion.height()) {
		pm = pm.scaled(imgsize.width() - (2*br), imgsize.height() - (2*br), Qt::KeepAspectRatio);
		r_disp = QRect(0, 0, imgsize.width(), imgsize.height());
	} else {
		pm = pm.scaled(imgsize.height() - (2*br), imgsize.width() - (2*br), Qt::KeepAspectRatio);
		r_disp = QRect(0, 0, imgsize.height(), imgsize.width());
	}

	QImage res(r_disp.width(),r_disp.height(), QImage::Format_RGB32);
	/* Paint */
	center = r_disp.center();
	r_pic = QRect(0, 0, pm.size().width(), pm.size().height());
	r_pic.moveCenter(center);

	QRegion r1(r_disp);
	QRegion r2(r_pic);

	painter.begin(&res);
	painter.setPen(Qt::NoPen);
	painter.setBrush(brcolor);
	painter.drawRects(r1.xored(r2).rects());
	painter.drawImage(r_pic, pm);
	painter.end();

	res.save(cropFilePath(), "JPEG");
}
//=====================================================================================================================


void QPhoto::toPixmap()
{
	QPixmap pm;
	QSettingsManager *settings = QSettingsManager::instance();

	if (!QPixmapCache::find(m_filePath, pm)) {
		/* If not in cache load in thread */
		if (!m_photoLoadWatcher) {
			m_photoLoadWatcher = new fw_t();
			connect(m_photoLoadWatcher, SIGNAL(finished()), this, SLOT(photoLoaded()));
			m_photoLoadWatcher->setFuture(QtConcurrent::run(bgLoadPhoto, this));
		}
		return;
	}
	if (!m_cropRegion.isValid()) {
		/* Prepare crop region */
		m_cropRegion = QCropRegion::fitToRect(QRectF(0,0, m_photoSize.width(), m_photoSize.height()));
	}
	emit photoReady(pm);
}
//=====================================================================================================================

void QPhoto::preload()
{
}
//=====================================================================================================================
void QPhoto::photoLoaded()
{
	QPixmap pm = QPixmap::fromImage(m_photoLoadWatcher->result());
	/* Insert to Cache */
	QPixmapCache::insert(getPath(), pm);
	if (!cropRegion().isValid()) {
		/* Prepare crop region */
		setCropRegion(QCropRegion::fitToRect(QRectF(0, 0, m_photoSize.width(), m_photoSize.height())));
	}
	emit photoReady(pm);
	if ((m_icon.isNull()) && (!m_iconGenerateWatcher)) {
		m_iconGenerateWatcher = new fw_t();
		connect(m_iconGenerateWatcher, SIGNAL(finished()), this, SLOT(iconGenerated()));
		m_iconGenerateWatcher->setFuture(QtConcurrent::run(bgCreateIcon, m_photoLoadWatcher->result()));
	}
	m_photoLoadWatcher->deleteLater();
	m_photoLoadWatcher = NULL;
}
//=====================================================================================================================

void QPhoto::iconGenerated()
{
	m_icon = QPixmap::fromImage(m_iconGenerateWatcher->result());
	m_iconGenerateWatcher->deleteLater();
	m_iconGenerateWatcher = NULL;
}
//=====================================================================================================================

QPhoto::QPhoto(QPhoto *parent) : QObject(parent)
{
	setObjectName ("QPhoto");
	m_photoLoadWatcher = NULL;
	m_iconGenerateWatcher = NULL;
}
//=====================================================================================================================

QPhoto::QPhoto(QPhoto *parent, QString path, QSize s, QRectF cr, QString attr, QPixmap icon) : QObject(parent), m_filePath(path), m_photoSize(s), m_cropRegion(cr), m_attr(attr)
{
    QFileInfo fi(path);
    m_icon = icon;
    qDebug()<<"QPhoto created (path="<<path<<" "<<s<<" "<<cr<<" attr="<<attr<<")";
    setObjectName ("QPhoto");
    m_parentItem = parent;
    m_photoLoadWatcher = NULL;
    m_iconGenerateWatcher = NULL;
    m_fileName = fi.fileName();
}
//=====================================================================================================================

QPhoto::~QPhoto()
{
	if (m_photoLoadWatcher) {
		m_photoLoadWatcher->waitForFinished();
	}
	if (m_iconGenerateWatcher) {
		m_iconGenerateWatcher->waitForFinished();
	}
	qDebug()<<"QPhoto destroy <"<<m_filePath<<">";
}
//=====================================================================================================================

void QPhoto::appendChild(QPhoto *item)
{
	m_childItems.append(item);
}
//==============================================================================================

QPhoto *QPhoto::child(int number)
{
	return m_childItems.value(number);
}
//======================================================================================================================

int QPhoto::childCount() const
{
	return m_childItems.count();
}
//======================================================================================================================

int QPhoto::childNumber() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<QPhoto*>(this));

	return 0;
}
//======================================================================================================================

int QPhoto::columnCount() const
{
	return 1;
}
//======================================================================================================================

QVariant QPhoto::data(int column) const
{
	return QVariant(m_fileName);
}
//======================================================================================================================

bool QPhoto::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > m_childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QPhoto *item = new QPhoto(this);
		m_childItems.insert(position, item);
	}

	return true;
}
//======================================================================================================================

bool QPhoto::insertColumns(int position, int columns)
{
	return false;
}
//======================================================================================================================

QPhoto *QPhoto::parent()
{
	return m_parentItem;
}
//======================================================================================================================

bool QPhoto::removeChildren(int position, int count)
{
	if (position < 0 || position + count > m_childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete m_childItems.takeAt(position);

	return true;
}
//======================================================================================================================

bool QPhoto::removeColumns(int position, int columns)
{
	return false;
}
//======================================================================================================================

bool QPhoto::setData(int column, const QVariant &value)
{
	return false;
	/*
		if (column == 0) {
			QPhoto *p = parent();
			if (p) {
				p->m_childMap.remove(getName());
				m_itemData[column] = value;
				p->m_childMap[getName()] = this;
			}
		} else {
			m_itemData[column] = value;
		}
	return true;
	*/
}
//======================================================================================================================

