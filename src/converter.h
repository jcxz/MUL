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

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QThread>
#include "opencv2/opencv.hpp"
#include "Filter.h"
#include "FilterPipeline.h"
#include "mattoqimage.h"

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

    FilterPipeline *pipeline = NULL;
    MatToQimage matToQimg;

public:
    explicit Converter(QObject *parent = 0);

    void convert(cv::VideoCapture *inputVideo, Filter *_filter,
                 QString _fileName, FilterPipeline *fltPipeline);

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
