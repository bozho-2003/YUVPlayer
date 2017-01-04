#-------------------------------------------------
#
# Project created by QtCreator 2016-10-13T22:54:33
#
#-------------------------------------------------

QT       += core gui
QMAKE_MAC_SDK = macosx10.12

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YUVPlayer
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += ./opencv/include/
INCLUDEPATH += ./opencv/include/opencv
INCLUDEPATH += ./opencv/include/opencv2
INCLUDEPATH += ./propertytree/include/
INCLUDEPATH += ./format/include/
INCLUDEPATH += ./filter/include/
INCLUDEPATH += ./tinyxml/include/

#LIBS += -L./opencv/lib -lopencv_core

#LIBS += -L/Users/bozho/MyCode/svn-source/trunk/Qt-example/YUVPlayer/opencv/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
SOURCES += main.cpp\
    cyuvdoc.cpp \
    common.cpp \
    filesource.cpp \
    render.cpp \
    inner_modules.cpp \
    imagewnd.cpp \
    yuv_value_wnd.cpp \
    yuv_compare_wnd.cpp \
    propertytree/source/qtbuttonpropertybrowser.cpp \
    propertytree/source/qteditorfactory.cpp \
    propertytree/source/qtgroupboxpropertybrowser.cpp \
    propertytree/source/qtpropertybrowser.cpp \
    propertytree/source/qtpropertybrowserutils.cpp \
    propertytree/source/qtpropertymanager.cpp \
    propertytree/source/qttreepropertybrowser.cpp \
    propertytree/source/qtvariantproperty.cpp \
    format/source/format_i420.cpp \
    format/source/format_i422.cpp \
    format/source/format_rgb24.cpp \
    format/source/format_uyvy.cpp \
    format/source/format_y8.cpp \
    format/source/format_y16.cpp \
    format/source/format_yuv444.cpp \
    format/source/format_yuyv.cpp \
    format/source/format_yv12.cpp \
    format/source/format_yv16.cpp \
    yuv_main_wnd.cpp \
    filter/source/filter_test.cpp \
    filter_wnd.cpp \
    filter/source/filterman.cpp \
    format/source/formaterman.cpp \
    filter/source/filter_mirror.cpp \
    tinyxml/source/tinystr.cpp \
    tinyxml/source/tinyxml.cpp \
    tinyxml/source/tinyxmlerror.cpp \
    tinyxml/source/tinyxmlparser.cpp \
    tinyxml/source/uti_xmlparse.cpp \
    filter/source/filter_conv.cpp

HEADERS  += \
    common.h \
    format.h \
    cyuvdoc.h \
    filesource.h \
    render.h \
    inner_modules.h \
    imagewnd.h \
    yuv_value_wnd.h \
    yuv_compare_wnd.h \
    propertytree/include/qtbuttonpropertybrowser.h \
    propertytree/include/qteditorfactory.h \
    propertytree/include/qtgroupboxpropertybrowser.h \
    propertytree/include/qtpropertybrowser.h \
    propertytree/include/qtpropertybrowserutils_p.h \
    propertytree/include/qtpropertymanager.h \
    propertytree/include/qttreepropertybrowser.h \
    propertytree/include/qtvariantproperty.h \
    format/include/format_i420.h \
    format/include/format_i422.h \
    format/include/format_rgb24.h \
    format/include/format_uyvy.h \
    format/include/format_y8.h \
    format/include/format_y16.h \
    format/include/format_yuv444.h \
    format/include/format_yuyv.h \
    format/include/format_yv12.h \
    format/include/format_yv16.h \
    yuv_main_wnd.h \
    filter/include/filter_test.h \
    filter_wnd.h \
    filter/include/filterman.h \
    format/include/formaterman.h \
    filter/include/filter_mirror.h \
    tinyxml/include/tinystr.h \
    tinyxml/include/tinyxml.h \
    tinyxml/include/uti_xmlparse.h \
    filter/include/filter_conv.h

FORMS    += \
    imagewidget.ui \
    yuv_value_wnd.ui \
    yuv_compare_wnd.ui \
    filter.ui \
    yuv_main_wnd.ui

DISTFILES +=

RESOURCES += \
    resource.qrc \
    propertytree/qtpropertybrowser.qrc
