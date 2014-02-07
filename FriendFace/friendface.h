#ifndef FRIENDFACE_H
#define FRIENDFACE_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <cv.h>
#include <highgui.h>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include "logger.h"
#include "trainerimage.h"
#include "webcamwidget.h"
#include "framecapture.h"
#include "eigenfacerecognizer.h"
#include "facedetector.h"

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

/**
 * @brief Główne okno aplikacji
 */
class FriendFace : public QMainWindow
{
    Q_OBJECT

public:
    FriendFace(QWidget* parent = 0);
    ~FriendFace();

public slots:
    void captureTrainingImage();
    void trainRecognizer();
    void recognizeFaces();

protected:
    static const Size testImageSize;

    WebcamWidget* webcamWidget;
    QListWidget* logWidget;

    QPushButton* captureTrainingImageButton;
    QPushButton* trainRecognizerButton;
    QPushButton* recognizeFaceButton;

    boost::thread frameCaptureThread;
    boost::thread faceDetectorThread;

    shared_ptr<VideoCapture> videoCapture;
    shared_ptr<FrameCapture> frameCapture;
    shared_ptr<FaceDetector> faceDetector;
    shared_ptr<EigenfaceRecognizer> recognizer;

    void closeEvent(QCloseEvent* event);

    void setupFramework();
    void setupGUI();

    void captureFrameAndFaces(vector<Rect> &rects, Mat& frame);
};

#endif // FRIENDFACE_H
