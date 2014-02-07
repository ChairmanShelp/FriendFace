#include "eigenfacerecognizer.h"

EigenfaceRecognizer::EigenfaceRecognizer(void)
    :images(0),
      numTrainedImages(0),
      numEigenvalues(0),
      eigenVectors(0),
      averageImage(0),
      eigenvalues(0),
      projectedTrainImage(0)
{
}

EigenfaceRecognizer::~EigenfaceRecognizer(void)
{
}

bool EigenfaceRecognizer::addTrainingImage(const QString& filename, const QString& name)
{
    trainingImages[filename] = name;
    return true;
}

void EigenfaceRecognizer::freeMemory(int fromIndex)
{
    for(; fromIndex >= 0; fromIndex--)
    {
        cvFree(&images[fromIndex]);
    }

    numTrainedImages = 0;
    cvFree(images);
}

bool EigenfaceRecognizer::train()
{
    isTrained = false;
    if(!loadTrainingImages())
    {
        return false;
    }

    // Analiza Głównych Składowych
    doPCA();

    // wyświetla wytrenowane obrazy w podprzestrzeni PCA
    projectedTrainImage = cvCreateMat( numTrainedImages, numEigenvalues, CV_32FC1 );
    int offset = projectedTrainImage->step / sizeof(float);
    for(int i = 0; i < numTrainedImages; i++)
    {
        cvEigenDecomposite(
                    images[i],
                    numEigenvalues,
                    eigenVectors,
                    0, 0,
                    averageImage,
                    projectedTrainImage->data.fl + i*offset);
    }

    isTrained = true;
    return isTrained;
}


bool EigenfaceRecognizer::loadTrainingImages()
{
    if(trainingImages.size() < 2)
    {
        return false;
    }

    if(images && numTrainedImages > 0)
    {
        freeMemory(numTrainedImages-1); // usuwa obrazy
    }

    // usuwa wszystkie mapowania do ścieżek i nazw
    recognitionDescriptor.clear();

    // liczba obrazów przeznaczonych do treningu
    numTrainedImages = trainingImages.size();

    // zaalokowanie pamięci dla tablicy obrazów
    images = (IplImage**) cvAlloc(sizeof(IplImage*) *numTrainedImages);

    int currentImage = 0;
    for(map<QString, QString>::iterator current = trainingImages.begin(); current != trainingImages.end(); ++current)
    {
        images[currentImage] = cvLoadImage(current->first.toStdString().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        if(images[currentImage] == NULL)
        {
            break;
        }

        // dodaje ściezkę i nazwę jako deskryptory aktualnego pliku
        recognitionDescriptor.push_back(*current);
        currentImage++;
    }

    // nie wszystkie obrazy zostały załadowane
    if(currentImage != numTrainedImages)
    {
        freeMemory(currentImage);
        return false;
    }
    return true;
}

void EigenfaceRecognizer::doPCA()
{
    // ustawia liczbę wartości eigena do użycia
    numEigenvalues = numTrainedImages-1;

    // liczba obrazów twarzy
    CvSize faceImgSize;

    // alokuje obrazy wektorów eigena
    faceImgSize.width  = images[0]->width;
    faceImgSize.height = images[0]->height;

    eigenVectors = (IplImage**)cvAlloc(sizeof(IplImage*) * numEigenvalues);

    for(int i = 0; i < numEigenvalues; i++)
    {
        eigenVectors[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);
    }

    // alokuje tablicę wartości eigena
    eigenvalues = cvCreateMat( 1, numEigenvalues, CV_32FC1 );

    // alokuje średni obraz
    averageImage = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

    // ustawia kryterium zakończenia PCA
    CvTermCriteria calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, numEigenvalues, 1);

    // oblicza średni obraz, wartości eigena i wektory eigena
    cvCalcEigenObjects(
                numTrainedImages,
                (void*)images,
                (void*)eigenVectors,
                CV_EIGOBJ_NO_CALLBACK,
                0,
                0,
                &calcLimit,
                averageImage,
                eigenvalues->data.fl);

    cvNormalize(eigenvalues, eigenvalues, 1, 0, CV_L1, 0);
}

QString EigenfaceRecognizer::recognize(Mat& face)
{
    IplImage faceToRecognize = (IplImage) face;
    float* projectedTestImage = (float*)cvAlloc(sizeof(float) *numEigenvalues);

    // projekcja obrazu testowego w podprzestrzeni PCA
    cvEigenDecomposite(
                &faceToRecognize,
                numEigenvalues,
                eigenVectors,
                0, 0,
                averageImage,
                projectedTestImage);

    int nearest = findNearestNeighbor(projectedTestImage);
    return recognitionDescriptor[nearest].second;
}

size_t EigenfaceRecognizer::numTrainingImages()
{
    return trainingImages.size();
}

int EigenfaceRecognizer::findNearestNeighbor(float* projectedTestImage)
{
    // ustawia najmniejszy dystans do maksymalnej wartości double
    double leastDistanceSquare = numeric_limits<double>::max();

    // indeks najbliższego elementu
    int nearestImage = 0;

    // liczy dystans pomiędzy każdym wytrenowanym obrazem a wyświetloną twarzą testową
    for(int current = 0; current < numTrainedImages; current++)
    {
        double distanceSquare = 0;

        for(int eigen = 0; eigen < numEigenvalues; eigen++)
        {
            // oblicza dystans dla bieżącej wartości eigena
            float distance = projectedTestImage[eigen] - projectedTrainImage->data.fl[current * numEigenvalues + eigen];

            // kwadrat z dystansu dodajemy do ostatecznej wartości
            distanceSquare += distance * distance;
        }

        if(distanceSquare < leastDistanceSquare)
        {
            leastDistanceSquare = distanceSquare;
            nearestImage = current;
        }
    }

    return nearestImage;
}
