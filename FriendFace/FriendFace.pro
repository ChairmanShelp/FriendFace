#-------------------------------------------------
#
# Project created by QtCreator 2013-12-21T15:42:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FriendFace
TEMPLATE = app


SOURCES += main.cpp\
        friendface.cpp \
    detector.cpp \
    webcamwidget.cpp \
    framecapture.cpp \
    trainerimage.cpp \
    facedetector.cpp \
    logger.cpp \
    eigenfacerecognizer.cpp

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -lopencv_nonfree
LIBS += -lboost_filesystem
LIBS += -lboost_system
LIBS += -lboost_thread-mt

HEADERS  += friendface.h \
    framereceiver.h \
    detector.h \
    webcamwidget.h \
    framecapture.h \
    recognizer.h \
    trainerimage.h \
    facedetector.h \
    logger.h \
    eigenfacerecognizer.h \
    controller.h
