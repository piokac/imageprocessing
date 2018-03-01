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
        LIBS += -L"C:/opencv/build/x64/vc14/bin"
        DEPENDPATH += C:/opencv/build/include
        INCLUDEPATH += C:/opencv/build/include
        CONFIG(debug){
            LIBS += -lopencv_world330d
        }

        CONFIG(release){
            LIBS += -lopencv_world330
        }
}
unix {
                #LIBS += -lGLU -lGL

                #INCLUDEPATH +=/home/piotr/OpenCV/opencv-2.4.10include/opencv2
                #INCLUDEPATH +=/home/piotr/OpenCV/opencv-2.4.10/include/opencv2
                #DEPENDPATH +=/home/piotr/OpenCV/opencv-2.4.10/build/include/opencv2
                INCLUDEPATH +=/usr/local/include/opencv2
                LIBS +=  -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs
}
