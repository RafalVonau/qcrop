#include "qpluginmanager.h"
#include <qplugininterfaces.h>
#include <QSet>
#include <QFileDialog>
//#include <QUiLoader>
#include <QWidget>
#include <QPluginLoader>
#include <QDebug>
#include "core.h"

QPluginManager::~QPluginManager() 
{
	qDeleteAll(m_plugins);
	m_plugins.clear();
}
//==============================================================================================

bool QPluginManager::loadPlugin(QObject *plugin)
{
	QString name;
	QToolInterface *tool = qobject_cast<QToolInterface *>(plugin);
	if (tool) {
		m_plugins << plugin;
		name = tool->name();
		if (!name.isEmpty()) {
			Core::addTranslator(name);
		}
		tool->install();
		return true;
	}
	delete plugin;
	return false;
}
//==============================================================================================

void QPluginManager::loadPlugins()
{
	QString dir(m_shDir);
	QDir pluginsDir(dir);
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin) {
			loadPlugin(plugin);
		} else {
			qDebug() << loader.errorString();
		}
	}
}
//==============================================================================================


QPluginManager::QPluginManager(QObject * p, QString appName, QString shDir): QObject(p)
{
	m_menuPlugins = 0;
	m_shDir = shDir;
	m_appName = appName;
	loadPlugins();
}
//==============================================================================================

