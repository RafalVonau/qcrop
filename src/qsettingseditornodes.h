#ifndef QSETTINGSEDITOR_P_H
#define QSETTINGSEDITOR_P_H

#include <QItemDelegate>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPointer>
#include <QDebug>
#include <QApplication>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QSize>
#include <QSizeF>
#include <QPainter>
#include <QFont>
#include <QPushButton>
#include <QFontDialog>
#include <QComboBox>
#include <QTreeView>
#include <QStyleFactory>
#include <QLineEdit>
#include "qsettingsmanager.h"
#include "qstylemanager.h"
#include "core.h"

/* nodes */
class QSettingsEditorDummyType: public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)
public:
	QSettingsEditorDummyType(QObject *parent , QObject * object, QString property, QObject *pm): QSettingsEditorInterface(parent, object, property, pm) {}
	~QSettingsEditorDummyType() {}
	virtual QWidget* createEditor(QWidget * parent, const QModelIndex & index) {Q_UNUSED(parent);Q_UNUSED(index);return 0;}
	virtual bool setEditorData(QWidget * editor, const QModelIndex & index) {Q_UNUSED(editor);Q_UNUSED(index);return false;}
	virtual bool paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) {Q_UNUSED(painter);Q_UNUSED(option);Q_UNUSED(index);return false;}
};

class QSettingsEditorSize : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

	Q_PROPERTY(int width READ width WRITE setWidth USER true)
	Q_PROPERTY(int height READ height WRITE setHeight USER true)

public:
	Q_INVOKABLE QSettingsEditorSize(QObject* parent, QObject* object, QString property, QObject *pm): QSettingsEditorInterface(parent, object, property, pm)
	{if (pm) refreshPropertis(qobject_cast<QSettingsEditorModel*>(pm));}

	virtual ~QSettingsEditorSize() {}
	QWidget* createEditor(QWidget *, const QModelIndex &) {return 0;}
	QVariant data(const QModelIndex & index)
	{
		switch (index.column()) {
			case 0:
				return QVariant(objectDescription());
			case 1:
				return QString("[%1,%2]").arg(value().toSize().width()).arg(value().toSize().height());
		}
		return QVariant();
	}
	int height() {return value().toSize().height();}
	void setHeight(int height) {QSize s = value().toSize();s.setHeight(height);setValue(s);}
	int width() {return value().toSize().width();}
	void setWidth(int width) {QSize s = value().toSize();s.setWidth(width);setValue(s);}
	Qt::ItemFlags flags(const QModelIndex &) {return Qt::ItemIsEnabled;}
};

class QSettingsEditorSizeF : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

	Q_PROPERTY(qreal width READ width WRITE setWidth USER true)
	Q_PROPERTY(qreal height READ height WRITE setHeight USER true)

public:
	Q_INVOKABLE QSettingsEditorSizeF(QObject* parent, QObject* object, QString property, QObject *pm): QSettingsEditorInterface(parent, object, property, pm)
	{if (pm) refreshPropertis(qobject_cast<QSettingsEditorModel*>(pm));}

	virtual ~QSettingsEditorSizeF() {}
	QWidget* createEditor(QWidget *, const QModelIndex &) {return 0;}
	QVariant data(const QModelIndex & index)
	{
		switch (index.column()) {
			case 0:
				return QVariant(objectDescription());
			case 1:
				return QString("[%1,%2]").arg(value().toSizeF().width()).arg(value().toSizeF().height());
		}
		return QVariant();
	}
	qreal height() {return value().toSizeF().height();}
	void setHeight(qreal height) {QSizeF s = value().toSizeF();s.setHeight(height);setValue(s);}
	qreal width() {return value().toSizeF().width();}
	void setWidth(qreal width) {QSizeF s = value().toSize();s.setWidth(width);setValue(s);}
	Qt::ItemFlags flags(const QModelIndex &/*index*/) {return Qt::ItemIsEnabled;}
};

class QSettingsEditorPoint : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

	Q_PROPERTY(int x READ x WRITE setX USER true)
	Q_PROPERTY(int y READ y WRITE setY USER true)

public:
	Q_INVOKABLE QSettingsEditorPoint(QObject* parent, QObject* object, QString property, QObject *pm): QSettingsEditorInterface(parent, object, property, pm)
	{if (pm) refreshPropertis(qobject_cast<QSettingsEditorModel*>(pm));}

	virtual ~QSettingsEditorPoint() {}
	QVariant data(const QModelIndex & index)
	{
		switch (index.column()) {
			case 0:
				return QVariant(objectDescription());
			case 1:
				return QString("[%1,%2]").arg(value().toPoint().x()).arg(value().toPoint().y());
		}
		return QVariant();
	}
	int x() {return value().toPoint().x();}
	void setX(int x) {QPoint s = value().toPoint();s.setX(x);setValue(s);}
	int y() {return value().toPoint().y();}
	void setY(int y) {QPoint s = value().toPoint();s.setY(y);setValue(s);}
	Qt::ItemFlags flags(const QModelIndex &) {return Qt::ItemIsEnabled;}
};

class QSettingsEditorPointF : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

	Q_PROPERTY(qreal x READ x WRITE setX USER true)
	Q_PROPERTY(qreal y READ y WRITE setY USER true)

public:
	Q_INVOKABLE QSettingsEditorPointF(QObject* parent, QObject* object, QString property, QObject *pm): QSettingsEditorInterface(parent, object, property, pm)
	{if (pm) refreshPropertis(qobject_cast<QSettingsEditorModel*>(pm));}

	virtual ~QSettingsEditorPointF() {}
	QVariant data(const QModelIndex & index)
	{
		switch (index.column()) {
			case 0:
				return QVariant(objectDescription());
			case 1:
				return QString("[%1,%2]").arg(value().toPointF().x()).arg(value().toPointF().y());
		}
		return QVariant();
	}
	qreal x() {return value().toPointF().x();}
	void setX(qreal x) {QPointF s = value().toPointF();s.setX(x);setValue(s);}
	qreal y() {return value().toPointF().y();}
	void setY(qreal y) {QPointF s = value().toPointF();s.setY(y);setValue(s);}
	Qt::ItemFlags flags(const QModelIndex &) {return Qt::ItemIsEnabled;}
};

class ChangeFont : public QPushButton
{
	Q_OBJECT
public:
	ChangeFont(QWidget *parent = 0): QPushButton(parent)
	{
		connect(this, SIGNAL(pressed()), this, SLOT(onClick()));
		setText(tr("Change font"));
	}

	void setFont(QFont font) {m_font = font;}
	QFont font() {return m_font;}

public slots:
	void onClick()
	{
		bool ok;
		QFont font = QFontDialog::getFont(&ok, m_font, this, tr("Choose a font"));

		if (ok){
			m_font = font;
			emit(fontChanged(m_font));
		}
	}

signals:
	void fontChanged(const QFont & font);

private:
	QFont m_font;
};

class QSettingsEditorFont : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

public:
	Q_INVOKABLE QSettingsEditorFont(QObject* parent, QObject* object, QString property, QObject *propertyModel);
	virtual ~QSettingsEditorFont() {}

	QWidget* createEditor(QWidget * parent, const QModelIndex & index)
	{
		Q_UNUSED(index);
		ChangeFont *cf = new ChangeFont(parent);
		connect(cf, SIGNAL(fontChanged(const QFont&)), this, SLOT(setValue(const QFont&)));
		return cf;
	}

	bool setEditorData(QWidget * editor, const QModelIndex &)
	{
		QVariant f;
		dynamic_cast<ChangeFont *>(editor)->setFont(value().value<QFont>());
		return true;
	}
	bool paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) {return false;}
public slots:
	void setValue(const QFont& f)
	{
		QSettingsEditorInterface::setValue(f);
	}
private:
	QFont m_font;
};

class QGeneralCombo : public QComboBox
{
	Q_OBJECT
public:
	QGeneralCombo(QStringList items, bool ed, QWidget* parent = 0): QComboBox(parent)
	{
		clear();
		setEditable(ed);
		foreach (QString i, items) {
			addItem(i);
			setItemData(count() - 1, i);
		}


	}
	virtual ~QGeneralCombo() {}

	QString itm() const {return currentText();}
	void setItm(QString c)
	{
		setCurrentIndex(findText(c));
		if (currentIndex() == -1) {
			addItem(c);
			setCurrentIndex(count() - 1);
		}
	}
private slots:

private:

};


class QSettingsEditorStyle : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

public:
	Q_INVOKABLE QSettingsEditorStyle(QObject* parent, QObject* object, QString property, QObject *propertyModel): QSettingsEditorInterface(parent, object, property, propertyModel) {}
	virtual ~QSettingsEditorStyle() {}

	QWidget* createEditor(QWidget * parent, const QModelIndex & index)
	{
		Q_UNUSED(index);
		QGeneralCombo *cb = new QGeneralCombo(QStyleFactory::keys(), false, parent);
		connect(cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
		return cb;
	}

	bool setEditorData(QWidget * editor, const QModelIndex &)
	{
		QVariant f = value();
		dynamic_cast<QGeneralCombo *>(editor)->setItm(f.toString());
		return true;
	}
	bool setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
	{
		QString st = dynamic_cast<QGeneralCombo *>(editor)->itm();
		model->setData(index, st);
		return true;
	}
	bool canHandle(const QString &name, const QVariant &value) {Q_UNUSED(value); return (name == "style");}
public slots:
	void setValue(QString s) {QSettingsEditorInterface::setValue(QVariant(s));}
};

class QSettingsEditorStyleSheet : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

public:
	Q_INVOKABLE QSettingsEditorStyleSheet(QObject* parent, QObject* object, QString property, QObject *propertyModel): QSettingsEditorInterface(parent, object, property, propertyModel) {}
	virtual ~QSettingsEditorStyleSheet() {}

	QWidget* createEditor(QWidget * parent, const QModelIndex & index)
	{
		Q_UNUSED(index);
		QGeneralCombo *cb = new QGeneralCombo(Core::styleManager()->steleSheets(), false, parent);
		connect(cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
		return cb;
	}

	bool setEditorData(QWidget * editor, const QModelIndex &)
	{
		dynamic_cast<QGeneralCombo *>(editor)->setItm(value().toString());
		return true;
	}
	bool setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
	{
		QString st = dynamic_cast<QGeneralCombo *>(editor)->itm();
		model->setData(index, st);
		return true;
	}
	bool canHandle(const QString &name, const QVariant &value) {Q_UNUSED(value); return (name == "styleSheet");}
public slots:
	void setValue(QString s) {QSettingsEditorInterface::setValue(QVariant(s));}
};

class FileEdit : public QWidget
{
    Q_OBJECT
public:
    FileEdit(QWidget *parent = 0);
    void setFilePath(const QString &filePath) { if (theLineEdit->text() != filePath) theLineEdit->setText(filePath); }
    QString filePath() const { return theLineEdit->text(); }
    void setFilter(const QString &filter) { theFilter = filter; }
    QString filter() const { return theFilter; }
signals:
    void filePathChanged(const QString &filePath);
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
private slots:
    void buttonClicked();
private:
    QLineEdit *theLineEdit;
    QString theFilter;
};

class QSettingsEditorFilePath : public QSettingsEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(QSettingsEditorInterface)

public:
	Q_INVOKABLE QSettingsEditorFilePath(QObject* parent, QObject* object, QString property, QObject *propertyModel): QSettingsEditorInterface(parent, object, property, propertyModel) {}
	virtual ~QSettingsEditorFilePath() {}

	QWidget* createEditor(QWidget * parent, const QModelIndex & index)
	{
		Q_UNUSED(index);
		FileEdit *cb = new FileEdit(parent);
		connect(cb, SIGNAL(filePathChanged(QString)), this, SLOT(setValue(QString)));
		return cb;
	}

	bool setEditorData(QWidget * editor, const QModelIndex &)
	{
		QVariant f = value();
		if (f.userType() == qMetaTypeId<QFilePath>()) {
			dynamic_cast<FileEdit *>(editor)->setFilePath(f.value<QFilePath>().path());
			dynamic_cast<FileEdit *>(editor)->setFilter(f.value<QFilePath>().filter());
		} else {
			dynamic_cast<FileEdit *>(editor)->setFilePath(f.toString());
		}
		return true;
	}
	QVariant data(const QModelIndex & index)
	{
		switch (index.column()) {
			case 0:
				return QVariant(objectDescription());
			case 1:
				QVariant f = value();
				if (f.userType() == qMetaTypeId<QFilePath>()) {
					return f.value<QFilePath>().path();
				} else {
					return f.toString();
				}
		}
		return QVariant();
	}
	bool setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
	{
		QFilePath ct;
		QString st = dynamic_cast<FileEdit *>(editor)->filePath();
		ct.setPath(st);
		model->setData(index, qVariantFromValue(ct));
		return true;
	}
public slots:
	void setValue(const QString  &s)
	{
		QFilePath ct;
		ct.setPath(s);
		QSettingsEditorInterface::setValue(qVariantFromValue(ct));
	}
};


#endif // QSETTINGSEDITOR_P_H
