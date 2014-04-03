#ifndef CONVERTER_H
#define CONVERTER_H

#include <QThread>
#include "opencv2/opencv.hpp"
#include "Filter.h"

/*!
 \brief Converter class

 Converts video input to file using selected filter. Runs in separate thread.
*/
class Converter : public QThread
{
    Q_OBJECT

private:
    cv::VideoCapture *input;
    Filter *filter = NULL;
    QString fileName = "";
    bool end = false; //inicates whether to stop processing

public:
    explicit Converter(QObject *parent = 0);

    void convert(cv::VideoCapture *inputVideo, Filter *_filter, QString _fileName);

    void stop();

signals:
    void sendProgress(int);
    void sendMaxProgress(int);

public slots:

protected:
    /*!
     \brief Thread main function
     This is called when trhead starts.
    */
    void run();
};

#endif // CONVERTER_H
