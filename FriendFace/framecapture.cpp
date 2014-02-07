#include "framecapture.h"

FrameCapture::FrameCapture(float framerate)
{
    videoCapture = new VideoCapture(0);
    setFramerate(framerate);
}

FrameCapture::FrameCapture(VideoCapture* existingVideoCapture, float framerate)
    :videoCapture(existingVideoCapture)
{
    if(videoCapture == 0)
        videoCapture = new VideoCapture(0);

    setFramerate(framerate);
}

FrameCapture::~FrameCapture()
{
    videoCapture->release();
}

VideoCapture* FrameCapture::getVideoCapture()
{
    return videoCapture;
}

void FrameCapture::startCapturing()
{
    keepCapturing = true;
    captureLoop();
}

void FrameCapture::stopCapturing()
{
    keepCapturing = false;
}

void FrameCapture::captureLoop()
{
    while(keepCapturing)
    {
        Mat frame;
        *videoCapture >> frame;
        notifyReceivers(frame);
    }
}

void FrameCapture::notifyReceivers(Mat &frame)
{
    lock_guard<mutex>(this->receiversLock);

    for(set<FrameReceiver* >::iterator itr = receivers.begin(); itr != receivers.end(); ++itr)
        (*itr)->receiveFrame(frame);
}

bool FrameCapture::addFrameReceiver(FrameReceiver* receiver)
{
    lock_guard<mutex>(this->receiversLock);
    return receivers.insert(receiver).second;
}

void FrameCapture::removeFrameReceiver(FrameReceiver* receiver)
{
    lock_guard<mutex>(this->receiversLock);
    receivers.erase(receiver);
}

float FrameCapture::getFramerate()
{
    return framerateWaitMs * 1000.0f;
}

void FrameCapture::setFramerate(float framerate)
{
    this->framerateWaitMs = static_cast<unsigned long>(1000.0f / framerate);
}
