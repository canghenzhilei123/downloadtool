QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

LIBS += E:/code/downloadtool/downloadtool/CH341DLL.LIB

LIBS += -L$$quote(D:/OpenSSL-Win32/lib) -llibcrypto
INCLUDEPATH += $$quote(D:/OpenSSL-Win32/include)

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
    aescrypto.cpp \
    base64.cpp \
    ch341.cpp \
    client.cpp \
    configreader.cpp \
    dataencoder.cpp \
    dataparser.cpp \
    download.cpp \
    downthread.cpp \
    loginform.cpp \
    main.cpp \
    mainwindow.cpp \
    mytime.cpp \
    readclient.cpp \
    readthread.cpp \
    registerform.cpp \
    rsacrypto.cpp \
    sha256.cpp \
    user.cpp \
    widget.cpp \
    writeclient.cpp

HEADERS += \
    CH341DLL_GCC.h \
    aescrypto.h \
    base64.h \
    ch341.h \
    client.h \
    configreader.h \
    dataencoder.h \
    dataparser.h \
    download.h \
    downthread.h \
    loginform.h \
    mainwindow.h \
    mytime.h \
    protocolmsg.h \
    readclient.h \
    readthread.h \
    registerform.h \
    rsacrypto.h \
    sha256.h \
    user.h \
    widget.h \
    writeclient.h

FORMS += \
    download.ui \
    login.ui \
    loginform.ui \
    mainwindow.ui \
    registerForm.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
