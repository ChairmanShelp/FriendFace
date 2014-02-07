#ifndef FRAMERECEIVER_H
#define FRAMERECEIVER_H

#include <cv.h>

using namespace cv;

/**
 * @brief Interfejs dla obiorników ramek (klatek)
 */
class FrameReceiver
{
public:
    virtual void receiveFrame(const Mat& frame) = 0;
};

#endif // FRAMERECEIVER_H
