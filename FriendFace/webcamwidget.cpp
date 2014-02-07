#include "webcamwidget.h"

WebcamWidget::WebcamWidget(QWidget* parent, FrameCapture* existingCaptureDevice)
    : QWidget(parent)
{
    if(existingCaptureDevice != 0)
    {
        existingCaptureDevice->addFrameReceiver(this);
    }

    previousTime = posix_time::microsec_clock::local_time();

    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setSizePolicy(sizePolicy);
    setMinimumSize(640,480);
}

WebcamWidget::~WebcamWidget()
{
}

QImage WebcamWidget::convertToQImage(const Mat& mat) const
{
    IplImage cvimage = (IplImage)mat;
    return QImage((const uchar*)cvimage.imageData, cvimage.width, cvimage.height, QImage::Format_RGB888).rgbSwapped();
}

void WebcamWidget::paintEvent(QPaintEvent* event)
{
    loadNextFrameIntoCurrent();

    if(!currentFrame.empty())
    {
        QImage img = convertToQImage(currentFrame);
        QPainter painter(this);
        painter.drawImage(QPoint(0,0), img);
    }
}


void WebcamWidget::stateChanged(Detector *detector)
{
    lock_guard<mutex>(this->facesLock);
    updateFramerate();
    faces = detector->getAreas();
}

inline void WebcamWidget::updateFramerate()
{
    posix_time::ptime currentTime = posix_time::microsec_clock::local_time();
    posix_time::time_duration duration = currentTime.time_of_day() - previousTime.time_of_day();
    if(duration.total_milliseconds() != 0)
    {
        detectionFramerate = (1000 / duration.total_milliseconds());
    }
    previousTime = currentTime;
}

void WebcamWidget::receiveFrame(const Mat& frame)
{
    frameLoadLock.lock();
    nextFrame = frame.clone();
    frameLoadLock.unlock();
    update();
}

cv::Mat WebcamWidget::getCurrentFrame()
{
    lock_guard<mutex>(this->frameLoadLock);
    return currentFrame.clone();
}

vector<Rect> WebcamWidget::getCurrentFaces()
{
    lock_guard<mutex>(this->facesLock);
    return faces;
}

void WebcamWidget::loadNextFrameIntoCurrent()
{
    lock_guard<mutex>(this->frameLoadLock);
    currentFrame = nextFrame;
}
