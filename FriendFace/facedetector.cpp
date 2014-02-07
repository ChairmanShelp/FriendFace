#include "facedetector.h"

FaceDetector::FaceDetector(const QString& faceCascadePath,
                           FrameCapture* captureDevice,
                           bool optimalSubregionDetect,
                           double roiScaleFactor,
                           const QString& name)
    :Detector(name, captureDevice),
      roiScaleFactor(roiScaleFactor),
      optimalSubregionDetect(optimalSubregionDetect)
{
    faceClassifier.load(faceCascadePath.toStdString());
}

FaceDetector::~FaceDetector(void)
{
}

void FaceDetector::processFrame()
{
    Mat frame = getMostRecentFrame();
    if(frame.empty())
    {
        return;
    }
    detectFaces(frame);
    if(hasUpdates)
    {
        notifyControllers();
    }
}

bool FaceDetector::empty() const
{
    return faceClassifier.empty();
}

void FaceDetector::detectFaces(const cv::Mat &frame)
{
    if(faceClassifier.empty())
    {
        return;
    }

    vector<Rect> lastRects = getAreas();
    try
    {
        if(!(optimalSubregionDetect && lastRects.size() > 0))
        {
            vector<Rect> tempRects;
            runFaceDetect(frame, tempRects);
            hasUpdates = setAreas(tempRects);
        }
        else
        {
            detectFacesInROI(lastRects, frame);
        }

    }
    catch (Exception& exc)
    {
        return;
    }
}

// bardziej optymalne wykrywanie dla mniejszych regionów
void FaceDetector::detectFacesInROI(vector<Rect>& lastRects, const Mat& frame)
{
    vector<Rect> newRects;
    for(vector<Rect>::iterator i = lastRects.begin(); i != lastRects.end(); ++i)
    {
        vector<Rect> tempRects;
        if(	i->x < 0 || i->y < 0 || i->width < 2 || i->height < 2)
        {
            continue;
        }

        // openCV wykrywa prostokąt, stąd wystarczy tylko szerokość
        int adjustFactor = max((static_cast<int>(i->width * roiScaleFactor) - i->width) / 2, 35);

        // ustawia region na poprzednią klatkę
        Mat roiFrame = frame(*i);

        // poszerza klatkę
        roiFrame.adjustROI(adjustFactor*1.10, adjustFactor*1.10, adjustFactor, adjustFactor);

        // wykrycie na mniejszej klatce
        runFaceDetect(roiFrame, tempRects);

        // dodanie do rezultatów wektora newRects
        for(vector<Rect>::iterator iRect = tempRects.begin(); iRect != tempRects.end(); ++iRect)
        {
            iRect->x += i->x - adjustFactor;
            iRect->y += i->y - adjustFactor;
            newRects.push_back(*iRect);
        }
    }

    // jeśli nie znaleziono twarzy, skanujemy całą klatkę
    if(newRects.empty())
    {
        runFaceDetect(frame, newRects);
    }

    // aktualizacja resultatów
    setAreas(newRects);
}

bool FaceDetector::loadClassifier(const QString& faceCascadePath)
{
    return faceClassifier.load(faceCascadePath.toStdString());
}

// fukcja do centralizacji parametrów
inline void FaceDetector::runFaceDetect(const Mat& frame, vector<Rect>& results)
{
    faceClassifier.detectMultiScale(frame, /*klatka wejściowa dla detekcji*/
                                    results, /*wektor rezultatów*/
                                    1.13,  /*współczynnik skali OpenCV wykorzystywany do zwiększenia przejścia każdego okna, domyślnie 1.1*/
                                    3,     /*minNeighbors, domyslnie: 3 (minimalna liczba prostokątów do zgrupowania twarzy)*/
                                    1,   /*flagi, Canny Prunning (1) uruchamia detektor krawedzi aby wyeliminowac obszary, w których nie ma twarzy*/
                                    Size(55, 55) /*minimalny rozmiar prostokąta*/
                                    );
}
