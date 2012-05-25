# -------------------------------------------------
# Project created by QtCreator 2009-11-17T20:41:10
# -------------------------------------------------
QT += network
TARGET = upcoder
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    tablemodel.cpp \
    uploader.cpp \
    qupfile.cpp \
    global.cpp \
    resultdialog.cpp
HEADERS += mainwindow.h \
    settingsdialog.h \
    tablemodel.h \
    uploader.h \
    qupfile.h \
    global.h \
    resultdialog.h
FORMS += mainwindow.ui \
    settingsdialog.ui \
    resultdialog.ui
RESOURCES += upcoder.qrc
TRANSLATIONS += upcoder_ru.ts
win32:RC_FILE = icon.rc
OTHER_FILES += upcoder_ru.ts \
    upcoder_ru.qm \
    qt_ru.ts \
    qt_ru.qm \
    icon.rc
