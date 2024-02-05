QT       += core gui sql serialport printsupport charts network multimedia networkauth websockets positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp\
    ambulance.cpp \
    arduino.cpp \
    assistant.cpp \
    audiotranscript.cpp \
    bill.cpp \
    calendar.cpp \
    config.cpp\
    connection.cpp\
    dialog.cpp \
    equipment.cpp \
    event.cpp \
    googledriveoauth.cpp \
    hospital.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    smtp.cpp \
    staff.cpp\
    staffwidget.cpp \
    weather.cpp

HEADERS += \
    account.h\
    ambulance.h \
    arduino.h \
    assistant.h \
    audiotranscript.h \
    bill.h \
    calendar.h \
    config.h \
    connection.h\
    dialog.h \
    equipment.h \
    event.h \
    googledriveoauth.h \
    hospital.h \
    logindialog.h \
    mainwindow.h \
    smtp.h \
    staff.h\
    staffwidget.h \
    weather.h

FORMS += \
    dialog.ui \
    mainwindow.ui\
    logindialog.ui \
    staffwidget.ui \
    staffwindow.ui

# LIBS += -lcryptopp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc
win32: LIBS+= -LC:/Qt/Tools/OpenSSL/Win_x86/lib/libssl.lib

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libcurl

include(curl-config.pri)
