// main.cpp
#include <QCoreApplication>
#include <QDebug>  // Include the necessary header for qDebug

#include "imageprocessor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 2) {
        qDebug() << "Usage: " << argv[0] << " <image_id>";
        return 1;  // Return an error code
    }

    bool conversionOk;
    int imageId = QString(argv[1]).toInt(&conversionOk);

    if (!conversionOk) {
        qDebug() << "Invalid image_id. Please provide a valid integer.";
        return 1;  // Return an error code
    }

    ImageProcessor imageProcessor;
    imageProcessor.processImage(imageId);

    return a.exec();
}
