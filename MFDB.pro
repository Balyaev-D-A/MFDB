QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

CONFIG += c++11 axcontainer


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectionform.cpp \
    csvreader.cpp \
    database.cpp \
    defectform.cpp \
    defectstable.cpp \
    deviceselectorform.cpp \
    dictionaryform.cpp \
    dragdropedit.cpp \
    dragdroplist.cpp \
    dragdroptable.cpp \
    dragdroptree.cpp \
    fieldeditor.cpp \
    headercombobox.cpp \
    importform.cpp \
    jsonobjectbuilder.cpp \
    krform.cpp \
    krreportform.cpp \
    krreportsform.cpp \
    krselectorform.cpp \
    main.cpp \
    mainwindow.cpp \
    materialsreportform.cpp \
    movesform.cpp \
    normativeform.cpp \
    progressform.cpp \
    raspform.cpp \
    receiptform.cpp \
    receiptsform.cpp \
    searchtable.cpp \
    settings.cpp \
    settingsform.cpp \
    stringlistvalidator.cpp \
    tablesearchwidget.cpp \
    trreportform.cpp \
    trreportsform.cpp \
    verifyform.cpp \
    worktypewidget.cpp \
    wtbutton.cpp \
    wtcheckbox.cpp

HEADERS += \
    connectionform.h \
    csvreader.h \
    database.h \
    defectform.h \
    defectstable.h \
    deviceselectorform.h \
    dictionaryform.h \
    dragdropedit.h \
    dragdroplist.h \
    dragdroptable.h \
    dragdroptree.h \
    fieldeditor.h \
    headercombobox.h \
    importform.h \
    jsonobjectbuilder.h \
    krform.h \
    krreportform.h \
    krreportsform.h \
    krselectorform.h \
    mainwindow.h \
    materialsreportform.h \
    movesform.h \
    normativeform.h \
    progressform.h \
    raspform.h \
    receiptform.h \
    receiptsform.h \
    searchtable.h \
    settings.h \
    settingsform.h \
    stringlistvalidator.h \
    tablesearchwidget.h \
    trreportform.h \
    trreportsform.h \
    verifyform.h \
    worktypewidget.h \
    wtbutton.h \
    wtcheckbox.h

FORMS += \
    connectionform.ui \
    defectform.ui \
    deviceselectorform.ui \
    dictionaryform.ui \
    importform.ui \
    krform.ui \
    krreportform.ui \
    krreportsform.ui \
    krselectorform.ui \
    mainwindow.ui \
    materialsreportform.ui \
    movesform.ui \
    normativeform.ui \
    progressform.ui \
    raspform.ui \
    receiptform.ui \
    receiptsform.ui \
    settingsform.ui \
    tablesearchwidget.ui \
    trreportform.ui \
    trreportsform.ui \
    verifyform.ui \
    worktypewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
