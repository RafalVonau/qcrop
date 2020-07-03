#ifndef QPHOTODB_H
#define QPHOTODB_H

#include <QObject>
#include <QtSql>
#include <QPointer>
#include <QList>
#include <QFutureWatcher>
#include <QProgressBar>
#include "qphotomodel.h"

class QPhotoDB : public QObject  {
	Q_OBJECT

	public:

		typedef QFutureWatcher<void> fwvoid_t;

		QPhotoDB(QObject *parent = 0);
		~QPhotoDB();

		QSqlError createDB(QString name);
		void loadDirectiory(QString dirname);
		void populate();
		QPhoto *getPhoto(QModelIndex i);

		void updateDB(QPointer<QPhoto> p);

		QPhotoModel *model() {return m_model;}
		QPhotoModel *m_model;

		void startCrop(QString dir, QProgressBar *pb);
	public slots:
		void conversionReady();

	signals:
		void cropReady();

	private:
		void newPhoto(QString path);
		QList<QPhoto *> m_photos;
		fwvoid_t *m_conversionWatcher;
		QString m_dir;
};

#endif // QPHOTODB_H
