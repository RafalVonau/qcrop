/****************************************************************************
**
** Copyright (C) 2006-2007 ELFIN <rafal.vonau@elfin-pe.pl>
**
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QSETTINGSMANAGER_H_
#define _QSETTINGSMANAGER_H_

#include <QtGui>
#include <QObject>
#include <QMainWindow>
#include <QSettings>
#include <QHash>
#include <QVariant>
#include <QString>
#include <QPointer>
#include <QDebug>
#include "core-config.h"

#define NopForTr(x)
class QSettingsEditorInterface;

class QFilePath
{
public:
	QFilePath() {}
	QFilePath(QString path) {setPath(path);}
	void setPath(QString p) {m_path = p;}
	QString path() const {return m_path;}
	void setFilter(QString f) {m_filter = f;}
	QString filter() const {return m_filter;}

private:
	QString m_path;
	QString m_filter;
};
Q_DECLARE_METATYPE(QFilePath)

QDataStream &operator<<(QDataStream &out, const QFilePath &myObj);
QDataStream &operator>>(QDataStream &in, QFilePath &myObj);


/*!
 * \brief Single property description.
 */
struct QSettingsProperty {
	bool              hidden;                      /* is hidden ?                       */
	QString           display_name;                /* display name                      */
	QPointer<QObject> object;                      /* pointer to holding QObject        */
	QString           name;                        /* property name                     */
	QVariant          defaultValue;                /* default Value                     */
};

/*!
 * \brief Single category description.
 */
struct QSettingsCategory
{
	QPointer<QObject> object;                      /* QObject for non-object properties */
	QString           display_name;                /* Category display name             */
	QString           name;                        /* Category name                     */
	QHash<QString, QSettingsProperty> properties;  /* Properties database               */
};

class QSettingsWidget;

/*!
 * \brief Settings manager.
 */
class CORE_EXPORT QSettingsManager : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("author", "Rafal Vonau")
public:

	static QSettingsManager *instance() {
		static QPointer<QSettingsManager> the_instance;
		if (!the_instance) {
			qRegisterMetaType<QFilePath>("QFilePath");
			qRegisterMetaTypeStreamOperators<QFilePath>("QFilePath");
			the_instance = new QSettingsManager();
		}
		return the_instance;
	}
	virtual ~QSettingsManager();

	void cleanup() {
		saveSettings();
	}


	QVariant property(const QString &category, const QString &name, const QVariant &def = QVariant());
	void setProperty(const QString &category, const QString &name, const QVariant &value);
	void setDisplayName(const QString &category, const QString &name, const QString &displayName);
	void setCategoryDisplayname(const QString &category, const QString &displayName);
	void setHidden(const QString &category, const QString &name, const bool &h);
	QStringList propertyNames(const QString &category);
	QStringList categoryNames();
	void restoreSettings();
	void saveSettings();

	void addObjectProperty(QString category_name,QString display_name,QObject *object,QString name);
	void storeObjectProperty(QString category_name,QString display_name,QObject *obj,QString name);
	QList<QSettingsProperty> objectProperties(const QString &category) {
		return m_categories[category].properties.values();
	}
	QList<QSettingsCategory> objectCategories() {
		return m_categories.values();
	}

	QSettingsCategory getCategory(QString name) {
		return m_categories[name];
	}

	void saveWindowState(QString name, QMainWindow *o);
	void loadWindowState(QString name, QMainWindow *o);

	void loadWidgetState(QString name, QWidget *o);
	void saveWidgetState(QString name, QWidget *o);

	void installMenu(QMenu *parent);

public slots:
	void showConfigurationDialog();


private:
	void generateCatDisplayName(QSettingsCategory &, QObject *object = 0);

	QSettingsManager(QObject *parent = 0);
	QHash<QString, QSettingsCategory>   m_categories;
	QSettingsWidget                    *m_view;
};

/*!
 * \brief Settings category model.
 */
class QSettingsEditorModel : public QAbstractItemModel
{
	Q_OBJECT
	Q_CLASSINFO("author", "Rafal Vonau")

public:
	QSettingsEditorModel(QObject* parent = 0, QSettingsCategory* cat = 0);
	~QSettingsEditorModel() {clearCreatedProperties();}

	void setCategory(QSettingsCategory *cat) {
		beginResetModel();
		m_cat = cat;
		refreshProperties();
		endResetModel();
	}
	void refreshProperties();
	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QModelIndex parent(const QModelIndex & index) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QSettingsEditorInterface* createPropery(QObject* object, QString property, QString description = QString());
	void resetModel() {beginResetModel();endResetModel();}
	static const QMetaObject *registerNode(int type, const QMetaObject *mo);

public slots:

private:
	void clearCreatedProperties();

private:
	QSettingsCategory* m_cat;
	QList<QSettingsEditorInterface*> m_properties;
	mutable QList< QPointer<QSettingsEditorInterface> >  m_createdProperties;
	static QHash<int, const QMetaObject*> sm_types;
	static QList<const QMetaObject*>      sm_autodetectTypes;
};


#endif // _QSETTINGSMANAGER_H_
