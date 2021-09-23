QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

CONFIG += c++11 axcontainer console


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    comboheader.cpp \
    csvreader.cpp \
    database.cpp \
    dictionaryform.cpp \
    fieldeditor.cpp \
    importform.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    comboheader.h \
    csvreader.h \
    database.h \
    dictionaryform.h \
    fieldeditor.h \
    importform.h \
    mainwindow.h \
    settings.h

FORMS += \
    dictionaryform.ui \
    importform.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
