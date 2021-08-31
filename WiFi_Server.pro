QT       += core gui network printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colorscale.cpp \
    fileform.cpp \
    fileprocessing.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    serialcom.cpp \
    server.cpp

HEADERS += \
    colorscale.h \
    fileform.h \
    fileprocessing.h \
    mainwindow.h \
    qcustomplot.h \
    serialcom.h \
    server.h

FORMS += \
    colorscale.ui \
    fileform.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
