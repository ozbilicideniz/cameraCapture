QT += core gui widgets multimedia multimediawidgets
QT += waylandclient-private

CONFIG += c++11 link_pkgconfig

TARGET = cameraCapture
TEMPLATE = app

SOURCES += \
    filebrowser.cpp \
    main.cpp \
    camera.cpp \
    cameradisplaywidget.cpp \
    keyboard.cpp \
    mediaviewer.cpp

HEADERS += \
    camera.h \
    cameradisplaywidget.h \
    filebrowser.h \
    keyboard.h \
    mediaviewer.h

FORMS += mainwindow.ui

PKGCONFIG += gstreamer-1.0
PKGCONFIG += gstreamer-video-1.0
PKGCONFIG += gstreamer-app-1.0
PKGCONFIG += gstreamer-wayland-1.0