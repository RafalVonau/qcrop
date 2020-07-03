#include "core.h"
#include "qcrop.h"
#include <QtGui>
#include <QDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QDir>

int main(int argc, char * argv[])
{
	//QStringList liblist;
	//liblist << QDir::currentPath();
	//QCoreApplication::setLibraryPaths(liblist);
	Core c(&QCrop::staticMetaObject);
	//Core::setUploadURL("http://rab.ict.pwr.wroc.pl/~rvonau/download/");
	//Core::setSingleApp(true);
	return c.run("qcrop", argc, argv);
}
