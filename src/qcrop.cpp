#include <QMessageBox>
#include <QImageReader>
#include "qcrop.h"




QCrop::QCrop(QWidget * parent, Qt::WindowFlags flags) : QMainWindowInterface(parent, flags)
{
	ui.setupUi(this);
	setObjectName("qcrop");
	setWindowTitle("QCrop v"+qApp->property("version").toString());

	if (!QSqlDatabase::drivers().contains("QSQLITE")) {
		QMessageBox::critical(this, "Unable to load database", "This program needs the SQLITE driver");
	}
	// initialize the database
	m_photoDB = NULL;

	m_scene.setItemIndexMethod(QGraphicsScene::NoIndex);
	ui.graphicsView->setScene(&m_scene);
	ui.graphicsView->setCacheMode(QGraphicsView::CacheBackground);
	ui.graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

	// Set the model
	m_delegate = new QPhotoDelegate(ui.images);
	ui.images->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.images->setItemDelegate(m_delegate);

	/* Connections */
	connect(&m_scene, SIGNAL(fitImage(QSize &)), this, SLOT(fitImage(QSize&)));
	connect(ui.images, SIGNAL(clicked(QModelIndex)), this, SLOT(showPhoto(QModelIndex)));
	connect(ui.actionE_xit, SIGNAL(activated()), this, SLOT(close()));
	connect(ui.actionStartConversion, SIGNAL(activated()), this, SLOT(startConversion()));
	connect(ui.action_New, SIGNAL(activated()), this, SLOT(newFile()));
	connect(ui.action_Open, SIGNAL(activated()), this, SLOT(openFile()));

        m_settings = QSettingsManager::instance();
        m_settings->loadWindowState("qcrop", this);
        m_progressBar = NULL;
        loadLastDir();
}
//=====================================================================================================================

void QCrop::loadLastDir()
{
    /* Settings */

    m_settings->property(QString("display"),QString("imgSize"),QSize(640,480));
    m_settings->property(QString("display"),QString("iconSize"),QSize(64,64));
    m_settings->property(QString("display"),QString("cacheSize"), 100);
    m_settings->property(QString("crop"),QString("border"),5);
    m_settings->property(QString("crop"),QString("borderColor"),QColor(Qt::white));
    m_settings->property(QString("crop"),QString("photoSize"),QSize(1795,1205));
    //settings->property(QString("state"),QString("lastDirectory"), QString("./"));
    //settings->saveSettings();
    //settings->restoreSettings();
    openDatabase(m_settings->property(QString("state"),QString("lastDirectory"), QString(".")).toString()+"/qcrop.db");
}
//=====================================================================================================================

void QCrop::openDatabase(QString path)
{
    QFileInfo fi ( path );

    if ( m_photoDB ) {
	delete m_photoDB;
    }
    m_photoDB = new QPhotoDB(this);
    QSqlError err = m_photoDB->createDB ( path );
    if (err.type() != QSqlError::NoError) {
	    showError(err);
	    return;
    }
    m_workingDirectory = fi.absoluteDir().absolutePath();    
    m_scene.setPhotoDB ( m_photoDB );
    m_delegate->setModel ( m_photoDB->model() );
    ui.images->setModel ( m_photoDB->model() );
    connect(m_photoDB, SIGNAL(cropReady()), this, SLOT(cropReady()));
    qDebug()<<fi.dir().absolutePath();
    m_photoDB->loadDirectiory( m_workingDirectory + '/' );
    m_settings->setProperty(QString("state"),QString("lastDirectory"), QVariant(m_workingDirectory));
    //QSettingsManager::instance()->saveSettings();
}
//=====================================================================================================================

void QCrop::newFile()
{
    QString fileName;
    fileName = QFileDialog::getExistingDirectory(this, tr("Select directory"));

    if (!fileName.isEmpty()) {
	openDatabase(fileName + "/qcrop.db");
    }

}
//=====================================================================================================================

void QCrop::openFile()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Select database"), "qcrop.db", "*.db");

    if (!fileName.isEmpty()) {
	openDatabase(fileName);
    }
}
//=====================================================================================================================

void QCrop::showPhoto(QModelIndex i)
{
	QPointer<QPhoto> p;
	QSize s;
	QModelIndex j;

	p = m_photoDB->getPhoto(i);
	if (!p)
		return;
	m_scene.setPhoto(p);
	/* Preload next image */
	QPhoto *n;
	j = i.sibling(i.row() + 1, i.column());
	if (j.isValid()) {
		n = m_photoDB->getPhoto(j);
		if (!n->getPath().isEmpty()) {
			n->preload();
		}
	}
}
//=====================================================================================================================

void QCrop::startConversion()
{
	if (!m_progressBar) {
		m_progressBar = new QProgressBar(ui.statusbar);
		m_progressBar->setMaximumHeight(16);
		m_progressBar->setMaximumWidth(width() - 200);
		m_progressBar->setTextVisible(false);
		m_progressBar->setValue(0);
		m_progressBar->setMinimum(0);
		m_progressBar->setMaximum(100);
	}
	ui.statusbar->addPermanentWidget(m_progressBar, 0);
	ui.statusbar->showMessage(QString("Processing:"));
	m_photoDB->startCrop(m_workingDirectory+"/generated", m_progressBar);
}
//=====================================================================================================================

void QCrop::cropReady()
{
	ui.statusbar->removeWidget(m_progressBar);
	m_progressBar->deleteLater();
	m_progressBar = NULL;
	ui.statusbar->showMessage("Reday",2000);
}
//=====================================================================================================================

void QCrop::fitImage(QSize &s)
{
	ui.graphicsView->fitInView(QRectF(0,0,s.width(),s.height()), Qt::KeepAspectRatio);
}
//=====================================================================================================================

QCrop::~QCrop()
{
    m_settings->saveSettings();
}
//=====================================================================================================================

void QCrop::showError(const QSqlError &err)
{
	QMessageBox::critical(this, "Unable to initialize Database",
					"Error initializing database: " + err.text());
}
//=====================================================================================================================
