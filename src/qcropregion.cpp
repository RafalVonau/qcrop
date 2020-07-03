#include <QPen>
#include "qcropregion.h"
#include "core.h"

QCropRegion::QCropRegion(QGraphicsItem * parent): QGraphicsItem(parent)
{
	m_action = None;
	setAcceptHoverEvents (true);
}
//=====================================================================================================================

QRectF QCropRegion::fitToRect(QRectF photo)
{
	QRectF res;
	QSettingsManager *settings = QSettingsManager::instance();
	/* Prepare crop region */
	int br = settings->property(QString("crop"),QString("border"),5).toInt();
	QSize ph = settings->property(QString("crop"),QString("photoSize"),QSize(1795,1205)).toSize();
	int x,y,c;
	double ratio;

	res.setX(0);
	res.setY(0);

	x = ph.width() - (2 * br);;
	y = ph.height() - (2 * br);
	ratio = (double)y/(double)x;

	if (photo.height() < photo.width()) {
		res.setWidth(photo.width());
		res.setHeight(photo.width() * ratio);
	} else {
		res.setWidth(photo.height() * ratio);
		res.setHeight(photo.height());
	}
	res.moveCenter(photo.center());
	return res;
}
//=====================================================================================================================

void QCropRegion::setPhotoRect(QSize photo, QSize display, QRectF rect)
{
	QRectF r;

	m_photo = photo;
	m_display = display;
	m_rect = rect.normalized();

	m_factorX = (double)m_display.width()/(double)m_photo.width();
	m_factorY = (double)m_display.height()/(double)m_photo.height();

	r.setX(m_rect.x() * m_factorX);
	r.setY(m_rect.y() * m_factorY);
	r.setWidth(((m_rect.x() + m_rect.width()) * m_factorX) - r.x());
	r.setHeight(((m_rect.y() + m_rect.height()) * m_factorY) - r.y());
	setRect(r);
}
//=====================================================================================================================

void QCropRegion::updateRects()
{
	QRectF r;

	r.setX(m_displayRect.x() / m_factorX);
	r.setY(m_displayRect.y() / m_factorY);
	r.setWidth(((m_displayRect.x() + m_displayRect.width()) / m_factorX) - r.x());
	r.setHeight(((m_displayRect.y() + m_displayRect.height()) / m_factorY) - r.y());
	m_rect = r.normalized();
}
//=====================================================================================================================

void QCropRegion::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
	setCursor(Qt::OpenHandCursor);
	if ((m_action != None) && (m_latch != m_displayRect)) {
		updateRects();
	}
	m_action = None;
}
//=====================================================================================================================

void QCropRegion::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	m_action = m_actionToCommit;
	m_latch = m_displayRect;
	switch (m_action) {
		case Move: setCursor(Qt::ClosedHandCursor);break;
		default: break;
	}
}
//=====================================================================================================================

void QCropRegion::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF newCenter;
	QRectF t;
	qreal b;

	switch (m_action) {
		case Move: {
                        newCenter = m_latch.center() + (event->screenPos() - event->buttonDownScreenPos(Qt::LeftButton));
                        t = m_latch;
                        t.moveCenter(newCenter);
                        if (t.x() < 0) {
                                b = newCenter.x() - t.x();
                                newCenter.setX(b);
                        }
                        if (t.y() < 0) {
                                b = newCenter.y() - t.y();
                                newCenter.setY(b);
                        }
                        if (t.x() + t.width() >  m_display.width()) {
                                b = newCenter.x() - (t.x() + t.width() - m_display.width());
                                newCenter.setX(b);
                        }
                        if (t.y() + t.height() > m_display.height()) {
                                b = newCenter.y() - (t.y() + t.height() - m_display.height());
                                newCenter.setY(b);
                        }
                        t.moveCenter(newCenter);
                        setRect(t);
                        update();
                } break;
		case Resize: {
			QPointF p = event->scenePos();
			t = QRectF(QPointF(m_resizePoint), p).normalized();
                        if (t.width() > m_display.width() ) {
                                t.setWidth( m_display.width() );
			}
                        if (t.height() > m_display.height() ) {
                                t.setHeight( m_display.height() );
			}
                        t = fitToRect ( t );
                        if ( p.x() > m_resizePoint.x() ) {
                            p.setX( m_resizePoint.x() + t.width() );
                        } else {
                            p.setX( m_resizePoint.x() - t.width() );
                        }
                        if ( p.y() > m_resizePoint.y() ) {
                            p.setY( m_resizePoint.y() + t.height() );
                        } else {
                            p.setY( m_resizePoint.y() - t.height() );
                        }
                        t = QRectF(QPointF(m_resizePoint), p).normalized();
                        setRect ( t );
			update();
                } break;
		default:break;
	}
}
//=====================================================================================================================

void QCropRegion::hoverMoveEvent ( QGraphicsSceneHoverEvent * event)
{
	int th = 10;
	if (m_action == None) {
		QPointF p = event->scenePos();
		if (m_displayRect.contains(p)) {
			QLineF l1(p, m_displayRect.bottomLeft());
			QLineF l2(p, m_displayRect.bottomRight());
			QLineF l3(p, m_displayRect.topLeft());
			QLineF l4(p, m_displayRect.topRight());
			if (l1.length()<th) {
				setCursor(Qt::SizeBDiagCursor);
				m_actionToCommit = Resize;
				m_resizePoint = QPoint(m_displayRect.x()+m_displayRect.width(), m_displayRect.y());
			} else if (l2.length()<th) {
				setCursor(Qt::SizeFDiagCursor);
				m_actionToCommit = Resize;
				m_resizePoint = QPoint(m_displayRect.x(), m_displayRect.y());
			} else if (l3.length()<th) {
				setCursor(Qt::SizeFDiagCursor);
				m_actionToCommit = Resize;
				m_resizePoint = QPoint(m_displayRect.x()+m_displayRect.width(), m_displayRect.y()+m_displayRect.height());
			} else if (l4.length()<th) {
				setCursor(Qt::SizeBDiagCursor);
				m_actionToCommit = Resize;
				m_resizePoint = QPoint(m_displayRect.x(), m_displayRect.y()+m_displayRect.height());
			} else {
				setCursor(Qt::OpenHandCursor);
				m_actionToCommit = Move;
			}
		} else {
			setCursor(Qt::ArrowCursor);
			m_actionToCommit = None;
		}
	}
}
//=====================================================================================================================

void QCropRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	QRegion r1(boundingRect().toRect());
	QRegion r2(m_displayRect.toRect());
	QPen pen;  // creates a default pen

	pen.setStyle(Qt::DashLine);
	pen.setWidth(2);
	pen.setBrush(Qt::white);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(128,128,128,80));
	painter->drawRects(r1.xored(r2).rects());

	painter->setPen(pen);
	painter->setBrush(QColor(0,0,0,0));
	painter->drawRect(m_displayRect);
}
//=====================================================================================================================

