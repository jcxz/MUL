/*
 * Copyright (C) 2014 Pavel Csoka <xcsoka00@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>

#include <QThread>
#include <QMutex>
#include <string>
#include <opencv2/opencv.hpp>
#include "mattoqimage.h"
#include "Filter.h"
#include "glrenderer.h"
#include "mattoqimage.h"

#include "FilterPipeline.h"

/*!
 \brief Player class

 Processes video input, applies filters and sends frames to Controller to
 display them. Runs in separate thread.
*/
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

    GLRenderer renderer;

    FilterPipeline *pipeline = NULL;
    MatToQimage matToQimg;

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

    void setFilterPipeline(FilterPipeline *fltPipeline) {
        pipeline = fltPipeline;
    }


    void setFilter(Filter *flt) {
        filterMutex.lock();
            if(filter != NULL)
                delete filter;

            filter = flt;
        filterMutex.unlock();
    }

    cv::VideoCapture* getCurrInput() {
        return &input;
    }

    Filter* getCurrFilter() {
        return filter;
    }

signals:

public slots:

};

#endif // PLAYER_H
