#ifndef QPHOTOMODEL_H
#define QPHOTOMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMap>
#include <QMetaObject>
#include "qphoto.h"


class QPhotoModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_CLASSINFO("author", "Rafal Vonau")

public:
	QPhotoModel(QObject *parent = 0);
	~QPhotoModel();

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

	bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());

	QPhoto *addPhoto(QString path, QSize s, QRectF cr, QString attr, QPixmap icon);

	QPhoto *getItem(const QModelIndex &index) const;

public slots:

signals:

private:
	QPhoto *m_rootItem;
};

#endif // QPHOTOMODEL_H
