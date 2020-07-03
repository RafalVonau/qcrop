TEMPLATE = subdirs

CONFIG -= static

SUBDIRS =	src/qcrop.pro

win32:RC_FILE = winicon.rc
include(install.pri)
