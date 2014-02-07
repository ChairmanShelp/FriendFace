#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <QString>
#include <algorithm>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "detector.h"

using namespace cv;
using namespace std;
using namespace boost;

/**
 * @brief Prosty detektor wykrywający twarz w klatce
 */
class FaceDetector : public Detector
{
public:

    /**
     * @brief Konstruktor detektora twarzy.
     *
     * @param faceCascadePath ściezka do pliku haar xml, ładuje klasyfikator
     * @param captureDevice jeśli 0, nic innego nie będzie rejestrowane jako odbiornik ramki
     * @param optimalSubregionDetect jesli true, skanujemy tylko obszar, gdzie wykryto twarz
     * @param roiScaleFactor jak dalego poza twarz powinniśmy skanować
     * @param name nazwa obiektu
     */
    FaceDetector(const QString& faceCascadePath,
                 FrameCapture* captureDevice = 0,
                 bool optimalSubregionDetect = false,
                 double roiScaleFactor = 1.16,
                 const QString& name = "face detector");
    virtual ~FaceDetector(void);

    /**
     * @brief Rozpoczyna wykrywanie obiektów na ostatniej klatce
     */
    virtual void processFrame();

    /**
     * @brief Sprawdza czy wewnętrzny klasyfikator jest pusty
     *
     * @return true jesli wewnętrzny klasyfikator jest pusty
     */
    bool empty() const;

protected:
    CascadeClassifier faceClassifier;

    double roiScaleFactor;
    bool optimalSubregionDetect;

    /**
     * @brief Uruchamia klasyfikator detekcji twarzy.
     *
     * @param frame klatka poddawana wykrywaniu
     * @param results wektor do przechowywania prostokątów wyjściowych
     */
    void runFaceDetect(const Mat& frame, vector<Rect>& results);

    /**
     * @brief Wykrywa twarze w klatce.
     *
     * @param frame klatka poddawana wykrywaniu
     */
    void detectFaces(const Mat& frame);

    /**
     * @brief Wykrywa twarze w podregionie klatki używając poprzednich wyników.
     *
     * @param lastRects ostatnie wyniki
     * @param frame aktualna klatka
     */
    void detectFacesInROI(vector<Rect>& lastRects, const Mat& frame);

    /**
     * @brief Ładuje wewnętrzny klasyfikator.
     *
     * @param faceCascadePath ścieżka do pliku xml
     * @return true - powodzenie
     */
    bool loadClassifier(const QString& faceCascadePath);
};

#endif // FACEDETECTOR_H
