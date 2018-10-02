#ifndef UTILITY
#define UTILITY

#include <iostream>
#include <cmath>
#include <QtWidgets>
#include <QMessageBox>
#include <QInputDialog>

#define KERNELSIZE  3
#define GAUSSIAN    0
#define HIGHPASS    2
#define MAXSHADES   256

QString readInput(QString inputBoxTitle, QString inputBoxPrompt, QString readErrorMessage);

float clamp(float value, float min, float max);

int findClosestPositionInArray(int value, int array[], int size);

QImage convolution(QImage& image, const double kernel[KERNELSIZE][KERNELSIZE], int option);

void calculateHistogram(QImage image, int histogram[MAXSHADES], bool isGrayscaled);

void calcCumulativeHistogram(int numPixels, int histogram[MAXSHADES], int cumulative[MAXSHADES]);

void displayHistogram(int histogram[MAXSHADES], QString title);

#endif // UTILITY

