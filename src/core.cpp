#include "qsettingsmanager.h"
#include "qpluginmanager.h"
#include "qstylemanager.h"
#include "core.h"
#include "qversionchecker.h"
#include "qtsingleapplication.h"
#include <QMessageBox>
#include <QApplication>

#define VER1_(x) #x
#define VER_(x) VER1_(x)
#define VER VER_(APP_VERSION)


void setCurrentDirectory(QStringList checklocations)
{
	QDir test("./scripts");
	if (!test.exists()) {
		foreach (QString l, checklocations) {
			test.setPath(l);
			if (test.exists()) {
				test.setPath("./");
				QDir::setCurrent(l);
				break;
			}
		}
	}
	qDebug() << "Shared data found"<<" <" << QDir::currentPath() + ">";
}
//==============================================================================================


QPluginManager       *Core::sm_pluginManager = 0;
QStyleManager        *Core::sm_styleManager = 0;
QMainWindowInterface *Core::sm_mainWindowInterface = 0;
const QMetaObject    *Core::sm_mainWindowMO = 0;

QString               Core::sm_qssDir = "qss";
QString               Core::sm_pluginsDir = "plugins";
QString               Core::sm_locale = "pl";
QString               Core::sm_uploadURL = "http://elfin-pe.pl/firmware/index.php";

bool Core::addTranslator(QString name, QString locale)
{
	bool ok;
	QTranslator *translator;
	QString fpath = QString("lang/"+name+"_") + locale;
	QFileInfo f(fpath+".qm");

	sm_locale = locale;
	if (!f.exists()) {
		return false;
	}
	translator = new QTranslator();
	ok = translator->load(fpath);
	if (!ok) {
		delete translator;
		return false;
	}
	QCoreApplication::installTranslator(translator);
	return ok;
}

bool Core::addTranslator(QString name)
{
	return addTranslator(name, sm_locale);
}

int Core::run(QString appName, int argc, char * argv[])
{
	int ret;
	QVersionChecker *vc;
	QApplication app(argc, argv);
	app.setApplicationName(appName);
	app.setProperty("version", VER);
	app.setQuitOnLastWindowClosed(true);

	if (m_searchPath.isEmpty()) {
		m_searchPath << qApp->applicationDirPath();
		m_searchPath << "/usr/share/"+qApp->applicationName();
	}
	setCurrentDirectory(m_searchPath);
	QStringList paths = app.libraryPaths();
	paths << QCoreApplication::applicationDirPath() + "/" + pluginsDir();
	app.setLibraryPaths(paths);

	QString locale = QLocale::system().name().section("_",0,0);

	addTranslator("qt", locale);
	addTranslator(appName, locale);

	if (!sm_mainWindowInterface) {
		QSettingsManager::instance()->restoreSettings();
		sm_mainWindowInterface = qobject_cast<QMainWindowInterface *>(sm_mainWindowMO->newInstance());
		QSettingsManager::instance()->installMenu(sm_mainWindowInterface->optionsMenu());
		sm_styleManager = new QStyleManager(sm_mainWindowInterface, qApp->applicationName(), qssDir());
		sm_pluginManager = new QPluginManager(sm_mainWindowInterface, qApp->applicationName(), pluginsDir());
		QSettingsManager::instance()->loadWindowState(qApp->applicationName(), sm_mainWindowInterface);
	}
	mainWindow()->show();
	//mainWindow()->connect(&instance, SIGNAL(showWindow()), mainWindow(), SLOT(show()));
	vc = new QVersionChecker(uploadURL(), sm_mainWindowInterface);
	ret = app.exec();
	QSettingsManager::instance()->saveWindowState(qApp->applicationName(), sm_mainWindowInterface);
	sm_mainWindowInterface->cleanupOnExit();
	//if (sm_mainWindowInterface) delete sm_mainWindowInterface;
	QSettingsManager::instance()->saveSettings();

	return ret;
}

Core::Core(const QMetaObject *mo, QStringList searchPath)
{
	sm_mainWindowMO = mo;
	m_searchPath = searchPath;
}

Core::~Core()
{
}


QSettingsManager *Core::settingsManager(){return QSettingsManager::instance();}
QPluginManager *Core::pluginManager() {return sm_pluginManager;}
QStyleManager *Core::styleManager() {return sm_styleManager;}
QMainWindowInterface   *Core::mainWindow() {return sm_mainWindowInterface;}

