#-------------------------------------------------
#
# Project created by QtCreator 2017-03-05T12:30:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LegoManual
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    Camera.cpp \
    themanual.cpp \
    constructar.cpp

HEADERS  += \
    Camera.h \
    lego.h \
    themanual.h \
    constructar.h

FORMS    += \
    constructar.ui

INCLUDEPATH += $$(OPENCV_SDK_DIR)\include
INCLUDEPATH += $$(UEYE_DIR)\include

LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_calib3d401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_core401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_dnn401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_features2d401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_flann401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_gapi401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_highgui401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_imgcodecs401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_imgproc401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_ml401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_objdetect401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_photo401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_stitching401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_video401.dll
LIBS += $$(OPENCV_SDK_DIR)\x64\mingw\bin\libopencv_videoio401.dll


LIBS += $$(UEYE_DIR)\Lib\uEye_api.lib
LIBS += $$(UEYE_DIR)\Lib\uEye_api_64.lib
LIBS += $$(UEYE_DIR)\Lib\uEye_tools.lib
LIBS += $$(UEYE_DIR)\Lib\uEye_tools_64.lib

# more correct variant, how set includepath and libs for mingw
# add system variable: OPENCV_SDK_DIR=D:/opencv/opencv-build/install
# read http://doc.qt.io/qt-5/qmake-variable-reference.html#libs

#INCLUDEPATH += $$(OPENCV_SDK_DIR)/include

RESOURCES += \
    resources.qrc

DISTFILES +=



