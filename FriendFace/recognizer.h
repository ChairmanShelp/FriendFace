#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QString>
#include <cv.h>

using namespace cv;
using namespace std;

/**
 * @brief Klasa bazowa dla rozpoznawaczy. Zapewnia interfejs dla różnych
 *        rodzajów rozpoznawaczy.
 */
class Recognizer
{
public:

    Recognizer() : isTrained(false){}

    /**
     * @brief Dodaje do zestawu poznanych obrazów ścieżkę do obrazu treningowego.
     *
     * @param filename ścieżka do obrazu
     * @param name nazwa przypisywana do pliku
     * @return true jeśli poprawnie dodano obraz
     */
    virtual bool addTrainingImage(const QString& filename, const QString& name) = 0;

    /**
     * @brief Rozpoczyna proces uczenia na wszystkich poznanych obrazach dodanych
     *        przez addTrainingImage.
     *
     * @return true jeśli poprawnie nauczono
     */
    virtual bool train() = 0;

    /**
     * @brief Przeprowadza rozpoznawanie twarzy i zwraca najlepsze dopasowanie.
     *
     * @param face macierz zawierająca twarz
     * @return nazwa najlepszego dopasowania spośród zbioru treningowego
     */
    virtual QString recognize(Mat &face) = 0;

    /**
     * @brief Zwraca liczbę dostępnych obrazów treningowych.
     *
     * @return liczba dostępnych obrazów treningowych.
     */
    virtual size_t numTrainingImages() = 0;

    bool trained() { return isTrained; }

protected:
    bool isTrained;
};

#endif // RECOGNIZER_H
