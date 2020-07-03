#ifndef QSETTINGSCATEGORYVIEW_H
#define QSETTINGSCATEGORYVIEW_H
#include <QObject>
#include "core-config.h"

#include <QTreeWidget>
#include <QItemDelegate>

/* Delegate */
class CORE_EXPORT QSettingsCategoryDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	QSettingsCategoryDelegate(QObject *parent = 0): QItemDelegate(parent) {}
	~QSettingsCategoryDelegate() {}

	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
	{return QItemDelegate::sizeHint(option, index) + QSize(4, 10);}
	void paint(QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

/* View */
class CORE_EXPORT QSettingsCategoryView: public QTreeWidget
{
	Q_OBJECT
public:
	QSettingsCategoryView(QWidget *parent = 0);
protected:
	void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
	QSettingsCategoryDelegate m_delegate;
};




#endif // QSETTINGSCATEGORYVIEW_H
