#include "friendface.h"

const Size FriendFace::testImageSize(150,200);

FriendFace::FriendFace(QWidget* parent) : QMainWindow(parent)
{
    setupFramework();
    setupGUI();
    setMinimumSize(QSize(640,480));

    frameCaptureThread = boost::thread(boost::bind(&FrameCapture::startCapturing, frameCapture));
    faceDetectorThread = boost::thread(boost::bind(&Detector::threadStart, faceDetector));

    Logger::getInstance().log("Gotowy...");
}

FriendFace::~FriendFace()
{
}

void FriendFace::setupFramework()
{
    frameCapture = shared_ptr<FrameCapture>(new FrameCapture(32));
    faceDetector = shared_ptr<FaceDetector>(new FaceDetector(/*DetailedFaceDetector::ALL,*/
                                                             "haarcascades/haarcascade_frontalface_alt.xml",
                                                             frameCapture.get(), false, 1.16));
    frameCapture->addFrameReceiver(faceDetector.get());
    recognizer = shared_ptr<EigenfaceRecognizer>(new EigenfaceRecognizer);
}

void FriendFace::closeEvent(QCloseEvent *event)
{
    faceDetector->removeController(webcamWidget);
    frameCapture->stopCapturing();
    faceDetector->requestTreadStop();
    frameCaptureThread.interrupt();
    faceDetectorThread.interrupt();
    frameCaptureThread.join();
    faceDetectorThread.join();
}

void FriendFace::setupGUI()
{
    webcamWidget = new WebcamWidget(this, frameCapture.get());
    faceDetector->addController(webcamWidget);

    logWidget = new QListWidget;

    captureTrainingImageButton = new QPushButton(tr("Przechwyć twarz i zapisz jako obraz treningowy"));
    connect(captureTrainingImageButton, SIGNAL(clicked()), this, SLOT(captureTrainingImage()));

    trainRecognizerButton = new QPushButton(tr("Trenuj rozpoznawacz"));
    connect(trainRecognizerButton, SIGNAL(clicked()), this, SLOT(trainRecognizer()));

    recognizeFaceButton = new QPushButton(tr("Rozpoznaj twarz"));
    connect(recognizeFaceButton, SIGNAL(clicked()), this, SLOT(recognizeFaces()));

    QWidget* centralWidget = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* widgetLayout = new QVBoxLayout;
    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    buttonsLayout->addWidget(captureTrainingImageButton);
    buttonsLayout->addWidget(trainRecognizerButton);
    buttonsLayout->addWidget(recognizeFaceButton);

    widgetLayout->addWidget(webcamWidget);
    widgetLayout->addLayout(buttonsLayout);

    mainLayout->addLayout(widgetLayout);
    mainLayout->addWidget(logWidget);

    centralWidget->setLayout(mainLayout);

    Logger::getInstance().setLogWidget(logWidget);

    setCentralWidget(centralWidget);
}

void FriendFace::captureTrainingImage()
{
    Mat frame;
    vector<Rect> faces;
    captureFrameAndFaces(faces, frame);

    if(faces.size() == 0)
    {
        QMessageBox::information(this, "Nie odnaleziono twarzy", "Detektor nie odnalazł żadnych twarzy!");
    }
    else
    {
        frame = frame.clone();
        TrainerImage convertor(testImageSize, true, "images/");
        Mat faceRegion;
        for(vector<Rect>::iterator face = faces.begin(); face != faces.end(); ++face)
        {
            faceRegion = frame(*face);
            QString filename = QInputDialog::getText(this,
                                                     tr("Nazwa obrazu"),
                                                     tr("wprowadź nazwę obrazka"));
            if(filename.size() < 1)
            {
                continue;
            }
            filename += ".bmp";
            if(!convertor.processAndSaveImage(faceRegion, filename))
            {
                QMessageBox::information(this, "Błąd", "Nie można przetworzyć i zapisac obrazu!");
            }
        }
    }
}

void FriendFace::trainRecognizer()
{
    path dir("images");
    directory_iterator end;

    for(directory_iterator file(dir); file != end; ++file)
    {
        if(is_regular_file(*file))
        {
            recognizer->addTrainingImage(QString::fromStdString(file->path().string()),
                                         QString::fromStdString(file->path().filename().string()));
        }
    }

    if(recognizer->train())
    {
        QMessageBox::information(this, "Sukces", "Rozpoznawacz zakończył uczenie!");
    }
    else
    {
        QMessageBox::information(this, "Error", "Nie udało się nauczyc Rozpoznawacza!");
    }
}

void FriendFace::recognizeFaces()
{
    if(recognizer->trained())
    {
        Mat frame;
        vector<Rect> faces;
        captureFrameAndFaces(faces, frame);

        if(faces.size() == 0)
        {
            QMessageBox::information(this, "Nie odnaleziono twarzy", "Nie onaleziono żadnych twarzy!");
        }
        else
        {
            frame = frame.clone();
            TrainerImage convertor(testImageSize, true);
            Mat faceRegion;
            Mat processedFace;

            for(vector<Rect>::iterator face = faces.begin(); face != faces.end(); ++face)
            {
                faceRegion = frame(*face);
                processedFace = convertor.processImage(faceRegion);
                QString name = recognizer->recognize(processedFace);
                Logger::getInstance().log(QString("Rozpoznano: ") + name);
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Rozpoznawacz nie jest nauczony", "Rozpoznawacz nie został nauczony, naucz Ropoznawacz!");
    }
}

void FriendFace::captureFrameAndFaces(vector<Rect> &rects, Mat& frame)
{
    rects = webcamWidget->getCurrentFaces();
    frame = webcamWidget->getCurrentFrame();
}
