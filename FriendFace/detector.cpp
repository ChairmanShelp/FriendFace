#include "detector.h"

Detector::Detector(const QString& name, FrameCapture* captureDevice) : name(name), autoNotify(true)
{
    if(captureDevice != 0)
    {
        captureDevice->addFrameReceiver(this);
    }
}

Detector::~Detector(void)
{
}

QString Detector::getName() const
{
    return name;
}

bool Detector::addController(Controller* controller)
{
    lock_guard<mutex>(this->controllersLock);
    return controllers.insert(controller).second;
}

void Detector::removeController(Controller* controller)
{
    lock_guard<mutex>(this->controllersLock);
    controllers.erase(controller);
}

unsigned Detector::getControllerCount()
{
    return controllers.size();
}

void Detector::notifyControllers()
{
    lock_guard<mutex>(this->controllersLock);
    for(set<Controller*>::iterator i = controllers.begin(); i != controllers.end(); ++i)
    {
        (*i)->stateChanged(this);
    }
}

void Detector::threadStart()
{
    keepProcessing = true;
    while(keepProcessing)
    {
        processFrame();
    }
}

void Detector::requestTreadStop()
{
    keepProcessing = false;
}

vector<Rect> Detector::getAreas()
{
    lock_guard<mutex>(this->areaLock);
    return areas;
}

bool Detector::setAreas(const vector<Rect>& newAreas)
{
    if( areas.size() == newAreas.size() && equal(newAreas.begin(), newAreas.end(), areas.begin()))
    {
        // nic nowego, nie aktualizuj powiadomień
        return false;
    }

    areaLock.lock();
    areas = newAreas;
    areaLock.unlock();

    // obszary się zmieniły i nautoNotify jest włączone, wysłanie aktualizacji
    if(autoNotify)
    {
        notifyControllers();
    }

    return true;
}

bool Detector::autoNotifying()
{
    return autoNotify;
}

void Detector::setAutoNotify(bool doAutoNotify)
{
    autoNotify = doAutoNotify;
}

void Detector::receiveFrame(const Mat& frame)
{
    lock_guard<mutex>(this->frameLoadLock);
    nextFrame = frame;
}

bool Detector::getHasUpdates()
{
    return hasUpdates;
}

Mat Detector::getMostRecentFrame()
{
    lock_guard<mutex>(this->frameLoadLock);
    return nextFrame.clone();
}
