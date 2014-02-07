#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QPen>
#include <QImage>
#include <QWidget>
#include <QPainter>
#include <cv.h>
#include <qdebug.h>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "logger.h"
#include "detector.h"
#include "controller.h"
#include "framereceiver.h"

using namespace cv;
using namespace std;
using namespace boost;

/**
 * @brief Widget wyświetlający obraz z kamery
 */
class WebcamWidget : public QWidget, public FrameReceiver, public Controller
{
    Q_OBJECT

public:
    WebcamWidget(QWidget* parent, FrameCapture* existingCaptureDevice);
    virtual ~WebcamWidget();

    QImage convertToQImage(const Mat& mat) const;

    void receiveFrame(const Mat& frame);
    void stateChanged(Detector* detector);

    Mat getCurrentFrame();

    vector<Rect> getCurrentFaces();

protected:
    vector<Rect> faces;
    mutex facesLock;

    Mat currentFrame;
    Mat nextFrame;

    mutex frameLoadLock;

    // kopiuje nextFrame do currentFrame,
    virtual void loadNextFrameIntoCurrent();

    void paintEvent(QPaintEvent* event);

    posix_time::ptime previousTime;
    volatile int detectionFramerate;
    void updateFramerate();
};

#endif // WEBCAMWIDGET_H
