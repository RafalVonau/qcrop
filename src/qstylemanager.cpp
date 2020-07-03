#include "qstylemanager.h"
#include "qsettingsmanager.h"
#include "core.h"
#include <QStyleFactory>
#include <QDir>

QStringList   QStyleManager::sm_steleSheets;

QStyleManager::~QStyleManager()
{
	saveSettings();
}

QStyleManager::QStyleManager(QObject * p, QString appName, QString shDir): QObject(p)
{
	int i;
	QWidget * parent = Core::mainWindow()->optionsMenu();

	m_shDir = shDir;
	m_appName = appName;

	m_menuStyle = new QMenu(parent);
	m_menuStyle->setObjectName("menuStyle");
	m_menuStyle->setTitle(tr("Theme"));
	parent->addAction(m_menuStyle->menuAction());

	m_menuStyleSheet = new QMenu(parent);
	m_menuStyleSheet->setObjectName("menuStyleSheet");
	m_menuStyleSheet->setTitle(tr("Style"));
	parent->addAction(m_menuStyleSheet->menuAction());
	/* Load Styles */
	{
		m_styleActions = new QActionGroup(this);
		QRegExp regExp("Q(.*)Style");
		QStringList styles = QStyleFactory::keys();
		for (i = 0; i < styles.size(); ++i) {
			QAction *act = new QAction(this);
			act->setObjectName(styles.at(i));
			act->setText(styles.at(i));
			act->setCheckable(true);
			act->setIcon(QIcon(QString::fromUtf8(":/core/images/style.png")));
			m_styleActions->addAction(act);
			m_menuStyle->addAction(act);
		}
		connect(m_styleActions, SIGNAL(triggered(QAction *)), SLOT(useStyleFromMenu(QAction *)));
	}
	/* Load StyleSheet */
	{
		m_styleSheetActions = new QActionGroup(this);
		{
			QAction *act = new QAction(this);
			act->setObjectName("Default");
			act->setText("Default");
			act->setCheckable(true);
			m_styleSheetActions->addAction(act);
			m_menuStyleSheet->addAction(act);
			sm_steleSheets<<"Default";
		}
		QDir dir(m_shDir);
		QFileInfoList list;
		list = dir.entryInfoList(QStringList("*.qss"), QDir::Files, QDir::Name);
		for (int i = 0; i < list.size(); ++i) {
			QFileInfo fileInfo = list.at(i);
			QString key = fileInfo.baseName();
			if (!key.contains("default",Qt::CaseInsensitive)) {
				QAction *act = new QAction(this);
				act->setObjectName(key);
				act->setText(key);
				sm_steleSheets<<key;
				act->setCheckable(true);
				m_styleSheetActions->addAction(act);
				m_menuStyleSheet->addAction(act);
			}
		}
		connect(m_styleSheetActions, SIGNAL(triggered(QAction *)), SLOT(useStyleSheetFromMenu(QAction *)));
	}
	m_windowStyle = "Fusion";
	m_windowStyleSheet = "Default";
	loadSettings();
}
//==============================================================================================




void QStyleManager::saveSettings()
{
	Core::settingsManager()->setProperty("appearance", "style", m_windowStyle);
	Core::settingsManager()->setProperty("appearance", "styleSheet", m_windowStyleSheet);
}
//==============================================================================================

void QStyleManager::loadSettings()
{
	QSettingsManager *settings = QSettingsManager::instance();
	QVariant tmp;

	setStyle(settings->property("appearance", "style", m_windowStyle).toString());
	settings->addObjectProperty("appearance", tr("style"), this, "style");
	loadStyleSheet(settings->property("appearance", "styleSheet", m_windowStyleSheet).toString());
	settings->addObjectProperty("appearance", tr("style sheet"), this, "styleSheet");
	settings->setCategoryDisplayname("appearance", tr("appearance"));
}
//==============================================================================================

void QStyleManager::selectAction(QActionGroup *g, QString val)
{
	//qDebug()<<g;
	if (g) {
		QList<QAction *> actions = g->actions();
		for (int i = 0; i < actions.size(); i++) {
			if (actions.at(i)->text() == val) {
				actions.at(i)->setChecked(true);
			}
		}
	}
}
//==============================================================================================
/*!
 * \brief Load and set Style Sheet from file.
 * \param sheetName - filename with style sheet rules.
 */
void QStyleManager::loadStyleSheet(const QString &sheetName)
{
	QString styleSheet = "";
	if (!sheetName.isEmpty() && (sheetName != "Default")) {
		QString name = m_shDir + "/" + sheetName + ".qss";
		//qDebug()<<name;
		QFile file(name);
		file.open(QFile::ReadOnly);
		styleSheet = QLatin1String(file.readAll());
	} else {
		QFile file(m_shDir + "/" + "default.qss");
		file.open(QFile::ReadOnly);
		styleSheet = QLatin1String(file.readAll());
	}
	qApp->setStyleSheet(styleSheet);
	m_windowStyleSheet = sheetName;
	selectAction(m_styleSheetActions, sheetName);
	emit styleSheetChanged(m_windowStyleSheet);
}
//====================================================================================================
/*!
 * \brief Set Application style.
 * \param name - style name.
 */
void QStyleManager::setStyle(const QString &name)
{
	m_windowStyle = name;
	qApp->setStyle(name);
	selectAction(m_styleActions, name);
	emit styleChanged(m_windowStyle);
}
