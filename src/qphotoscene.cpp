#include "qphotoscene.h"

static inline double square(double d)
{
	return d*d;
}
//=====================================================================================================================


QPhotoScene::QPhotoScene(QObject *parent) : QGraphicsScene(parent)
{
	addItem(&m_region);
	m_region.setZValue(100);
}
//=====================================================================================================================

QPhotoScene::~QPhotoScene()
{
	releasePhoto(m_photo);
}
//=====================================================================================================================


void QPhotoScene::releasePhoto(QPointer<QPhoto> b)
{
	if (b) {
		if (m_photodb)
			b->setCropRegion(m_region.cropRegion());
			m_photodb->updateDB(b);
		//delete b;
	}
}
//=====================================================================================================================
void QPhotoScene::setPhoto(QPointer<QPhoto> b)
{
	releasePhoto(m_photo);
	m_photo = b;
	connect(b, SIGNAL(photoReady(QPixmap &)), this, SLOT(photoReady(QPixmap&)));
	b->toPixmap();
}
//=====================================================================================================================

void QPhotoScene::photoReady(QPixmap &p)
{
	QSize s;
	m_pixmap = p;
	setSceneRect(QRectF(0, 0, m_pixmap.size().width(), m_pixmap.size().height()));
	m_region.setPhotoRect(m_photo->size(), m_pixmap.size(), m_photo->cropRegion());
	s = m_pixmap.size();
	emit fitImage(s);
}
//=====================================================================================================================

void QPhotoScene::drawBackground(QPainter *painter, const QRectF &exposed)
{
	painter->drawPixmap(exposed, m_pixmap, exposed);
}
//=====================================================================================================================
/*
void QPhotoScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		QPointF pos = event->scenePos();

	}
	if (event->button() == Qt::RightButton) {
		QPointF pos = event->screenPos();

	}
}
//=====================================================================================================================

void QPhotoScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF pos = event->scenePos();

}
//=====================================================================================================================

void QPhotoScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF pos = event->scenePos();
	QRectF r = m_region.boundingRect();

	if (r.contains(pos)) {
		m_region.setCursor(Qt::SizeAllCursor);
	} else {
		//m_region.setCursor(Qt::ArrowCursor);
	}
}
//=====================================================================================================================

void QPhotoScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) {
	if (wheelEvent->modifiers() != Qt::ControlModifier)
		return;
	QPointF pos = wheelEvent->scenePos();

}
//=====================================================================================================================
*/
