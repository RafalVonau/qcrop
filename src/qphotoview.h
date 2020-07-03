#ifndef QPHOTOVIEW_H
#define QPHOTOVIEW_H

#include <QTreeView>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QWidget>

/* View */
class QPhotoView: public QTreeView
{
	Q_OBJECT
public:
	QPhotoView(QWidget *parent = 0);
protected:
	void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
#endif // QPHOTOVIEW_H
