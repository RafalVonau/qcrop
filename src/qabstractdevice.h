#ifndef QABSTRACTDEVICE_H
#define QABSTRACTDEVICE_H
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QCompleter>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QScriptEngine>


class QAbstractDevice: public QObject
{
	Q_OBJECT;

public:
	Q_INVOKABLE QAbstractDevice(QString name, QString xmlFile = QString(), QObject * parent = 0) : QObject(parent) {Q_UNUSED(name);Q_UNUSED(xmlFile)}
	virtual ~QAbstractDevice() {}

	virtual bool deviceOpen(QString path) {Q_UNUSED(path);return false;}
	virtual QString getName() {return "unnamed";}
public slots:
	virtual void doCommand(QString command) {Q_UNUSED(command);}
	virtual void writeRegister(QString name, QVariant value) {Q_UNUSED(name);Q_UNUSED(value);}
	virtual void fromFile(QString fileName) {Q_UNUSED(fileName);}
	virtual void toFile(QString fileName, bool append) {Q_UNUSED(fileName);Q_UNUSED(append);}
	virtual void showBase() {}

signals:
	void logText(const QString);
	//void registerChanged(QString path, QVariant value);
};
//Q_DECLARE_METATYPE(QAbstractDevice);

#endif // QABSTRACTDEVICE_H
