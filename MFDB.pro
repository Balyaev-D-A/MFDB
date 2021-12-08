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
    dragdroplist.cpp \
    dragdroptable.cpp \
    dragdroptree.cpp \
    fieldeditor.cpp \
    headercombobox.cpp \
    importform.cpp \
    main.cpp \
    mainwindow.cpp \
    progressform.cpp \
    raspform.cpp \
    raspprintpreviewform.cpp \
    settings.cpp \
    worktypewidget.cpp \
    wtbutton.cpp \
    wtcheckbox.cpp

HEADERS += \
    comboheader.h \
    csvreader.h \
    database.h \
    dictionaryform.h \
    dragdroplist.h \
    dragdroptable.h \
    dragdroptree.h \
    fieldeditor.h \
    headercombobox.h \
    importform.h \
    mainwindow.h \
    progressform.h \
    raspform.h \
    raspprintpreviewform.h \
    settings.h \
    worktypewidget.h \
    wtbutton.h \
    wtcheckbox.h

FORMS += \
    dictionaryform.ui \
    importform.ui \
    mainwindow.ui \
    progressform.ui \
    raspform.ui \
    raspprintpreviewform.ui \
    worktypewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
