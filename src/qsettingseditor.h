#ifndef __QSETTINGSEDITOR_H__
#define __QSETTINGSEDITOR_H__

#include <QTreeView>
#include <QVariant>
#include <QHash>
#include <QAbstractItemModel>
#include <QItemDelegate>
#include "core-config.h"

class QHBoxLayout;
class QSettingsCategory;


class CORE_EXPORT QSettingsEditor : public QWidget
{
	Q_OBJECT
public:
	QSettingsEditor(QWidget *parent);
	virtual ~QSettingsEditor();
	void setCategorry(QSettingsCategory *cat);
	void refresh();
	void clear();
private:
	QAbstractItemModel                *m_model;
	QItemDelegate                     *m_delegate;
	QTreeView                         *m_view;
	QHBoxLayout                       *m_layout;
};

/* View */
class CORE_EXPORT QSettingsTreeView: public QTreeView
{
	Q_OBJECT
public:
	QSettingsTreeView(QWidget *parent = 0);
public slots:
	void itemClickedSlot( const QModelIndex & index );
protected:
	void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

/* Delegate */
class CORE_EXPORT QSettingsEditorDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	QSettingsEditorDelegate(QObject *parent = 0): QItemDelegate(parent) {}
	~QSettingsEditorDelegate() {}

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void setEditorData(QWidget * editor, const QModelIndex & index) const;
	void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
	void paint(QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};



#endif
