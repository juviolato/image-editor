#ifndef IMAGE
#define IMAGE

#include <iostream>
#include <cmath>
#include <QApplication>
#include <QtWidgets>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QImage>
#include <QColor>
#include <QMenu>
#include "utility.h"

#define FORMAT              "JPG"
#define LOADTITLE           "Load Image"
#define LOADPROMPT          "Please enter name of file to be loaded:"
#define SAVETITLE           "Save Image As"
#define SAVEPROMPT          "Please enter desired image name:"
#define QUANTTITLE          "Quantization"
#define QUANTPROMPT         "Quantize in how many shades?"
#define INPUTERROR          "Could not read input."
#define BRIGHTTITLE         "Adjusting Brightness"
#define BRIGHTPROMPT        "New brightness value:"
#define CONTTITLE           "Adjusting Contrast"
#define CONTPROMPT          "New contrast value:"
#define ZOOMTITLE           "Zooming Out"
#define ZOOMXPROMPT         "How much to zoom horizontally?"
#define ZOOMYPROMPT         "How much to zoom vertically?"
#define KERNELSIZE          3
#define GAUSSIAN            0
#define LAPLACIAN           1
#define HIGHPASS            2
#define PREWITTX            3
#define PREWITTY            4
#define SOBELX              5
#define SOBELY              6

const struct t_Filters
{
    double Gaussian[KERNELSIZE][KERNELSIZE];
    double Laplacian[KERNELSIZE][KERNELSIZE];
    double HighPass[KERNELSIZE][KERNELSIZE];
    double PrewittHx[KERNELSIZE][KERNELSIZE];
    double PrewittHy[KERNELSIZE][KERNELSIZE];
    double SobelHx[KERNELSIZE][KERNELSIZE];
    double SobelHy[KERNELSIZE][KERNELSIZE];
} filters = {{0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625},    // Gaussian kernel
             {0.0, -1.0, 0.0, -1.0, 4.0, -1.0, 0.0, -1.0, 0.0},                     // Laplacian kernel
             {-1.0, -1.0, -1.0, -1.0, 8.0, -1.0, -1.0, -1.0, -1.0},                 // HighPass kernel
             {-1.0, 0.0, 1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0},                      // PrewittHx kernel
             {-1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0},                      // PrewittHy kernel
             {-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0},                      // SobelHx kernel
             {-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0}};                     // SobelHy kernel



class Image : public QWidget
{
    Q_OBJECT

private:
    QImage original;
    QImage edited;
    QLabel *displayLabel;
    QWidget *displayWindow;
    bool isGrayscaled;

    void grayscaleEqualization();
    void coloredEqualization();
    void linearTransformation(int bias, int gain);

public:
    bool loadNewImage(QString fileName);
    void saveNewImage();
    void grayscale();
    void quantization();
    void flipHorizontal();
    void flipVertical();
    void createHistogram(QString title);
    void adjustBrightness();
    void adjustContrast();
    void negative();
    void equalizeHistogram();
    void histogramMatching();
    void zoomOut();
    void zoomIn();
    void rotateClockwise();
    void rotateCounterClockwise();
    void filter(int filterType);

    int height();
    int width();
    void setWindow(QLabel* label, QWidget* window);
    QImage getOriginal();
    QImage getEdited();
    void restoreOriginal();

    Image()
    {
        isGrayscaled = false;
        displayLabel = new QLabel();
        displayWindow = new QWidget();
    };

    ~Image()
    {
        displayLabel->~QLabel();
        displayWindow->~QWidget();
    };
};

#endif // IMAGE
