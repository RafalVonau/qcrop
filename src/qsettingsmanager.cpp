/****************************************************************************
**
** Copyright (C) 2007-2008 ELFIN <rafal.vonau@elfin-pe.pl>
**
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qsettingsmanager.h"
#include "qsettingswidget.h"
#include <QRegExp>
#include <QKeyEvent>
#include <QScrollBar>
#include <QFocusEvent>
#include <QApplication>
#include <QDebug>
#include "core.h"
#include "qstylemanager.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QFocusEvent>
#include "qsettingseditornodes.h"


QDataStream &operator<<(QDataStream &out, const QFilePath &myObj)
{
	QStringList l;
	l<<myObj.path();
	l<<myObj.filter();
	out<<l;
	return out;
}
//====================================================================================================

QDataStream &operator>>(QDataStream &in, QFilePath &myObj)
{
	QStringList s;
	in>>s;
	if (s.length()>1) {
		myObj.setPath(s.at(0));
		myObj.setFilter(s.at(1));
	}
	return in;
}
//====================================================================================================


QSettingsManager::QSettingsManager(QObject *parent) : QObject(parent)
{
	m_view = 0;
	restoreSettings();
}
//====================================================================================================

QSettingsManager::~QSettingsManager()
{
	if (m_view) {
		delete m_view;
	}
	saveSettings();
}

QVariant QSettingsManager::property(const QString &category, const QString &name, const QVariant &def)
{
	QVariant ret;
	if (category.isEmpty()) return QVariant();
	if (name.isEmpty()) return QVariant();
	if ((!m_categories.contains(category)) || (!m_categories[category].properties.contains(name))) {
		setProperty(category, name, def);
		m_categories[category].properties[name].defaultValue = def;
		return def;
	}
	QPointer<QObject> ob = m_categories[category].properties[name].object;
	if (ob) {
		ret = ob->property(name.toLatin1().data());
		if (!m_categories[category].properties[name].defaultValue.isValid())
			m_categories[category].properties[name].defaultValue = def;
	}
	return ret;
}

void QSettingsManager::setDisplayName(const QString &category, const QString &name, const QString &displayName)
{
	if (!m_categories.contains(category)) return;
	if (!m_categories[category].properties.contains(name)) return;
	m_categories[category].properties[name].display_name = displayName;
}

void QSettingsManager::setCategoryDisplayname(const QString &category, const QString &displayName)
{
	if (!m_categories.contains(category)) return;
	m_categories[category].display_name = displayName;
}

void QSettingsManager::setHidden(const QString &category, const QString &name, const bool &h)
{
	if (!m_categories.contains(category)) return;
	if (!m_categories[category].properties.contains(name)) return;
	m_categories[category].properties[name].hidden = h;
}

void QSettingsManager::setProperty(const QString &category, const QString &name, const QVariant &value)
{
	QSettingsProperty P;
	if (category.isEmpty()) return;
	if (name.isEmpty()) return;
	if (!m_categories.contains(category)) {
		QSettingsCategory C;
		C.object = new QObject(this);
		C.name = category;
		generateCatDisplayName(C);
		m_categories[category] = C;
	}
	if (!m_categories[category].properties.contains(name)) {
		P.hidden = false;
		P.name = P.display_name = name;
		P.object = m_categories[category].object;
	} else {
		P = m_categories[category].properties[name];
	}
	if (!P.object) {
		P.object = m_categories[category].object;
	}
	if (P.object) {
		P.object->setProperty(name.toLatin1().data(), value);
		m_categories[category].properties[name] = P;
	}
}


QStringList QSettingsManager::propertyNames(const QString &category)
{
	if (!m_categories.contains(category)) return QStringList();
	return m_categories[category].properties.keys();
}

QStringList QSettingsManager::categoryNames()
{
	return m_categories.keys();
}

void QSettingsManager::restoreSettings()
{
	QSettings settings("ELFIN", qApp->applicationName());
	QStringList allkeys=settings.allKeys();
	for (int j=0; j<allkeys.count(); j++) {
		QStringList list=allkeys[j].split("/");
		if (list.count()>=2) {
			QString val = list[list.size()-1];
			list.pop_back();
			QString cat = list.join("/");
			setProperty(cat, val, settings.value(allkeys[j]));
		}
	}
}

void QSettingsManager::saveSettings()
{
	QSettings settings("ELFIN", qApp->applicationName());
	QStringList cnames = categoryNames();
	for (int j=0; j<cnames.count(); j++) {
		if (cnames[j]!="temporary") {
			QStringList pnames = propertyNames(cnames[j]);
			for (int k=0; k<pnames.count(); k++) {
				QVariant val = property(cnames[j],pnames[k]);
				if (val.isValid()) {
					settings.setValue(cnames[j]+"/"+pnames[k],property(cnames[j],pnames[k]));
				}
			}
		}
	}
}

void QSettingsManager::addObjectProperty(QString category_name,QString display_name,QObject *object,QString name)
{
	if (!object) return;
	QSettingsProperty P, Pold;
	if (!m_categories.contains(category_name)) {
		QSettingsCategory C;
		C.object = new QObject(this);
		C.name = category_name;
		generateCatDisplayName(C, object);
		m_categories[category_name] = C;
	}
	P.display_name = display_name;
	P.object = object;
	P.name = name;
	P.hidden = false;
	Pold = P;
	if (m_categories[category_name].properties.contains(name)) {
		Pold = m_categories[category_name].properties[name];
	};
	if ((Pold.object) && (Pold.object != P.object)) {
		P.object->setProperty(name.toLatin1().data(), Pold.object->property(name.toLatin1().data()));
		generateCatDisplayName(m_categories[category_name], object);
		//qDebug()<<Pold.object->property(name.toAscii().data());
		//object->setProperty(name.toAscii().data(), QPoint(10,10));
	}
	P.hidden = Pold.hidden;
	m_categories[category_name].properties[name] = P;
}

void QSettingsManager::storeObjectProperty(QString category_name,QString display_name,QObject *obj,QString name)
{
	QSettingsProperty P;
	if (!obj) return;
	P = m_categories[category_name].properties[name];
	P.object->setProperty(name.toLatin1().data(), obj->property(display_name.toLatin1().data()));
}

void QSettingsManager::showConfigurationDialog()
{
	if (!m_view) {
		m_view = new QSettingsWidget(this);
		m_view->refresh_tree();
		m_view->setWindowTitle(qApp->applicationName() + " " +tr("settings"));
	}
        //m_view->refresh_tree();
#if defined(Q_WS_X11)
#else
        m_view->setWindowFlags(m_view->windowFlags() | Qt::Tool);
#endif
	m_view->show();
}

void QSettingsManager::saveWindowState(QString name, QMainWindow *o)
{
	NopForTr(tr("windows"));
	//qDebug()<<"Save window "<<o->objectName()<<" "<<o->pos();
	setProperty("windows/"+name, "size", o->size());
	setProperty("windows/"+name, "pos", o->pos());
	setProperty("windows/"+name, "flags", (int)o->windowState());
	setProperty("windows/"+name, "widgets", o->saveState());
	setProperty("windows/"+name, "font", o->property("font"));
	//setProperty("windows/"+name, "palette", o->property("palette"));
	saveSettings();
}
//============================================================================================================

void QSettingsManager::loadWindowState(QString name, QMainWindow *o)
{
	addObjectProperty("windows/"+name, tr("size"), o,"size");
	addObjectProperty("windows/"+name, tr("pos"), o, "pos");
	addObjectProperty("windows/"+name, tr("font"), o, "font");
	//addObjectProperty("windows/"+name, tr("palette"), o, "palette");
	o->setWindowState((Qt::WindowStates)property("windows/"+name,"flags", (int)o->windowState()).toInt());
	o->restoreState(property("windows/"+name,"widgets", o->saveState()).toByteArray());
	setHidden("windows/"+name, "flags", true);
	setHidden("windows/"+name, "widgets", true);
}
//============================================================================================================

void QSettingsManager::loadWidgetState(QString name, QWidget *o)
{
	addObjectProperty("windows/"+name, tr("size"), o,"size");
	addObjectProperty("windows/"+name, tr("pos"), o, "pos");
}
//============================================================================================================

void QSettingsManager::saveWidgetState(QString name, QWidget *o)
{
	setProperty("windows/"+name, "size", o->size());
	setProperty("windows/"+name, "pos", o->pos());
}
//============================================================================================================


void QSettingsManager::generateCatDisplayName(QSettingsCategory &c, QObject *object)
{
	QStringList l = c.name.split("/");
	QStringList t;
	QString translated;
	foreach(QString n, l) {

		//t<<QApplication::tr(n.toAscii());
		translated = QApplication::translate("QSettingsManager", n.toLatin1().data());
		if ((translated == n) && (object)) {
			//qDebug()<< " n = "<<n<<" class = "<<object->metaObject()->className();
			translated = QApplication::translate(object->metaObject()->className(), n.toLatin1().data());
		}
		t<<translated;
	}
	c.display_name = t.join("/");
}
//============================================================================================================

void QSettingsManager::installMenu(QMenu *parent)
{
	QAction *act;
	act = new QAction(this);
	act->setObjectName("Settings Editor");
	act->setText(tr("Settings Editor"));
	//act->setIcon(QIcon(QString::fromUtf8(":/core/images/script_clear.png")));
	parent->addAction(act);
	connect(act, SIGNAL(triggered()), this, SLOT(showConfigurationDialog()));
}




QHash<int, const QMetaObject*> QSettingsEditorModel::sm_types;
QList<const QMetaObject*>      QSettingsEditorModel::sm_autodetectTypes;

QSettingsEditorInterface* QSettingsEditorModel::createPropery(QObject* object, QString property, QString description)
{
	QSettingsEditorInterface* node = 0;
	node = new QSettingsEditorDummyType(object, object, property, this);
	if (!description.isEmpty()) {
		node->setObjectDescription(description);
	}
	return node;
}

const QMetaObject *QSettingsEditorModel::registerNode(int type, const QMetaObject *mo)
{
	const QMetaObject *old = sm_types[type];
	if (mo) {
		sm_types[type] = mo;
	} else {
		sm_types.remove(type);
	}
	return old;
}

QSettingsEditorModel::QSettingsEditorModel(QObject* parent, QSettingsCategory* cat): QAbstractItemModel(parent), m_cat(cat)
{
	static int sm_initialized = 0;
	if (!sm_initialized) {
		sm_initialized = 1;
		sm_types[QVariant::nameToType("QSize")] = &QSettingsEditorSize::staticMetaObject;
		sm_types[QVariant::nameToType("QSizeF")] = &QSettingsEditorSizeF::staticMetaObject;
		sm_types[QVariant::nameToType("QFont")] = &QSettingsEditorFont::staticMetaObject;
		sm_types[QVariant::nameToType("QPoint")] = &QSettingsEditorPoint::staticMetaObject;
		sm_types[QVariant::nameToType("QPointF")] = &QSettingsEditorPointF::staticMetaObject;
		sm_types[qMetaTypeId<QFilePath>()] = &QSettingsEditorFilePath::staticMetaObject;

		sm_autodetectTypes<<&QSettingsEditorStyle::staticMetaObject;
		sm_autodetectTypes<<&QSettingsEditorStyleSheet::staticMetaObject;
	}
	refreshProperties();
}

void QSettingsEditorModel::refreshProperties()
{
	/* CleanUp */
	for (int p = 0;p < m_properties.size();p++)
		delete m_properties[p];
	m_properties.clear();
	clearCreatedProperties();
	/* Create */
	if (m_cat) {
		foreach (QSettingsProperty b, m_cat->properties) {
			if (b.hidden)
				continue;
			const QMetaObject *mo;
			QSettingsEditorInterface *node = 0;
			QVariant val = m_cat->object->property(b.name.toLatin1().data());
			mo = sm_types[val.userType()];
			if (mo) {
				QObject *qo;
				qo = mo->newInstance(Q_ARG(QObject*, this), Q_ARG(QObject*, b.object), Q_ARG(QString, b.name), Q_ARG(QObject*, this));
				if (qo) {
					node = qobject_cast<QSettingsEditorInterface*>(qo);
					if (!node)
						delete qo;
				}
			}
			if (!node) {
				/* Dummy create all types and try it */
				foreach (mo, sm_autodetectTypes) {
					if (mo) {
						QObject *qo;
						qo = mo->newInstance(Q_ARG(QObject*, this), Q_ARG(QObject*, b.object), Q_ARG(QString, b.name), Q_ARG(QObject*, this));
						if (qo) {
							node = qobject_cast<QSettingsEditorInterface*>(qo);
							if (!node)
								delete qo;
						}
					}
					if ((node) && (node->canHandle(b.name, val))) {
						break;
					} else {
						delete node;
						node = NULL;
					}
				}
			}
			if (!node) {
				node = new QSettingsEditorDummyType(this, b.object, QString(b.name), this);
			}
			node->setObjectDescription(b.display_name);
			node->setDefaultValue(b.defaultValue);
			node->setConnected(b.object != m_cat->object);
			//if (QObject::parent()) {
				//connect(node, SIGNAL(propertyChanged(QObject*,QString,QVariant,QVariant)), QObject::parent(), SIGNAL(propertyChanged(QObject*,QString,QVariant,QVariant)));
				//connect(node, SIGNAL(propertyChanged(QObject*,QString,QVariant,QVariant)), QObject::parent(), SLOT(slot_property_changed(QObject*,QString,QVariant,QVariant)));
			//}
			m_createdProperties.push_back(node);
			m_properties.push_back(node);
		}
	}
}

QModelIndex QSettingsEditorModel::index(int row, int column, const QModelIndex & parent) const
{
	if (parent.isValid()) {
		if (row >= reinterpret_cast<QSettingsEditorInterface*>(parent.internalPointer())->rowCount(parent))
			return QModelIndex();
		reinterpret_cast<QSettingsEditorInterface *>(parent.internalPointer())->property(row)->setParentIndex(parent);
		return createIndex(row, column, reinterpret_cast<QSettingsEditorInterface *>(parent.internalPointer())->property(row));
	}
	if (row >= m_properties.size())
		return QModelIndex();
	if (0 == column)
		m_properties[row]->setParentIndex(parent);
	return createIndex(row, column, m_properties[row]);
}

int QSettingsEditorModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return reinterpret_cast<QSettingsEditorInterface*>(parent.internalPointer())->rowCount(parent);
	return m_properties.size();
}

int QSettingsEditorModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return reinterpret_cast<QSettingsEditorInterface*>(parent.internalPointer())->columnCount(parent);
	return 2;
}


QVariant QSettingsEditorModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid() && ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
		return reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->data(index);
	if (Qt::SizeHintRole == role)
	    return QSize(0, QFontMetrics(QFont()).lineSpacing()+6);
	/*
	if (role == Qt::BackgroundColorRole) {
		if (index.row()%2) {
			return QColor(0xdee6ff);
		} else {
			return QColor(0xbfcfff);
		}
	}*/
	return QVariant();
}

bool QSettingsEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (Qt::EditRole != role)
		return false;
	bool ret = false;
	if (index.isValid() && ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
		ret = reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->setData(value, index);
	if (ret)
		emit dataChanged(index, index);
	return ret;
}


QModelIndex QSettingsEditorModel::parent(const QModelIndex & index) const
{
	if (index.isValid())
		return reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->parentIndex(index);
	return QModelIndex();
}


Qt::ItemFlags QSettingsEditorModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
		return reinterpret_cast<QSettingsEditorInterface*>(index.internalPointer())->flags(index);
	return Qt::NoItemFlags;
}

QVariant QSettingsEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	switch (section) {
		case 0:
			return tr("Property");
			case 1:
			return tr("Value");
			default:
			return QVariant();
	}
	return QVariant();
}

void QSettingsEditorModel::clearCreatedProperties()
{
	foreach (QPointer<QSettingsEditorInterface> prop, m_createdProperties)
		delete prop;
	m_createdProperties.clear();
}
