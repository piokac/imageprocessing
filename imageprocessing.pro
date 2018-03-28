#-------------------------------------------------
#
# Project created by QtCreator 2018-02-28T21:14:14
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imageprocessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
win32{
        LIBS += -L"C:/opencv/build/x64/vc15/bin" -L"C:/opencv/build/x64/vc15/lib"
        DEPENDPATH += C:/opencv/build/include
        INCLUDEPATH += C:/opencv/build/include
        CONFIG(debug){
            LIBS += -lopencv_world341d
        }

        CONFIG(release){
            LIBS += -lopencv_world341
        }
}
unix {

                INCLUDEPATH +=/usr/local/include/opencv2
                LIBS +=  -L/usr/local/lib -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -lopencv_calib3d -lopencv_features2d -lopencv_xfeatures2d
}
