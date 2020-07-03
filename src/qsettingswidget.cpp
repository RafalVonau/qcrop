#include "qsettingswidget.h"
#include <QHeaderView>

QSettingsWidget::QSettingsWidget(QSettingsManager *SM_in ,QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	SM = SM_in;
	SM->loadWidgetState(QString("QSettingsWidget"), this);
	connect(ui.tree, SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)), this, SLOT(slot_current_item_changed(QTreeWidgetItem *)));
	refresh_tree();
//	connect(ui.editor, SIGNAL(propertyChanged(QObject *,QString)), this, SLOT(slot_property_changed(QObject *,QString)));
}

QSettingsWidget::~QSettingsWidget()
{
	SM->saveWidgetState(QString("QSettingsWidget"), this);
}

void QSettingsWidget::refresh_tree()
{
	QList<QTreeWidgetItem *> topLevelItems;
	QList<QSettingsCategory> catg;
	int i;
	ui.tree->clear();
	ui.tree->header()->hide();
	m_catmap.clear();
	catg = SM->objectCategories();

	foreach (QSettingsCategory cat, catg) {
		QString c = cat.display_name;
		QStringList path = c.split("/");
		if (!path.contains("private")) {
			m_catmap[cat.display_name] = cat.name;
			if (!path.size()) {
				/* Flat categories */
				QTreeWidgetItem *item = new QTreeWidgetItem;
				item->setText(0, c);
				topLevelItems<<item;

			} else {
				/* Tree categories */
				QTreeWidgetItem *parent = 0;
				QTreeWidgetItem *node = 0;
				/* find parent */
				QString master = path.at(0);
				path.pop_front();
				foreach (QTreeWidgetItem *it, topLevelItems) {
					if (it->text(0) == master) {
						parent = it;
						break;
					}
				}
				/* Create parent if not found */
				if (!parent) {
					parent = new QTreeWidgetItem;
					parent->setText(0, master);
				}
				topLevelItems<<parent;

				/* iterate childrens */
				foreach (QString n, path) {
					node = 0;
					for (i = 0; i< parent->childCount(); ++i) {
						QTreeWidgetItem *it = parent->child(i);
						if (it->text(0) == n) {
							node = it;
							break;
						}
					}
					if (!node) {
						node = new QTreeWidgetItem;
						node->setText(0, n);
						parent->addChild(node);
					}
					parent = node;
				}
			}
			ui.tree->addTopLevelItems(topLevelItems);
		}
	}
}

void QSettingsWidget::slot_current_item_changed(QTreeWidgetItem *item)
{
	QStringList path;
	QString c;
	QTreeWidgetItem *node = item;
	/* Create category path */
	do {
		path.push_front(node->text(0));
		node = node->parent();
	} while (node);
	c = m_catmap[path.join("/")];
	if (c.length()) {
		m_cat = SM->getCategory(c);
		ui.editor->setCategorry(&m_cat);
	}
}

