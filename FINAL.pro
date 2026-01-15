QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    activitymodel.cpp \
    adminwidget.cpp \
    csvexporttask.cpp \
    databasemanager.cpp \
    main.cpp \
    logindialog.cpp \
    organizerwidget.cpp \
    studentwidget.cpp

HEADERS += \
    activitymodel.h \
    adminwidget.h \
    csvexporttask.h \
    databasemanager.h \
    logindialog.h \
    organizerwidget.h \
    studentwidget.h

FORMS += \
    adminwidget.ui \
    createactivitydialog.ui \
    logindialog.ui \
    organizerwidget.ui \
    studentwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
