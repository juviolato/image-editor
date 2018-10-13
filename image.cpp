#include "image.h"

bool Image::loadNewImage(QString fileName)
{
    bool oWorked, eWorked;

    oWorked = original.load(fileName);      // Saves original image for recovery purposes
    eWorked = edited.load(fileName);        // Saves image to be edited at will

    return oWorked && eWorked;
}

void Image::saveNewImage()
{
    QString imageName;
    imageName = readInput(SAVETITLE, SAVEPROMPT, INPUTERROR);

    if (imageName == nullptr)
        return;

    edited.save(imageName, FORMAT, 100);
}

void Image::grayscale()
{
    if (isGrayscaled)
        return;

    for (int i = 0; i < edited.width(); i++)
    {
        for (int j = 0; j < edited.height(); j++)
        {
            //  Read, recalculate and set a value for all channels
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);

            int grayValue = 299*pixelValue.red() + 587*pixelValue.green() + 114*pixelValue.blue();
            grayValue = static_cast<int>(round(grayValue/1000.0));
            pixelValue.setRed(grayValue);
            pixelValue.setGreen(grayValue);
            pixelValue.setBlue(grayValue);

            edited.setPixel(i, j, pixelValue.rgb());
        }
    }
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    isGrayscaled = true;
}

void Image::quantization()
{
    int numShades, w = edited.width(), h = edited.height();
    QString input = readInput(QUANTTITLE, QUANTPROMPT, INPUTERROR);

    if (input == nullptr)
        return;

    numShades = input.toInt();
    numShades = static_cast<int>(clamp(numShades, 1, MAXSHADES));
    int range = static_cast<int>(floor(MAXSHADES/numShades));

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);

            int grayValue = static_cast<int>(0.299*pixelValue.red())
                          + static_cast<int>(0.587*pixelValue.green())
                          + static_cast<int>(0.114*pixelValue.blue());
            // Recalculate pixel value to the middle of the interval it belongs to
            grayValue = static_cast<int>(floor(static_cast<double>(grayValue)/range)*range + floor(range/2));

            grayValue = static_cast<int>(clamp(grayValue, 0, 255));

            pixelValue.setRed(grayValue);
            pixelValue.setGreen(grayValue);
            pixelValue.setBlue(grayValue);

            edited.setPixel(i, j, pixelValue.rgb());
        }
    }
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::flipHorizontal()
{
    int w = edited.width();
    int h = edited.height();
    QImage flipped = QPixmap(w, h).toImage();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);
            flipped.setPixel(w-i-1, j, pixelValue.rgb());
        }
    }
    edited = flipped;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::flipVertical()
{
    int w = edited.width();
    int h = edited.height();
    QImage flipped = QPixmap(w, h).toImage();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);
            flipped.setPixel(i, h-j-1, pixelValue.rgb());
        }
    }
    edited = flipped;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::createHistogram(QString title)
{
    int histogram[MAXSHADES] = {0};

    calculateHistogram(edited, histogram, isGrayscaled);
    displayHistogram(histogram, title);
}

void Image::linearTransformation(double bias, double gain)
{
    int w = edited.width(), h = edited.height();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            int newValue;
            pixelValue = edited.pixel(i, j);

            newValue = static_cast<int>(gain*pixelValue.red() + bias);
            newValue = static_cast<int>(clamp(newValue, 0, 255));
            pixelValue.setRed(newValue);

            newValue = static_cast<int>(gain*pixelValue.green() + bias);
            newValue = static_cast<int>(clamp(newValue, 0, 255));
            pixelValue.setGreen(newValue);

            newValue = static_cast<int>(gain*pixelValue.blue() + bias);
            newValue = static_cast<int>(clamp(newValue, 0, 255));
            pixelValue.setBlue(newValue);

            edited.setPixel(i, j, pixelValue.rgb());
        }
    }
}

void Image::adjustBrightness()
{
    int bias;
    QString input = readInput(BRIGHTTITLE, BRIGHTPROMPT, INPUTERROR);

    if (input == nullptr)
        return;

    bias = input.toInt();
    bias = static_cast<int>(clamp(bias, -255, 255));

    linearTransformation(static_cast<double>(bias), 1);
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::adjustContrast()
{
    double gain;
    QString input = readInput(CONTTITLE, CONTPROMPT, INPUTERROR);

    if (input == nullptr)
        return;

    gain = input.toDouble();
    gain = clamp(gain, 0.001, 255.0);

    linearTransformation(0, gain);
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::negative()
{
    linearTransformation(255, -1);
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::grayscaleEqualization()
{
    int w = edited.width(), h = edited.height();

    int histogram[MAXSHADES], cumulative[MAXSHADES];
    calculateHistogram(edited, histogram, isGrayscaled);
    calcCumulativeHistogram(w*h, histogram, cumulative);

    createHistogram("Before equalization");

    // Use cumulative histogram as a function to find the new values
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue = edited.pixel(i, j);
            int newValue = cumulative[pixelValue.red()];

            pixelValue.setRed(static_cast<int>(clamp(newValue, 0.0, 255.0)));
            pixelValue.setGreen(static_cast<int>(clamp(newValue, 0.0, 255.0)));
            pixelValue.setBlue(static_cast<int>(clamp(newValue, 0.0, 255.0)));

            edited.setPixel(i, j, pixelValue.rgb());
        }
    }

    displayLabel->setPixmap(QPixmap::fromImage(edited));
    createHistogram("After equalization");
}

void Image::coloredEqualization()
{
    int w = edited.width(), h = edited.height();
    QImage gray = QPixmap(w, h).toImage();
    QImage equalized = QPixmap(w, h).toImage();
    int histogram[MAXSHADES], cumulative[MAXSHADES];

    gray = edited;
    calculateHistogram(gray, histogram, false);
    calcCumulativeHistogram(w*h, histogram, cumulative);

    for (int i = 0; i < w; i ++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue = edited.pixel(i, j);

            int newValue = cumulative[pixelValue.red()];
            newValue = static_cast<int>(clamp(newValue, 0.0, 255.0));
            pixelValue.setRed(newValue);

            newValue = cumulative[pixelValue.green()];
            newValue = static_cast<int>(clamp(newValue, 0.0, 255.0));
            pixelValue.setGreen(newValue);

            newValue = cumulative[pixelValue.blue()];
            newValue = static_cast<int>(clamp(newValue, 0.0, 255.0));
            pixelValue.setBlue(newValue);

            equalized.setPixel(i, j, pixelValue.rgb());
        }
    }
     edited = equalized;
     displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::equalizeHistogram()
{
    if (isGrayscaled)
        grayscaleEqualization();
    else
        coloredEqualization();
}

void Image::histogramMatching()
{
    if (!isGrayscaled)
        grayscale();

    // Matches current editing image's histogram to a different, user chosen image
    QString imageName;
    imageName = readInput(LOADTITLE, LOADPROMPT, INPUTERROR);

    if (imageName == nullptr)
        return;

    QImage newImage(imageName);
    int sourceHist[MAXSHADES], targetHist[MAXSHADES], sourceCumulative[MAXSHADES], targetCumulative[MAXSHADES];
    int matchingFunction[MAXSHADES];

    // Calculate histogram of image to match
    calculateHistogram(newImage, targetHist, false);
    int w = newImage.width();
    int h = newImage.height();
    calcCumulativeHistogram(w*h, targetHist, targetCumulative);

    // Calculate histogram of the image that is being edited
    calculateHistogram(edited, sourceHist, isGrayscaled);
    w = edited.width();
    h = edited.height();
    calcCumulativeHistogram(w*h, sourceHist, sourceCumulative);

    // For each pixel in editing image, find the closest value in the histogram to be matched
    for (int i = 0; i < MAXSHADES; i++)
    {
        int frequency = sourceCumulative[i];
        int closest = findClosestPositionInArray(frequency, targetCumulative, MAXSHADES);
        matchingFunction[i] = closest;
    }

    QImage matched = QPixmap(w, h).toImage();
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue = edited.pixel(i,j);
            int newValue = matchingFunction[pixelValue.red()];
            pixelValue.setRed(newValue);
            pixelValue.setGreen(newValue);
            pixelValue.setBlue(newValue);

            matched.setPixel(i, j, pixelValue.rgb());
        }
    }

    edited = matched;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::zoomOut()
{
    QString input = readInput(ZOOMTITLE, ZOOMXPROMPT, INPUTERROR);
    int sx = input.toInt();

    if (input == nullptr)
        return;

    input = readInput(ZOOMTITLE, ZOOMXPROMPT, INPUTERROR);
    int sy = input.toInt();

    if (input == nullptr)
        return;

    int w = edited.width(), h = edited.height();
    QImage zoomedOut = QPixmap(w/sx, h/sy).toImage();

    for (int i = 0; i < w-1; i+=sx)
    {
        for (int j = 0; j < h-1; j+=sy)
        {
            // Use a rectangle of size sx by sy to reduce image with factors sx and sy
            QColor originalPixel, newPixel;
            int newRed = 0, newGreen = 0, newBlue = 0;

            originalPixel = edited.pixel(i, j);
            // Calculate average of pixel values inside rectangle
            for (int k = 0; k < sx; k++)
            {
                for (int m = 0; m < sy; m++)
                {
                    newPixel = edited.pixel(i+k, j+m);
                    newRed += newPixel.red();
                    newGreen += newPixel.green();
                    newBlue += newPixel.blue();
                }
            }


            // Average becomes new pixel value (for each channel)
            newPixel.setRed(newRed/(sx*sy));
            newPixel.setGreen(newGreen/(sx*sy));
            newPixel.setBlue(newBlue/(sx*sy));

            zoomedOut.setPixel(i/sx, j/sy, newPixel.rgb());
        }
    }

    edited = zoomedOut;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    displayLabel->setGeometry(0, 0, edited.width(), edited.height());
    displayWindow->setGeometry(original.width()+70, 0, edited.width(), edited.height());
}

void Image::zoomIn()
{
    int w = edited.width(), h = edited.height();
    QImage zoomedIn = QPixmap(2*w-1, 2*h-1).toImage();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);
            zoomedIn.setPixel(2*i, 2*j, pixelValue.rgb());
        }
    }

    for (int i = 1; i < 2*w-1; i+=2)
    {
        for (int j = 0; j < 2*h-1; j+=2)
        {
            QColor pixelValue1, pixelValue2, newValue;
            pixelValue1 = zoomedIn.pixel(i-1, j);
            pixelValue2 = zoomedIn.pixel(i+1, j);

            newValue.setRed((pixelValue1.red() + pixelValue2.red())/2);
            newValue.setGreen((pixelValue1.green() + pixelValue2.green())/2);
            newValue.setBlue((pixelValue1.blue() + pixelValue2.blue())/2);

            zoomedIn.setPixel(i, j, newValue.rgb());
        }
    }

    for (int i = 0; i < 2*w-1; i+=2)
    {
        for (int j = 1; j < 2*h-1; j+=2)
        {
            QColor pixelValue1, pixelValue2, newValue;
            pixelValue1 = zoomedIn.pixel(i, j-1);
            pixelValue2 = zoomedIn.pixel(i, j+1);

            newValue.setRed((pixelValue1.red() + pixelValue2.red())/2);
            newValue.setGreen((pixelValue1.green() + pixelValue2.green())/2);
            newValue.setBlue((pixelValue1.blue() + pixelValue2.blue())/2);

            zoomedIn.setPixel(i, j, newValue.rgb());
        }
    }

    for (int i = 1; i < 2*w-1; i+=2)
    {
        for (int j = 1; j < 2*h-1; j += 2)
        {
            QColor pixel1, pixel2, pixel3, pixel4, newValue;
            pixel1 = zoomedIn.pixel(i-1, j);
            pixel2 = zoomedIn.pixel(i, j-1);
            pixel3 = zoomedIn.pixel(i+1, j);
            pixel4 = zoomedIn.pixel(i, j+1);

            newValue.setRed((pixel1.red() + pixel2.red() + pixel3.red() + pixel4.red())/4);
            newValue.setGreen((pixel1.green() + pixel2.green() + pixel3.green() + pixel4.green())/4);
            newValue.setBlue((pixel1.blue() + pixel2.blue() + pixel3.blue() + pixel4.blue())/4);

            zoomedIn.setPixel(i, j, newValue.rgb());
        }
    }

    edited = zoomedIn;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    displayLabel->setGeometry(0, 0, edited.width(), edited.height());
    displayWindow->setGeometry(original.width()+70, 0, edited.width(), edited.height());
}

void Image::rotateClockwise()
{
    int w = edited.width(), h = edited.height();
    QImage rotated = QPixmap(h, w).toImage();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);
            rotated.setPixel(h-j-1, i, pixelValue.rgb());
        }
    }

    edited = rotated;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    displayLabel->setGeometry(0, 0, edited.width(), edited.height());
    displayWindow->setGeometry(original.width()+70, 0, edited.width(), edited.height());
}

void Image::rotateCounterClockwise()
{
    int w = edited.width(), h = edited.height();
    QImage rotated = QPixmap(h, w).toImage();

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            QColor pixelValue;
            pixelValue = edited.pixel(i, j);
            rotated.setPixel(j, w-i-1, pixelValue.rgb());
        }
    }

    edited = rotated;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    displayLabel->setGeometry(0, 0, edited.width(), edited.height());
    displayWindow->setGeometry(original.width()+70, 0, edited.width(), edited.height());
}

void Image::filter(int filterType)
{
    grayscale();
    switch (filterType)
    {
    case GAUSSIAN:
        edited = convolution(edited, filters.Gaussian, filterType);
        break;
    case LAPLACIAN:
        edited = convolution(edited, filters.Laplacian, filterType);
        break;
    case HIGHPASS:
        edited = convolution(edited, filters.HighPass, filterType);
        break;
    case PREWITTX:
        edited = convolution(edited, filters.PrewittHx, filterType);
        break;
    case PREWITTY:
        edited = convolution(edited, filters.PrewittHy, filterType);
        break;
    case SOBELX:
        edited = convolution(edited, filters.SobelHx, filterType);
        break;
    case SOBELY:
        edited = convolution(edited, filters.SobelHy, filterType);
        break;
    }
    displayLabel->setPixmap(QPixmap::fromImage(edited));
}

void Image::setWindow(QLabel* label, QWidget* window)
{
    displayLabel = label;
    displayWindow = window;
}

QImage Image::getOriginal()
{
    return original;
}

QImage Image::getEdited()
{
    return edited;
}

void Image::restoreOriginal()
{
    edited = original;
    displayLabel->setPixmap(QPixmap::fromImage(edited));
    displayLabel->setGeometry(0, 0, edited.width(), edited.height());
    displayWindow->setGeometry(original.width()+70, 0, edited.width(), edited.height());
    isGrayscaled = false;
}

int Image::height()
{
    return edited.height();
}

int Image::width()
{
    return edited.width();
}
