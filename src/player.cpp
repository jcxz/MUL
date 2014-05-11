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

        if(end)
            break;
        //video reached its end
        if(frame.empty()) {
            input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video
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
