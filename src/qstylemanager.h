#ifndef __QSTYLEMANAGER_H__
#define __QSTYLEMANAGER_H__

#include <QObject>
#include <QString>
#include <QMenu>
#include <QActionGroup>
#include <QApplication>
#include "core-config.h"

class CORE_EXPORT QStyleManager : public QObject
{
	Q_OBJECT

public:
	QStyleManager(QObject * parent, QString appName, QString shDir);
	~QStyleManager();
	Q_PROPERTY(QString style READ getStyle WRITE setStyle)
	Q_PROPERTY(QString styleSheet READ currentStyleSheet WRITE loadStyleSheet)
signals:
	void styleSheetChanged(const QString &sheetName);
	void styleChanged(const QString &styleName);

public slots:
	void useStyleFromMenu(QAction *sb) {setStyle(sb->text()); }
	void useStyleSheetFromMenu(QAction *sb) {loadStyleSheet(sb->text()); }
	void loadStyleSheet(const QString &sheetName);
	QString currentStyleSheet() {return m_windowStyleSheet;}
	void saveSettings();
	void loadSettings();
	void selectAction(QActionGroup *g, QString val);

	void setStyle(const QString &name);
	QString getStyle() {return  m_windowStyle;}
	static QStringList steleSheets() {return sm_steleSheets;}
private:

	
	QString              m_shDir;
	QString              m_appName;
	QString              m_windowStyleSheet;
	QString              m_windowStyle;
	QMenu               *m_menuStyle;
	QMenu               *m_menuStyleSheet;
	QActionGroup        *m_styleActions;
	QActionGroup        *m_styleSheetActions;
	static QStringList   sm_steleSheets;
};

#endif
