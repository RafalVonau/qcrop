#ifndef __QSETTINGSWIDGET_H__
#define __QSETTINGSWIDGET_H__

#include "ui_qsettingswidget.h"
#include "qsettingsmanager.h"
#include <QHash>
#include "core-config.h"
class QSettingsCategory;

class CORE_EXPORT QSettingsWidget : public QWidget {
	Q_OBJECT
private:
	Ui::QSettingsWidget ui;
	QSettingsManager   *SM;
	QHash<QString, QString> m_catmap;
public:
	QSettingsWidget(QSettingsManager *SM_in ,QWidget *parent = 0);
	~QSettingsWidget();
	void refresh_tree();
private slots:
	void slot_current_item_changed(QTreeWidgetItem *item);
private:
	QSettingsCategory m_cat;
};

#endif
