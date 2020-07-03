#ifndef __QPLUGINMANAGER_H__
#define __QPLUGINMANAGER_H__

#include <QObject>
#include <QString>
#include <QMenu>
#include <QActionGroup>
#include <QApplication>
#include <QList>
#include "core-config.h"
#include <QScriptEngine>

class CORE_EXPORT QPluginManager : public QObject
{
	Q_OBJECT

public:
	QPluginManager(QObject * parent, QString appName, QString shDir);
	~QPluginManager();
	
	bool loadPlugin(QObject *plugin);
	void loadPlugins();
signals:

public slots:
	
private:
	QString          m_shDir;
	QString          m_appName;
	QMenu           *m_menuPlugins;
	QList<QObject *> m_plugins;
};

#endif
