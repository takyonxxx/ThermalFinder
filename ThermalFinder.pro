#-------------------------------------------------
#
# Project created by QtCreator 2019-05-27T10:38:31
#
#-------------------------------------------------

QT       += core gui network positioning location

CONFIG += mobility
MOBILITY += sensors
MOBILITY += location

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quickwidgets

TARGET = ThermalFinder
TEMPLATE = app

win32: DEFINES += WIN32 _WINDOWS _USE_MATH_DEFINES

win32:RC_ICONS += $$PWD\icons\app.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

message("enabling c++17 support in clang")
CONFIG += c++1z

INCLUDEPATH += .

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        trackmanager.cpp

HEADERS += \
        conversions.h \
        mainwindow.h \
        pathcontroller.h \
        trackmanager.h

FORMS += \
        mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

ios {
QMAKE_INFO_PLIST = ios/Info.plist
}

android {

QT += androidextras

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
}
