#include "qsettingscategoryview.h"
#include <QTreeView>
#include <QPainter>
#include <QColor>
#include <QStyleOptionViewItemV3>
#include <QWidget>
#include <QApplication>
#include <QPen>

QSettingsCategoryView::QSettingsCategoryView(QWidget *parent) : QTreeWidget(parent)
{
	//connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClickedSlot(QModelIndex)));
	setItemDelegate(&m_delegate);
}

void QSettingsCategoryView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
		c = QColor(0xf4f6d7);
	} else {
		c = QColor(0xf6fd8b);
	}
	QRadialGradient radialGrad(option.rect.center(),option.rect.width());
	radialGrad.setColorAt(0, c);
	radialGrad.setColorAt(0.5, c.darker(150));
	radialGrad.setColorAt(1, c);
	if (c.isValid()) {
	    painter->fillRect(option.rect, radialGrad);
	    opt.palette.setColor(QPalette::AlternateBase, c.lighter(112));
	}
	QTreeWidget::drawRow(painter, opt, index);
	QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->save();
	painter->setPen(QPen(color));
	painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
	painter->restore();
}


void QSettingsCategoryDelegate::paint(QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{
	QStyleOptionViewItem option = opt;

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
