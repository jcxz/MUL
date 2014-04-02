#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>

#include <QThread>
#include <QMutex>
#include <string>
#include <opencv2/opencv.hpp>
#include "mattoqimage.h"
#include "Filter.h"

class Player : public QThread
{
    Q_OBJECT
private:
    cv::VideoCapture input;
    double playSpeed;

    cv::Mat frame;
    MatToQimage converter;

    QMutex filterMutex;
    bool end = false;
    Filter *filter = NULL;

protected:
    /*!
     \brief Thread main function
     This is called when trhead starts.
    */
    void run();

public:
    explicit Player(QObject *parent = 0);

    bool setVideoFile(QString fPath);


    void play();
    void stop();
    void pause();


    void setFilter(Filter *flt) {
        filterMutex.lock();
            if(filter != NULL)
                delete filter;

            filter = flt;
        filterMutex.unlock();
    }

    bool convertToFile(QString &fileName);
signals:

public slots:

};

#endif // PLAYER_H
