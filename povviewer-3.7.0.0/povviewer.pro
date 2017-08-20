#-------------------------------------------------
#
# Project created by slair 2015-10-03T11:45:20
#
#-------------------------------------------------

BINNAME = povviewer

BINPATH = bin
#FORMSPATH = forms
SRCPATH = src
RESPATH = res
BUILDPATH = build
OBJPATH = obj

CONFIG += debug_and_release

CONFIG -= qt

DEFINES += LCASE_RESOURCES

CONFIG(debug, debug|release) {
	#CONFIG += debug
	DEFINES += _DEBUG
	BUILDTARGET = debug
}

CONFIG(release, release|debug) {
	#CONFIG += release
	DEFINES += _RELEASE NDEBUG
	BUILDTARGET = release
}

MOC_DIR = $$BUILDPATH/$$BUILDTARGET/$$OBJPATH
UI_DIR = $$BUILDPATH/$$BUILDTARGET/$$OBJPATH
RCC_DIR = $$BUILDPATH/$$BUILDTARGET/$$OBJPATH
OBJECTS_DIR = $$BUILDPATH/$$BUILDTARGET/$$OBJPATH

#~ QT += core gui

#~ greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$BUILDPATH/$$BUILDTARGET/$$BINPATH/$$BINNAME
TEMPLATE = app

CONFIG += console
#~ QXT += core gui

SOURCES += $$SRCPATH/*.cpp

HEADERS += $$SRCPATH/*.hpp

#~ FORMS += $$FORMSPATH/mainwindow.ui \
	#~ $$FORMSPATH/edit_task_window.ui

#~ RESOURCES += $$RESPATH/$$BINNAME.qrc

LIBS += $$system(sdl-config  --libs)
message(output from sdl-config --libs added to LIBS =$$LIBS)

QMAKE_CXXFLAGS += $$system(sdl-config  --cflags)
message(output from sdl-config --cflags added to CXX_FLAGS= $$CXX_FLAGS)

CONFIG += link_pkgconfig
PKGCONFIG += gl
PKGCONFIG += glu
PKGCONFIG += x11

target.path = /usr/local/bin
#~ unix:target.files = $$TARGET
unix:target.extra = sudo cp $$TARGET /usr/local/bin
INSTALLS += target
