#include "utility.h"

QString readInput(QString inputBoxTitle, QString inputBoxPrompt, QString readErrorMessage)
{
    QInputDialog *popup = new QInputDialog(NULL, NULL);     // Create popup window
    QString fileName;
    bool read;

    do
    {
        fileName = popup->getText(NULL, inputBoxTitle, inputBoxPrompt, QLineEdit::Normal, "", &read);       // Read user input from window

        if (read && fileName.isEmpty())
            QMessageBox::information(0, "ERROR", readErrorMessage);
    } while (read && fileName.isEmpty());

    if (read)
        return fileName;
    else
        return NULL;
}

float clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;

    return value;
}

int findClosestPositionInArray(int value, int array[], int size)
{
    int difference = INT_MAX;
    int position;

    for (int i = 0; i < size; i++)
    {
        int currDiff = fabs(array[i] - value);
        if (currDiff <= difference && array[i] > 0)
        {
            difference = currDiff;
            position = i;
        }
    }

    return position;
}

QImage convolution(QImage& image, const double kernel[KERNELSIZE][KERNELSIZE], int filterType)
{
    int w = image.width();
    int h = image.height();
    QColor pixelValue, newValue;
    QImage filtered = QPixmap(w, h).toImage();
    double rotatedKernel[KERNELSIZE][KERNELSIZE];

    // Rotates kernel 180º
    for (int i = 0; i < KERNELSIZE; i++)
    {
        for (int j = 0; j < KERNELSIZE; j++)
        {
            rotatedKernel[i][j] = kernel[KERNELSIZE-i-1][KERNELSIZE-j-1];
            rotatedKernel[KERNELSIZE-i-1][KERNELSIZE-j-1] = kernel[i][j];
        }
    }

    for (int i = 1; i < w-1; i++)
    {
        for (int j = 1; j < h-1; j++)
        {
            int newRed = 0, newGreen = 0, newBlue = 0;

            // Recalculates pixel value using kernel and neighbour pixels
            for (int m = 0; m < KERNELSIZE; m++)
            {
                for (int n = 0; n < KERNELSIZE; n++)
                {
                    pixelValue = image.pixel(i+n-1, j+m-1);

                    newRed += round(rotatedKernel[m][n] * (double)pixelValue.red());
                    newGreen += round(rotatedKernel[m][n] * (double)pixelValue.green());
                    newBlue += round(rotatedKernel[m][n] * (double)pixelValue.blue());
                }
            }

            if (filterType != GAUSSIAN && filterType != HIGHPASS)
            {
                newRed += 127;
                newGreen += 127;
                newBlue += 127;
            }

            newValue.setRed(clamp(newRed, 0, 255));
            newValue.setGreen(clamp(newGreen, 0, 255));
            newValue.setBlue(clamp(newBlue, 0, 255));
            filtered.setPixel(i, j, newValue.rgb());
        }
    }

    return filtered;
}

void calculateHistogram(QImage image, int histogram[MAXSHADES], bool isGrayscaled)
{
    QColor pixelValue;
    int w = image.width();
    int h = image.height();

    for (int i = 0; i < MAXSHADES; i++)
        histogram[i] = 0;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            pixelValue = image.pixel(i, j);
            int grayValue;

            // Uses the image in grayscale
            if (!isGrayscaled)
                grayValue = 0.299*pixelValue.red() + 0.587*pixelValue.green() + 0.114*pixelValue.blue();    // Grayscales only if it isn't already
            else
                grayValue = pixelValue.red();
            histogram[grayValue] += 1;
        }
    }
}

void calcCumulativeHistogram(int numPixels, int histogram[MAXSHADES], int cumulative[MAXSHADES])
{
    double alpha = 255.0/numPixels;

    cumulative[0] = round(alpha*(float)histogram[0]);
    for (int i = 1; i < MAXSHADES; i++)
        cumulative[i] = cumulative[i-1] + round(alpha*(float)histogram[i]);
}

void displayHistogram(int histogram[MAXSHADES], QString title)
{
    QImage graph = QPixmap(MAXSHADES, MAXSHADES).toImage();
    QColor black(0, 0, 0), white(255, 255, 255);
    int numPixels = *std::max_element(histogram, histogram+MAXSHADES);
    double alpha = 255.0/numPixels;

    for (int i = 0; i < MAXSHADES; i++)
       histogram[i] = round(alpha*histogram[i]);

    for (int i = 0; i < MAXSHADES; i++)
    {
        for (int j = 0; j < MAXSHADES; j++)
        {
            if (histogram[i] >= (MAXSHADES - j))
                graph.setPixel(i, j, black.rgb());
            else
                graph.setPixel(i, j, white.rgb());
        }
    }

    QWidget *histWindow = new QWidget();
    QLabel *histLabel = new QLabel(histWindow);
    histLabel->setPixmap(QPixmap::fromImage(graph));
    histWindow->setWindowTitle(title);
    histWindow->setGeometry(0, 0, MAXSHADES, MAXSHADES);
    histWindow->show();
}




