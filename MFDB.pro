QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql printsupport webenginewidgets

CONFIG += c++11 axcontainer console


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
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
    krform.cpp \
    krreportform.cpp \
    krreportpreviewform.cpp \
    krreportsform.cpp \
    krselectorform.cpp \
    main.cpp \
    mainwindow.cpp \
    movesform.cpp \
    normativeform.cpp \
    progressform.cpp \
    raspform.cpp \
    raspprintpreviewform.cpp \
    receiptform.cpp \
    receiptsform.cpp \
    settings.cpp \
    trreportform.cpp \
    trreportsform.cpp \
    verifyform.cpp \
    worktypewidget.cpp \
    wtbutton.cpp \
    wtcheckbox.cpp

HEADERS += \
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
    krform.h \
    krreportform.h \
    krreportpreviewform.h \
    krreportsform.h \
    krselectorform.h \
    mainwindow.h \
    movesform.h \
    normativeform.h \
    progressform.h \
    raspform.h \
    raspprintpreviewform.h \
    receiptform.h \
    receiptsform.h \
    settings.h \
    trreportform.h \
    trreportsform.h \
    verifyform.h \
    worktypewidget.h \
    wtbutton.h \
    wtcheckbox.h

FORMS += \
    defectform.ui \
    deviceselectorform.ui \
    dictionaryform.ui \
    importform.ui \
    krform.ui \
    krreportform.ui \
    krreportpreviewform.ui \
    krreportsform.ui \
    krselectorform.ui \
    mainwindow.ui \
    movesform.ui \
    normativeform.ui \
    progressform.ui \
    raspform.ui \
    raspprintpreviewform.ui \
    receiptform.ui \
    receiptsform.ui \
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
