#include "qphotodelegate.h"
#include <QtGui>


void QPhotoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	QStyleOptionViewItem option = opt;
	QPhoto *p = toItem(index);
	QVariant data = index.model()->data(index, Qt::EditRole);

	QItemDelegate::paint(painter, option, index);

	const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
	const QPen oldPen = painter->pen();
	painter->setPen(QPen(color));
	painter->drawLine(option.rect.right(), option.rect.y(),
			  option.rect.right(), option.rect.bottom());
	painter->drawLine(option.rect.x(), option.rect.bottom(),
			  option.rect.right(), option.rect.bottom());
	painter->setPen(oldPen);
}

QSize QPhotoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(64, 64);//QItemDelegate::sizeHint(option, index) + QSize(32, 32);
}

QWidget *QPhotoDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QWidget* editor = 0;
/*
	QTesterItem *p = toItem(index);
	if (m_editMode) {
		return QItemDelegate::createEditor(parent, option, index);
	}
	editor = p->createEditor(parent, option);
	if (!editor) {
		if (QString(p->metaObject()->className()) != QString("QTesterItemGroup")) {
			m_model->measure(p);
		}
	}
	*/
	return editor;
}

void QPhotoDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
/*
	QTesterItem *p = toItem(index);
	QVariant data = index.model()->data(index, Qt::EditRole);
	if (!p->setEditorData(editor, data)) {
		 QItemDelegate::setEditorData(editor, index);
	}
	*/
}

void QPhotoDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	/*
	QVariant data = index.model()->data(index, Qt::EditRole);
	QTesterItem *p = toItem(index);

	if (m_editMode) {
		QItemDelegate::setModelData(editor, model, index);
		p->checkValue(p->getValue());
	} else {
		data = p->editorData(editor);
		if (data.isValid()) {
			p->setValue(data);
		}
	}
	*/
}

void QPhotoDelegate::parseEditorHints(QWidget* editor, const QString& editorHints) const
{
	if (editor && !editorHints.isEmpty()) {
		// Parse for property values
		QRegExp rx("(.*)(=\\s*)(.*)(;{1})");
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(editorHints, pos)) != -1) {
			//qDebug("Setting %s to %s", qPrintable(rx.cap(1)), qPrintable(rx.cap(3)));
			editor->setProperty(qPrintable(rx.cap(1).trimmed()), rx.cap(3).trimmed());
			pos += rx.matchedLength();
		}
	}
}



