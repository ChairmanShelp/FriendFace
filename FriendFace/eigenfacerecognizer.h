#ifndef EIGENFACERECOGNIZER_H
#define EIGENFACERECOGNIZER_H

#include <map>
#include <limits>
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include "recognizer.h"

using namespace cv;
using namespace std;

/**
 * @brief Klasa implementująca algorytm rozpoznawania twarzy.
 */
class EigenfaceRecognizer : public Recognizer
{
public:
    EigenfaceRecognizer(void);
    virtual ~EigenfaceRecognizer(void);

    /**
     * @brief Dodaje ścieżke do obrazu treningowego do zestawu obrazów.
     *
     * @param filename ścieżka do pliku
     * @param name nazwa nadawana plikowi
     * @return  true jeśli dodano poprawnie
     */
    bool addTrainingImage(const QString& filename, const QString& name);

    /**
     * @brief Rozpoczyna proces uczenia dla wszystkich obrazów dodanych przez
     *        addTrainingImage.
     *
     * @return true jeśli nauczono, false jesli nie
     */
    bool train();

    /**
     * @brief Rozpoznaje twarz i zwraca najlepsze dopasowanie.
     *
     * @param face macierz zawierająca twarz
     * @return nazwa najściślejszego dopasowania z zestawu wytrenowanych obrazów
     */
    QString recognize(Mat& face);

    /**
     * @brief Zwraca liczbę wyuczonych obrazów.
     *
     * @return liczba wyuczonych obrazów
     */
    size_t numTrainingImages();

protected:
    bool loadTrainingImages();
    // Analiza Głównych Składowych
    void doPCA();
    void freeMemory(int fromIndex = -1);
    int findNearestNeighbor(float* projectedTestImage);

    // scieżka - > nazwa
    map<QString, QString> trainingImages;

    // index - > ściezka, nazwa
    vector< pair<QString, QString> > recognitionDescriptor;

    IplImage** images;			// tablica wytrenowanych obrazów
    int numTrainedImages;		// liczba używanych wytrenowanych obrazów
    int numEigenvalues;			// liczba wartości eigena
    IplImage** eigenVectors;	// tablica wektorów eigena
    IplImage* averageImage;		// średni obraz z wyuczonych obrazów
    CvMat* eigenvalues;			// macierz wartości eigena
    CvMat* projectedTrainImage;	// wyświetlana macierz obrazu
};
#endif // EIGENFACERECOGNIZER_H
