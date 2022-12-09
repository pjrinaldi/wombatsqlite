QT += widgets core
linux:CONFIG += debug_and_release debug_and_release_target qt x11 build_all c++14 
#INCLUDEPATH += /usr/local/lib/
INCLUDEPATH += /usr/local/include/
#VPATH += /usr/local/lib/
#VPATH += /usr/local/include/
FORMS = wombatsqlite.ui #tagmanager.ui htmlviewer.ui aboutbox.ui
HEADERS = wombatsqlite.h #tagmanager.h htmlviewer.h aboutbox.h cssstrings.h
SOURCES = main.cpp wombatsqlite.cpp #tagmanager.cpp htmlviewer.cpp aboutbox.cpp cssstrings.cpp
RESOURCES += wombatsqlite.qrc
release: DESTDIR = release
debug:   DESTDIR = debug
#linux:LIBS = libregf.a

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
}
#linux:
#install
target.path = /usr/local/bin
#target.path = ./
INSTALLS += target
