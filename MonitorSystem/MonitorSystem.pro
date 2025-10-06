#-------------------------------------------------
#
# Project created by QtCreator 2019-08-23T09:43:16
#
#-------------------------------------------------

QT       += core gui charts network xml sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = THMonitorSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++20

SOURCES += \
    DatabaseTool.cpp \
        InterfaceLayout.cpp \
    JsonTool.cpp \
    LogOutput.cpp \
    LoginLayout.cpp \
    StatisticalChart.cpp \
        main.cpp \
    StatisticsLayout.cpp \
        widget.cpp \
    NetworkThread.cpp \
    SettingLayout.cpp \
    MailTool.cpp \
    sendemailapi/emailaddress.cpp \
    sendemailapi/mimeattachment.cpp \
    sendemailapi/mimecontentformatter.cpp \
    sendemailapi/mimefile.cpp \
    sendemailapi/mimehtml.cpp \
    sendemailapi/mimeinlinefile.cpp \
    sendemailapi/mimemessage.cpp \
    sendemailapi/mimemultipart.cpp \
    sendemailapi/mimepart.cpp \
    sendemailapi/mimetext.cpp \
    sendemailapi/quotedprintable.cpp \
    sendemailapi/smtpclient.cpp \
    AlarmSound.cpp \
    AlgorithmTool.cpp

HEADERS += \
    DatabaseTool.h \
        InterfaceLayout.h \
    JsonTool.h \
    LogOutput.h \
    LoginLayout.h \
    StatisticalChart.h \
    StatisticsLayout.h \
        widget.h \
    NetworkThread.h \
    SettingLayout.h \
    MailTool.h \
    sendemailapi/emailaddress.h \
    sendemailapi/mimeattachment.h \
    sendemailapi/mimecontentformatter.h \
    sendemailapi/mimefile.h \
    sendemailapi/mimehtml.h \
    sendemailapi/mimeinlinefile.h \
    sendemailapi/mimemessage.h \
    sendemailapi/mimemultipart.h \
    sendemailapi/mimepart.h \
    sendemailapi/mimetext.h \
    sendemailapi/quotedprintable.h \
    sendemailapi/smtpclient.h \
    sendemailapi/smtpmime.h \
    AlarmSound.h \
    AlgorithmTool.h

FORMS += \
        InterfaceLayout.ui \
    LoginLayout.ui \
    StatisticalChart.ui \
    StatisticsLayout.ui \
        widget.ui \
    SettingLayout.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

RC_ICONS = icon.ico

VERSION = 1.0.0.1

QMAKE_TARGET_PRODUCT = "THMonitorSystem"
QMAKE_TARGET_COMPANY = "INVO"
QMAKE_TARGET_DESCRIPTION = "THMonitorSystem"
QMAKE_TARGET_COPYRIGHT = "INVO Copyright 2019-2029"

DISTFILES += \
    readme.txt
