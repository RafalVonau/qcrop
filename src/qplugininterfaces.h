#ifndef __QPLUGININTERFACES_H__
#define __QPLUGININTERFACES_H__

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QCompleter>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QScriptEngine>
#include <QStyleOptionViewItem>
#include "qabstractdevice.h"
#include "qsettingsmanager.h"


class QToolInterface
{
public:
	virtual ~QToolInterface() {}
	virtual QString name() {return QString();}
	virtual void install() {}
};
Q_DECLARE_INTERFACE(QToolInterface,"elfin.QToolInterface/1.0");


class QLogInterface: public QObject
{
	Q_OBJECT
public:
	QLogInterface(QObject *parent = 0): QObject(parent) {}
	virtual ~QLogInterface() {}
public slots:
	virtual void addDevice(QAbstractDevice *dev) = 0;
	virtual void doCommand(const QString &) = 0;
	virtual void deviceConnect(QString name, QString path) = 0;
	virtual void deviceDisConnect(QString name) = 0;
	virtual void deviceDisConnectAll() = 0;
	virtual void logText(const QString &) = 0;
	virtual void logError(const QString &) = 0;
	virtual void showDialog() = 0;
};
Q_DECLARE_INTERFACE(QLogInterface,"elfin.QLogInterface/1.0")


class QMainWindowInterface: public QMainWindow
{
	Q_OBJECT
public:
	QMainWindowInterface( QWidget * parent = 0, Qt::WindowFlags flags = 0) : QMainWindow(parent, flags) {}
	virtual ~QMainWindowInterface() {}
	virtual void cleanupOnExit() {}
	virtual QMenu *optionsMenu() {
		static QMenu *sm_optionsMenu = 0;
		if (!sm_optionsMenu) {
			sm_optionsMenu = new QMenu(this);
			sm_optionsMenu->setObjectName("Options");
			sm_optionsMenu->setTitle(tr("&Options"));
			menuBar()->addAction(sm_optionsMenu->menuAction());
		}
		return sm_optionsMenu;
	}
	virtual QMenu *toolsMenu() {
		static QMenu *sm_toolsMenu = 0;
		if (!sm_toolsMenu) {
			sm_toolsMenu = new QMenu(this);
			sm_toolsMenu->setObjectName("Tools");
			sm_toolsMenu->setTitle(tr("&Tools"));
			menuBar()->addAction(sm_toolsMenu->menuAction());
		}
		return sm_toolsMenu;
	}
};

class QSettingsEditorInterface: public QObject
{
	Q_OBJECT

public:
	QSettingsEditorInterface(QObject *parent , QObject * object, QString property, QObject *pm): QObject(parent)
	{
		m_propertyModel = qobject_cast<QSettingsEditorModel*>(pm);
		setObject(object);
		setObjectProperty(property);
		setObjectDescription(property);
		m_properties.clear();
		m_defaultValue = value();
		m_connected = true;
	}
	virtual ~QSettingsEditorInterface()
	{
		for (int p = 0;p < m_properties.size();p++)
			delete m_properties[p];
	}
	virtual void setObject(QObject * object)         {m_object = object;}
	virtual QObject * object()                       {return m_object;}

	virtual void setObjectProperty(QString property) {m_property = property;}
	virtual QString objectProperty()                 {return m_property;}
	virtual void setObjectDescription(QString d)     {m_description = d;}
	virtual QString objectDescription()              {return m_description;}


	QVariant value() {return object()->property(objectProperty().toLatin1().data());}
	bool setValue(QVariant data) {
		QVariant old_data = value();
		object()->setProperty(objectProperty().toLatin1().data(), data);
		if (old_data != data) {
			emit(propertyChanged(object(), objectProperty(), old_data, data));
			return true;
		}
		return false;
	}
	QVariant defaultValue()                          {return m_defaultValue;}
	void setDefaultValue(QVariant data)              {m_defaultValue = data;}

	virtual QWidget* createEditor(QWidget * parent, const QModelIndex & index) {Q_UNUSED(parent);Q_UNUSED(index);return 0;}
	virtual bool setEditorData(QWidget * editor, const QModelIndex & index) {Q_UNUSED(editor);Q_UNUSED(index);return false;}
	virtual bool setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const {Q_UNUSED(editor);Q_UNUSED(index);Q_UNUSED(model);return false;}
	virtual bool paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) {Q_UNUSED(painter);Q_UNUSED(option);Q_UNUSED(index); return false;}

	virtual QVariant data(const QModelIndex & index)
	{
		if (!index.isValid() || !object() || objectProperty().isNull())
			return QVariant();
		if (index.column() == 0) {
			return QVariant(objectDescription());
		}
		return value();
	}
	virtual bool setData(QVariant data, const QModelIndex &) {return setValue(data);}

	virtual int rowCount(const QModelIndex & parent)
	{
		if (!parent.isValid() || parent.column())
			return 0;
		return m_properties.size();
	}
	virtual int columnCount(const QModelIndex & parent)
	{
		if (!parent.column() && m_properties.size())
			return 2;
		return 0;
	}
	virtual Qt::ItemFlags flags(const QModelIndex &index)
	{
		if (index.column())
			return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
		else
			return Qt::ItemIsEnabled;
	}
	virtual void setParentIndex(const QModelIndex & parentIndex) {m_parentIndex = parentIndex;}
	virtual QModelIndex parentIndex(const QModelIndex & index) {Q_UNUSED(index); return m_parentIndex;}
	virtual QSettingsEditorInterface* property(int prop) const
	{	if (m_properties.size() <= prop)
			return 0;
		return m_properties[prop];
	}
	void refreshPropertis(QSettingsEditorModel * pm)
	{
		if (!pm)
			return;
		for (int i = 0;i < m_properties.size();i++)
			delete m_properties[i];
		m_properties.clear();
		for (int i = 0;i < metaObject()->propertyCount();i++) {
			if (metaObject()->property(i).isUser() && metaObject()->property(i).isValid() && metaObject()->property(i).isWritable() && metaObject()->property(i).isReadable()) {
				QSettingsEditorInterface* pi = pm->createPropery(this, metaObject()->property(i).name());
				if (pi)
					m_properties.push_back(pi);
			}
		}
	}
	virtual bool isChanged() {
		if (m_defaultValue.isValid()) {
			return (m_defaultValue != value());
		}
		return false;
	}
	bool isConnected() {return m_connected;}
	void setConnected(bool c) {m_connected = c;}
	virtual bool canHandle(const QString &name, const QVariant &value) {Q_UNUSED(name);Q_UNUSED(value); return false;}
signals:
	void propertyChanged(QObject * obj, const QString & propertyName, const QVariant & old_value, const QVariant & new_value);

public:
	QList<QSettingsEditorInterface*> m_properties;

private:
	QModelIndex                      m_parentIndex;
	QObject                         *m_object;
	QString                          m_property;
	QString                          m_description;
	const QSettingsEditorModel      *m_propertyModel;
	QVariant                         m_defaultValue;
	bool                             m_connected;
};
Q_DECLARE_INTERFACE(QSettingsEditorInterface, "elfin.QSettingsEditorInterface/1.0");


#endif // __PLUGINSINTERFACES_H__
