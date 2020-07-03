#ifndef QPHOTOSCENE_H
#define QPHOTOSCENE_H

#include <QGraphicsScene>
#include <QPointF>
#include <QPainter>
#include <QRectF>
#include <QContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPointer>
#include "qcropregion.h"
#include "qphoto.h"
#include "qphotodb.h"


class QPhotoScene : public QGraphicsScene
{
	Q_OBJECT

public:
	QPhotoScene(QObject *parent = 0);
	~QPhotoScene();

	void setPhoto(QPointer<QPhoto> b);
	void setPhotoDB(QPointer<QPhotoDB> d) {m_photodb = d;}
public slots:

	void photoReady(QPixmap &);
signals:
	void fitImage(QSize &);

private:
	void releasePhoto(QPointer<QPhoto> b);

protected:
	virtual void drawBackground(QPainter *painter, const QRectF &exposed);
/*
	void contextMenuEvent(QContextMenuEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
*/
	QPointer<QPhoto>           m_photo;
	QPixmap                    m_pixmap;
	QCropRegion				   m_region;
	QPointer<QPhotoDB>         m_photodb;
};

#endif // QPHOTOSCENE_H
