#ifndef DETECTOR_H
#define DETECTOR_H

#include <QString>
#include <boost/thread/mutex.hpp>
#include "framereceiver.h"
#include "framecapture.h"
#include "controller.h"

using namespace cv;
using namespace std;
using namespace boost;

class Controller;

/**
 * @brief Klasa bazowa dla detektorów.
 *
 */
class Detector : public FrameReceiver
{
public:
    /**
     * @brief Tworzy obiekt o zadanej nazwie i urzadzeniu przechwytującym
     *
     * @param name nazwa
     * @param captureDevice urządzenie przechwytujące
     */
    Detector(const QString& name, FrameCapture* captureDevice = 0);
    virtual ~Detector(void);

    /**
     * @brief Zwraca nazwe detektora
     *
     * @return nazwa
     */
    QString getName() const;

    /**
     * @brief Zwraca rezultat ostatniej detekcji
     *
     * @return prostokaty, w których są wykryte obiekty
     */
    vector<Rect> getAreas();

    /**
     * @brief Dodaje kontroler do zestawu.
     *
     * @param controller wskaźnik na kontroler do odbierania powiadomień z tego obiektu
     * @return true jeśli dodano, false w przypadku błędu lub gdy kontroler już jest w zestawie
     */
    bool addController(Controller* controller);

    /**
     * @brief Usuwa kontroler z zestawu.
     *
     * @param controller wskaźnik na kontroler do usunięcia
     */
    void removeController(Controller* controller);

    /**
     * @brief Zwraca liczbę kontrolerów
     */
    unsigned getControllerCount();

    /**
     * @brief Uruchamia wewnętrzny watek.
     */
    void threadStart();

    /**
     * @brief Wysyła sygnał stop do wewnętrznego wątku.
     */
    void requestTreadStop();

    /**
     * @brief Aktualizuje wewnętrzną klatkę.
     *
     * @param frame ostatnia przechwycona klatka
     */
    virtual void receiveFrame(const Mat& frame);

    /**
     * @brief Metoda szablonowa dla klas potomnych.
     */
    virtual void processFrame() = 0;

    /**
     * @brief Zwraca true jeśli jest aktualizacja.
     *
     * @return true jeśli jest aktualizacja
     */
    bool getHasUpdates();

protected:
    QString name;

    // jeśli false, wątek zatrzymuje się
    bool keepProcessing;

    // true jeśli ostatnia processFrame posiada aktualizację, inaczej false
    volatile bool hasUpdates;

    // zestaw kontrolerów, które mają być informowane o zmianach statusu
    set<Controller*> controllers;
    mutex controllersLock;

    Mat nextFrame;
    mutex frameLoadLock;

    /**
     * @brief Pobiera najnowszą klatkę.
     *
     * @return kopia najnowszej klatki
     */
    Mat getMostRecentFrame();

    /**
     * @brief Powiadamia podłączone kontrolery o aktualizacjach.
     */
    void notifyControllers();

    /**
     * @brief Aktualizuje obszary wykrywania i sprawdza czy są różne
     *
     * @param areas nowy wektor prostokątów, w których wykryto obiekty
     * @return true jeśli obszary różnią się od tych ze stanu poprzedniego, false jeśli nie
     */
    bool setAreas(const vector<Rect>& areas);

    /**
     * @brief Sprawdza czy detektor powiadamia automatycznie
     *
     * @return true jeśli jest autopowiadamianie, false jeśli nie ma
     */
    bool autoNotifying();

    /**
     * @brief Ustawia wewnętrzna flagę, która powoduje, że setAreas wysyła
     *        dołączonym kontrolerom automatyczne powiadomienia.
     *
     * @param doAutoNotify true jeśli jest potrzeba automatycznego powiadamiania,
     *                     false jeśli nie ma
     */
    void setAutoNotify(bool doAutoNotify);

private:
    bool autoNotify;
    mutex areaLock;
    vector<Rect> areas;
};

#endif // DETECTOR_H
