#ifndef TRAINERIMAGE_H
#define TRAINERIMAGE_H

#include <cmath>
#include <QString>
#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

/**
 * @brief Klasa używana do przygotowania i przechowywania obrazów do użycia
 *        w klasach rozpoznających.
 */
class TrainerImage
{
public:
    /**
     * @brief Inicjalizuje klasę do zmiany rozmiaru (z lub bez zachowania ratio), przetworzenia
     *        do skali szarości i opcjonalnie do zachowania obrazu.
     *
     * @param goalSize docelowy rozmiar
     * @param keepAspectRatio true - zachowuje ratio, false - nie zachowuje
     * @param directoryPath katalog do przechowywania obrazów (domyślnie - aktualny katalog roboczy)
     */
    TrainerImage(Size goalSize = Size(140,200), bool keepAspectRatio = true, const QString& directoryPath = "");

    /**
     * @brief Przetwarza obraz z godnie z konfiguracją obiektu i zwraca
     *        przetworzony obraz.
     *
     * @param image obraz do przetworzenia
     * @return przetworzona macierz
     */
    Mat processImage(Mat& image);

    /**
     * @brief Przetwarza obraz zgodnie z konfiguracją obiektu i zapsuje
     * przetworzony obraz do katalogu z zadaną nazwą.
     *
     * @param image obraz do przetworzenia
     * @param name docelowa nazwa pliku z obrazem
     * @return true jeśli pomyślnie zapisano, false jeśli nie
     */
    bool processAndSaveImage(Mat& image, const QString& name);

protected:
    QString directoryPath;
    Size goalSize;
    bool keepAspectRatio;
};
#endif // TRAINERIMAGE_H
