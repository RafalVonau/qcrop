#ifndef QCROP_H
#define QCROP_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include <QItemDelegate>
#include <QAbstractItemModel>
#include <QPointer>
#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>
#include <QProgressBar>
#include <QVector>
#include <QMap>
#include <QTextEdit>
#include <QLabel>
#include <QSystemTrayIcon>
#include "core.h"
#include <QtSql>
#include <QtScript>
#include <QProgressBar>
#include <QTimer>
#include <QFileDialog>
#include "ui_qcrop.h"
#include "qphotodb.h"
#include "qphotoscene.h"
#include "qphotodelegate.h"

class QCrop : public QMainWindowInterface
{
	Q_OBJECT
	Q_ENUMS(SelType)
public:
	enum SelType {None, Move, ResizeD0, ResizeD1, ResizeD2, ResizeD3};
	Q_INVOKABLE QCrop(QWidget * parent = 0, Qt::WindowFlags flags = 0);
	virtual ~QCrop();


public slots:
	void newFile();
	void openFile();
	void openDatabase(QString path);
	void showPhoto(QModelIndex);
	void fitImage(QSize &);
	void startConversion();
	void cropReady();
	void loadLastDir();

protected:
//        void closeEvent(QCloseEvent *event);
private:
	void showError(const QSqlError &err);

	Ui::QCrop                            ui;
	QPhotoScene                          m_scene;
	QPointer<QPhotoDB>                   m_photoDB;
	QPhotoDelegate                      *m_delegate;
	QProgressBar                        *m_progressBar;
	QString                              m_workingDirectory;
	QTimer                               m_timer;
	QSettingsManager                    *m_settings;
};


#endif // QCROP_H
