#ifndef QCROPREGION_H
#define QCROPREGION_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class QCropRegion: public QGraphicsItem
{
	Q_ENUMS(ActType)

	public:
		enum ActType {None, Move, Resize};

		QCropRegion(QGraphicsItem * parent = 0);

		static QRectF fitToRect(QRectF photo);
		void setPhotoRect(QSize photo, QSize display, QRectF rect);
		QRectF cropRegion() const {return m_rect;}

		QRectF boundingRect() const {return QRectF(0,0,m_display.width(),m_display.height()); /* m_displayRect;*/}
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void setRect(QRectF r) {m_displayRect = r;}

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

		void hoverMoveEvent ( QGraphicsSceneHoverEvent * event);
	private:
		void updateRects();

		QSize   m_photo;
		QSize   m_display;
		QRectF  m_rect;
		QRectF  m_displayRect;
		QRectF  m_latch;
		QPoint  m_resizePoint;
		double  m_factorX;
		double  m_factorY;
		ActType m_action;
		ActType m_actionToCommit;
};

#endif // QCROPREGION_H
