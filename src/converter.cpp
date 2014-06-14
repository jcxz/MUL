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

#include "converter.h"
#include "controller.h"

Converter::Converter(QObject *parent) :
    QThread(parent)
{
}

void Converter::convert(cv::VideoCapture *inputVideo, Filter *_filter,
                        QString _fileName, FilterPipeline *fltPipeline) {
    input = inputVideo;
    filter = _filter;
    fileName = _fileName;
    end = false;
    pipeline = fltPipeline;
    this->start(QThread::NormalPriority);
}

void Converter::stop()
{
    end = true;
    //this->quit();//musi se nejdriv vyskocit z cyklu v run()
    this->wait();
}

void Converter::run()
{
    if(input == NULL) {
        std::cerr << "Converter: No input file" << std::endl;
        //return false;
        Controller::ctrlInst()->convertFinished(false);
        return;
    }

    // query for the video properties
    unsigned int frame_max = input->get(CV_CAP_PROP_FRAME_COUNT);

    emit sendMaxProgress(frame_max);

    int fourcc = input->get(CV_CAP_PROP_FOURCC);
    double fps = input->get(CV_CAP_PROP_FPS);
    cv::Size size(input->get(CV_CAP_PROP_FRAME_WIDTH), input->get(CV_CAP_PROP_FRAME_HEIGHT));

    // rewind video
    input->set(CV_CAP_PROP_POS_FRAMES, 0);

   /* std::cout << "Input video properties:" << std::endl;
    std::cout << "fourcc : " << fourcc << std::endl;
    std::cout << "fps    : " << fps << std::endl;
    std::cout << "size   : " << size << std::endl;*/

    // open the output video file
    cv::VideoWriter out(fileName.toStdString(), fourcc, fps, size);
    if (!out.isOpened())
    {
        std::cerr << "Failed to open output video file: " << fileName.toStdString() << std::endl;
        //return false;
        Controller::ctrlInst()->convertFinished(false);
        return;
    }

    // process the individual video frames
    cv::Mat frame;
    QImage outQFrame, inQFrame;

    for (unsigned int frame_i = 0; frame_i < frame_max; ++frame_i)
    {
        if(end) {
            Controller::ctrlInst()->convertFinished(false);
            break;
        }

        if (!input->read(frame))
        {
            std::cerr << "WARNING: An error occured while reading frame n. " << frame_i << std::endl;
            continue;
        }

        //TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//        if(filter)
//            filter->exec(frame);
        inQFrame = matToQimg.convert(frame);

        if(!pipeline->isEmpty())
            outQFrame = pipeline->run(inQFrame);
        else
           outQFrame = inQFrame;

        //out.write(frame);
        out.write(matToQimg.qImageToMat(outQFrame));

        if(frame_i % 100 == 0)
            emit(sendProgress(frame_i)); //update progress bar
    }

    // rewind video
    input->set(CV_CAP_PROP_POS_MSEC, 0);


    if(!end) {
        emit(sendProgress(frame_max));
        //return true
        Controller::ctrlInst()->convertFinished(true);
    }
}
