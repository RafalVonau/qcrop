#ifndef QPHOTO_H
#define QPHOTO_H

#include <QPixmap>
#include <QString>
#include <QObject>
#include <QSize>
#include <QString>
#include <QRectF>
#include <QFutureWatcher>
#include <QVariant>
#include <QMap>
#include <QtConcurrent>

class QPhoto;

class QPhoto: public QObject
{
	Q_OBJECT
	Q_CLASSINFO("author", "Rafal Vonau")

	public:
		typedef QFutureWatcher<QImage> fw_t;

		QPhoto(QPhoto *parent = 0);
		QPhoto(QPhoto *parent, QString path, QSize s, QRectF cr, QString attr, QPixmap icon);
		~QPhoto();

		/* Standard item access */
		QPhoto *child(int number);
		void appendChild(QPhoto *child);
		int childCount() const;
		int columnCount() const;
		bool insertChildren(int position, int count, int columns);
		bool insertColumns(int position, int columns);
		QPhoto *parent();
		bool removeChildren(int position, int count);
		bool removeColumns(int position, int columns);
		int childNumber() const;
		virtual bool setData(int column, const QVariant &value);
		virtual QVariant getRole(int column, int role) {
				if ((role == Qt::DecorationRole) && (column == 0)) {
					return m_icon;
				}
				return m_role[role];
		}
		virtual void setRole(int role, QVariant v) {m_role[role] = v;}
		virtual void removeRole(int role) {m_role.remove(role);}
		virtual QVariant data(int column) const;

		/* helpers */

		void setPath(QString path) {m_filePath = path;}
		QString getPath() {return m_filePath;}
                QString getName() {return m_fileName;}

		void setSize(QSize s) {m_photoSize = s;}
		QSize size() {return m_photoSize;}

		void setCropRegion(QRectF cr) {m_cropRegion = cr;}
		QRectF cropRegion() {return m_cropRegion;}

		void setAttributes(QString s) {m_attr = s;}
		QString getAttributes() {return m_attr;}

		QPixmap &toIcon() {return m_icon;}
		void toPixmap();
		void preload();

		void crop();

		void setCropFilePath(QString p) {m_cropFilePath = p;}
		QString cropFilePath() const {return m_cropFilePath;}

	public slots:
		void photoLoaded();
		void iconGenerated();


	signals:
		void photoReady(QPixmap &);

	public:
		QPixmap                 m_icon;                 /*!< Icon             */
		QPhoto                 *m_parentItem;
		QMap<int, QVariant>     m_role;
		QList<QPhoto*>          m_childItems;


	private:
		QString  m_filePath;             /*!< File path        */
                QString  m_fileName;             /*!< File name        */
		QString  m_cropFilePath;
		QSize    m_photoSize;            /*!< Photo size       */
		QRectF   m_cropRegion;           /*!< Crop region      */
		QString  m_attr;                 /*!< Atributes        */

		fw_t    *m_photoLoadWatcher;
		fw_t    *m_iconGenerateWatcher;
};

#endif // QPHOTO_H
