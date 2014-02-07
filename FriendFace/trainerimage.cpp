#include "trainerimage.h"

TrainerImage::TrainerImage(Size goalSize, bool keepAspectRatio, const QString& directoryPath)
    :directoryPath(directoryPath), goalSize(goalSize), keepAspectRatio(keepAspectRatio)
{
    // jeśli użytkownik podał ścieżkę bez slasha, jest on dodawany
    QString::iterator i = this->directoryPath.end();
    if( this->directoryPath.size() > 0 && (*i != '/' && *i == '\\') )
    {
        this->directoryPath.append("/");
    }
}

Mat TrainerImage::processImage(Mat& image)
{
    Mat resized;
    Size originalSize = image.size();

    if(keepAspectRatio)
    {
        float ratio = static_cast<float>(goalSize.height) / originalSize.height;
        Size newSize((int) (originalSize.width * ratio), (int) (originalSize.height * ratio));

        if(newSize.height != goalSize.height)
        {
            newSize.height = goalSize.height;
        }

        resize(image, resized, newSize);

        if(resized.size().width != goalSize.width)
        {
            if(keepAspectRatio)
            {
                int delta = goalSize.width - resized.size().width;

                if(delta < 0)
                {
                    Rect clipRect(abs(delta) / 2, 0, goalSize.width, resized.size().height);
                    resized = resized(clipRect);
                }
                else if (delta > 0)
                {
                    Mat widened(goalSize, resized.type());
                    Rect widenRect(delta / 2, 0, goalSize.width, goalSize.height);
                    Mat widenedCenter = widened(widenRect);
                    resized.copyTo(widenedCenter);
                    resized = widened;
                }
            }
        }
    }
    else
    {
        resize(image, resized, goalSize);
    }

    Mat grayFrame;
    cvtColor(resized, grayFrame, CV_BGR2GRAY);

    return grayFrame;
}

bool TrainerImage::processAndSaveImage(Mat& image, const QString& name)
{
    Mat processed = processImage(image);
    return imwrite(directoryPath.toStdString() + name.toStdString(), processed);
}
