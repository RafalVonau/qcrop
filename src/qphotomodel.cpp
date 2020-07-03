#include "qphotomodel.h"

QPhotoModel::QPhotoModel(QObject *parent): QAbstractItemModel(parent)
{
	m_rootItem = new QPhoto(NULL);
}
//======================================================================================================================

QPhotoModel::~QPhotoModel()
{
	/* Ok now cleanup model */
	delete m_rootItem;
}
//======================================================================================================================

int QPhotoModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_rootItem->columnCount();
}
//======================================================================================================================

QVariant QPhotoModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QPhoto *item = getItem(index);
	if (role != Qt::DisplayRole && role != Qt::EditRole) {
		return item->getRole(index.column(), role);
	}
	return item->data(index.column());
}
//======================================================================================================================

Qt::ItemFlags QPhotoModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
//======================================================================================================================

QPhoto *QPhotoModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		QPhoto *item = static_cast<QPhoto *>(index.internalPointer());
		if (item)
			return item;
	}
	return m_rootItem;
}
//======================================================================================================================

QVariant QPhotoModel::headerData(int section, Qt::Orientation orientation,int role) const
{
	if (section == 0)
		return QVariant(QString("Name"));
	return QVariant();
}
//======================================================================================================================

QModelIndex QPhotoModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	QPhoto *parentItem = getItem(parent);

	QPhoto *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}
//======================================================================================================================

bool QPhotoModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginInsertColumns(parent, position, position + columns - 1);
	success = m_rootItem->insertColumns(position, columns);
	endInsertColumns();

	return success;
}
//======================================================================================================================

bool QPhotoModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	QPhoto *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, m_rootItem->columnCount());
	endInsertRows();

	return success;
}
//======================================================================================================================

QModelIndex QPhotoModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	QPhoto *childItem = getItem(index);
	QPhoto *parentItem = childItem->parent();

	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}
//======================================================================================================================

bool QPhotoModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginRemoveColumns(parent, position, position + columns - 1);
	success = m_rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (m_rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return success;
}
//======================================================================================================================

bool QPhotoModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	QPhoto *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}
//======================================================================================================================

int QPhotoModel::rowCount(const QModelIndex &parent) const
{
	QPhoto *parentItem = getItem(parent);

	return parentItem->childCount();
}
//======================================================================================================================

bool QPhotoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool b;
	if (role != Qt::EditRole)
		return false;

	QPhoto *item = getItem(index);
	b = item->setData(index.column(), value);
	return b;
}
//======================================================================================================================

bool QPhotoModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	return m_rootItem->setData(section, value);
}
//======================================================================================================================

QPhoto *QPhotoModel::addPhoto(QString path, QSize s, QRectF cr, QString attr, QPixmap icon)
{
	QPhoto *p = new QPhoto(m_rootItem, path, s, cr, attr, icon);
	m_rootItem->appendChild(p);
	return p;
}
//======================================================================================================================
