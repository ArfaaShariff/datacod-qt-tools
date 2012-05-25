# -------------------------------------------------
# Project created by QtCreator 2009-11-04T16:09:56
# -------------------------------------------------
QT += network
TARGET = downcoder
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    addlistdialog.cpp \
    settingsdialog.cpp \
    urlextracter.cpp \
    downloader.cpp \
    tablemodel.cpp \
    global.cpp
HEADERS += mainwindow.h \
    addlistdialog.h \
    settingsdialog.h \
    urlextracter.h \
    downloader.h \
    global.h \
    global.h \
    tablemodel.h
FORMS += mainwindow.ui \
    addlistdialog.ui \
    settingsdialog.ui
RESOURCES += resources.qrc
win32:RC_FILE = icon.rc
OTHER_FILES += icon.rc \
    downcoder_ru.ts \
    downcoder_ru.qm \
    qt_ru.ts \
    qt_ru.qm
TRANSLATIONS = downcoder_ru.ts
