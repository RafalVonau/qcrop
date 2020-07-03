#include "qsettingseditor.h"
#include "qsettingsmanager.h"
#include <QHBoxLayout>
#include <QVariant>
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>
#include "core.h"
#include "qstylemanager.h"
#include "qsettingseditornodes.h"




QSettingsEditor::QSettingsEditor(QWidget *parent) : QWidget(parent)
{
	m_layout = new QHBoxLayout;
	setLayout(m_layout);
	m_model = new QSettingsEditorModel(this, 0);
	m_delegate = new QSettingsEditorDelegate(this);
	m_view = new QSettingsTreeView(this);
	m_view->setItemDelegate(m_delegate);
	m_view->setModel(m_model);
	m_layout->addWidget(m_view);
	clear();

}

QSettingsEditor::~QSettingsEditor()
{
	if (m_delegate)
		delete m_delegate;
	if (m_model)
		delete m_model;
	if (m_view)
		delete m_view;
}

void QSettingsEditor::refresh()
{
	reinterpret_cast<QSettingsEditorModel*>(m_model)->refreshProperties();
}

void QSettingsEditor::clear()
{
	reinterpret_cast<QSettingsEditorModel*>(m_model)->setCategory(NULL);
}

void QSettingsEditor::setCategorry(QSettingsCategory *cat)
{
	reinterpret_cast<QSettingsEditorModel*>(m_model)->setCategory(cat);
}




QSettingsTreeView::QSettingsTreeView(QWidget *parent) : QTreeView(parent)
{
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClickedSlot(QModelIndex)));
}

void QSettingsTreeView::itemClickedSlot( const QModelIndex & index )
{
	if (index.isValid()) {
		if (index.column()) {
			edit(index);
		}
	}
}

void QSettingsTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;
	QColor c;
	if (index.row()%2) {
		c = QColor(0xdee6ff);
	} else {
		c = QColor(0xbfcfff);
	}
	if (c.isValid()) {
	    painter->fillRect(option.rect, c);
	    opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
	}
	QTreeView::drawRow(painter, opt, index);
	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->save();
	painter->setPen(QPen(color));
	painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
	painter->restore();
}

QWidget * QSettingsEditorDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QWidget *editor = 0;
	if (index.isValid()) {
		editor = reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->createEditor(parent, index);
		if (!editor) {
			editor = QItemDelegate::createEditor(parent, option, index);
		}
	}
	return editor;
}

QSize QSettingsEditorDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
	return QItemDelegate::sizeHint(option, index) + QSize(2, 2);
}

void QSettingsEditorDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	if (index.isValid()) {
		if (!reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->setEditorData(editor, index)) {
			QItemDelegate::setEditorData(editor, index);
		}
	}
}

void QSettingsEditorDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	if (index.isValid()) {
		if (!reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->setModelData(editor, model, index)) {
			QItemDelegate::setModelData(editor, model, index);
		}
	}
}

void QSettingsEditorDelegate::paint(QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{
	QStyleOptionViewItem option = opt;
	QSettingsEditorInterface *i = NULL;

	if (index.isValid()) {
		i = reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer());
		if (i) {
			if ((index.column()))
				if (i->isChanged()) {
					option.font.setBold(true);
				if (!i->isConnected())
					option.font.setItalic(true);
			}
			if (!i->paint(painter, option, index)) {
				QItemDelegate::paint(painter, option, index);
			}
		}

	}
	const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
	const QPen oldPen = painter->pen();
	painter->setPen(QPen(color));
	painter->drawLine(option.rect.right(), option.rect.y(),
			  option.rect.right(), option.rect.bottom());
	painter->drawLine(option.rect.x(), option.rect.bottom(),
			  option.rect.right(), option.rect.bottom());
	painter->setPen(oldPen);
}
