#ifndef FRAMECAPTURE_H
#define FRAMECAPTURE_H

#include <set>
#include <cv.h>
#include <highgui.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>
#include "framereceiver.h"

using namespace cv;
using namespace std;
using namespace boost;

/**
 * @brief Przechwytuje ramki z urządzenia przechwytującego (cv::VideoCapture)
 *		  i wysyła ramki do wszystkich podłączonych odbiorników
 */
class FrameCapture
{
public:
    /**
     * @brief Tworzy obiekt, automatycznie tworzy VideoCapture
     *
     * @param framerate ilość klatek na sekundę, w której wysyłana jest aktualizacja do odbiorników
     */
    FrameCapture(float framerate = 24.0f);

    /**
     * @brief Tworzy obiekt z istniejącym urządzeniem przechwwytującym
     *
     * @param existingVideoCapture urządzenie przechwytujące
     * @param framerate ilość klatek na sekundę, w której wysyłana jest aktualizacja do odbiorników
     */
    FrameCapture(VideoCapture* existingVideoCapture, float framerate = 24.0f);

    /**
      * @brief Zamyka urządzenie przechwytujące
      */
    ~FrameCapture();

    /**
     * @brief Zwraca wskaźnik do urządzenia przechwytującego
     *
     * @return wskaźnik do urządzenia przechwytującego
     */
    VideoCapture* getVideoCapture();

    /**
     * @brief Rozpoczyna przechwytywanie klatek
     */
    void startCapturing();

    /**
     * @brief Sygnalizuje zatrzymanie pętli przechwytywania
     */
    void stopCapturing();

    /**
     * @brief Dodaje obiekt, który ma odbierać ramki
     *
     * @param receiver obiekt, który będzie odbierał ramki
     * @return true, jeśli został on dodany. false, jeśli już został zarejestrowany lub jeśli się nie udało
     */
    bool addFrameReceiver(FrameReceiver* receiver);

    /**
     * @brief Usuwa obiekt, który odbiera ramki
     *
     * @param receiver obiekt do usunięcia
     */
    void removeFrameReceiver(FrameReceiver* receiver);

    /**
     * @brief Zwraca liczbę klatek na sekundę
     *
     * @return liczba klatek na sekundę
     */
    float getFramerate();

    /**
     * @brief Ustawia ilość klatek na sekundę, w którym obiekt ten wysyła ramki do swoich odbiorników
     *
     * @param framerate ilość klatek na sekundę
     */
    void setFramerate(float framerate);

protected:
    VideoCapture* videoCapture;
    unsigned long framerateWaitMs;
    bool keepCapturing;

    set<FrameReceiver* > receivers;
    mutex receiversLock;

    /**
     * @brief Powiadamia odbiorniki
     *
     * @param frame Ramka wysyłana do odbiorników
     */
    void notifyReceivers(Mat &frame);

    /**
     * @brief Przechwytuje ramki i powiadamia odbiorniki
     */
    void captureLoop();
};

#endif // FRAMECAPTURE_H
