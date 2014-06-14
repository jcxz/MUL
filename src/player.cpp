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

#include "player.h"
#include "controller.h"

void Player::run()
{
    double duration; //for time measurment
    QImage outQFrame, inQFrame;

    for(;;) {
        //time measurment
        duration = static_cast<double>(cv::getTickCount());

        input >> frame;

        if(end) {
            break;
        }
        //video reached its end
        if(frame.empty()) {
            input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video
            Controller::ctrlInst()->writeMsg("Stopped");
            break;
        }

        inQFrame = matToQimg.convert(frame);

        //if any filter is selected, use it
        filterMutex.lock();
//            if(filter)
//                filter->exec(frame);
            if(!pipeline->isEmpty())
                outQFrame = pipeline->run(inQFrame);
            else
               outQFrame = inQFrame;
        filterMutex.unlock();

        //Controller::ctrlInst()->renderFrame(frame);
        Controller::ctrlInst()->renderQFrame(outQFrame);
        //msleep(playSpeed);

        //time measurment
        duration = static_cast<double>(cv::getTickCount()) - duration;
        duration /= cv::getTickFrequency(); // time elapsed in seconds
        duration *= 1000; // time in ms

        //if the time of execution is lower than targeted time,
        //wait the rest of the time, else continue immediately
        if(duration < playSpeed) {
            msleep(playSpeed - duration);
        } else {
            //continue immediately
        }
    }
}

bool Player::setVideoFile(QString fPath) {
    if(!input.open(fPath.toStdString()))
        return false;

    playSpeed = 1000/input.get(CV_CAP_PROP_FPS); //play speed in milisecond

    //display the first frame ov the video
    input >> frame;
    Controller::ctrlInst()->renderFrame(frame);
    input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video

    return true;
}

void Player::play() {
    end = false;
    this->start(QThread::NormalPriority);
}

void Player::stop() {
    pause();
    input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video

    //display the first frame ov the video
    input >> frame;
    Controller::ctrlInst()->renderFrame(frame);
    input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video
}

void Player::pause() {
    end = true;
    //this->quit();//musi se nejdriv vyskocit z cyklu v run()
    this->wait();
}

Player::Player(QObject *parent) :
    QThread(parent)
{
}
