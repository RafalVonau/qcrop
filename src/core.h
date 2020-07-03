#ifndef __CORE_H__
#define __CORE_H__

#include <QMainWindow>
#include <QString>
#include <QMetaObject>
#include <QObject>
#include <QStringList>
#include "qplugininterfaces.h"
#include "qsettingsmanager.h"
#include "core-config.h"

class QSettingsManager;
class QPluginManager;
class QStyleManager;
class QScriptManager;
class QAudio;

class CORE_EXPORT Core
{
public:
	Core(const QMetaObject *mo, QStringList searchPath = QStringList());
	virtual ~Core();

	int run(QString appName, int argc, char * argv[]);
	static bool addTranslator(QString name, QString locale);
	static bool addTranslator(QString name);

	/* Managers */
	static QSettingsManager       *settingsManager();
	static QPluginManager         *pluginManager();
	static QStyleManager          *styleManager();
	static QMainWindowInterface   *mainWindow();
	static void setLogManager(QLogInterface *);

	/* Directories */
	static void setQssDir(QString dir) {sm_qssDir = dir;}
	static QString qssDir() {return sm_qssDir;}
	static void setPluginsDir(QString dir) {sm_pluginsDir = dir;}
	static QString pluginsDir() {return sm_pluginsDir;}
    static void setUploadURL(QString url) {sm_uploadURL = url;}
    static QString uploadURL() {return sm_uploadURL;}

private:
	static QPluginManager         *sm_pluginManager;
	static QStyleManager          *sm_styleManager;
	static QMainWindowInterface   *sm_mainWindowInterface;
	static const QMetaObject      *sm_mainWindowMO;

	static QString                 sm_qssDir;
	static QString                 sm_pluginsDir;
	static QString                 sm_locale;
    static QString                 sm_uploadURL;
	QStringList                    m_searchPath;
};

#endif // CORE_H
