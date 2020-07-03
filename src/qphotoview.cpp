#include "qphotoview.h"
#include <QStyleOptionViewItemV3>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QColor>
#include <QModelIndex>
#include <QApplication>

QPhotoView::QPhotoView(QWidget *parent) : QTreeView(parent)
{
}

void QPhotoView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV3 opt = option;
	QColor c;
	QModelIndex id = index;
	int row = 0;
	while (id.isValid()) {
		row += id.row() + 1;
		id = id.parent();
	}
	if (row%2) {
		c = option.palette.color(QPalette::Base);
	} else {
		c = option.palette.color(QPalette::Base).darker(104);
	}
	//QRadialGradient radialGrad(option.rect.center(),option.rect.width());
	//radialGrad.setColorAt(0, c);
	//radialGrad.setColorAt(0.5, c.darker(130));
	//radialGrad.setColorAt(1, c);
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

