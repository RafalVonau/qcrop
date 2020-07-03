#ifndef QPHOTODELEGATE_H
#define QPHOTODELEGATE_H

#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include "qphotomodel.h"

class QPhotoDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	QPhotoDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}
	virtual ~QPhotoDelegate() {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
	void setModel(QPhotoModel *m) { m_model = m;}
private slots:

private:
	QPhotoModel  *m_model;
protected:
	void parseEditorHints(QWidget* editor, const QString& editorHints) const;
	QPhoto *toItem(const QModelIndex &index) const {
		return toModel(index)->getItem(index);
	}
	const QPhotoModel *toModel(const QModelIndex &index) const {
		return static_cast<const QPhotoModel*>(index.model());
	}
};

#endif // QPHOTODELEGATE_H
