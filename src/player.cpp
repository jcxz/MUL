#include "player.h"
#include "controller.h"

void Player::run()
{
    for(;;) {
        input >> frame;

        if(end || frame.empty())
            break;

        //if any filter is selected, use it
        filterMutex.lock();
            if(filter)
                filter->exec(frame);
        filterMutex.unlock();

        Controller::ctrlInst()->renderFrame(frame);
        msleep(playSpeed);
    }
}

/*bool Player::renderFrame()
{
    input >> frame;
    if(frame.empty())
        return false;

    Controller::ctrlInst()->renderFrame(frame);
}*/

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
    this->quit();//musi se nejdriv vyskocit z cyklu v run()
    this->wait();
}

bool Player::convertToFile(QString &fileName)
{
    // query for the video properties
    unsigned int frame_max = input.get(CV_CAP_PROP_FRAME_COUNT);
    Controller::ctrlInst()->setMaxProgress(frame_max); //set max value of progress bar
    int fourcc = input.get(CV_CAP_PROP_FOURCC);
    double fps = input.get(CV_CAP_PROP_FPS);
    cv::Size size(input.get(CV_CAP_PROP_FRAME_WIDTH), input.get(CV_CAP_PROP_FRAME_HEIGHT));

    // rewind video
    input.set(CV_CAP_PROP_POS_FRAMES, 0);

    std::cout << "Input video properties:" << std::endl;
    std::cout << "fourcc : " << fourcc << std::endl;
    std::cout << "fps    : " << fps << std::endl;
    std::cout << "size   : " << size << std::endl;

    // open the output video file
    cv::VideoWriter out(fileName.toStdString(), fourcc, fps, size);
    if (!out.isOpened())
    {
      std::cerr << "Failed to open output video file: " << fileName.toStdString() << std::endl;
      return false;
    }

    // process the individual video frames
    cv::Mat frame;

    //disable controls while converting
    Controller::ctrlInst()->controlsEnabled(false);

    for (unsigned int frame_i = 0; frame_i < frame_max; ++frame_i)
    {
      if (!input.read(frame))
      {
        std::cerr << "WARNING: An error occured while reading frame n. " << frame_i << std::endl;
        continue;
      }

      if(filter)
        filter->exec(frame);

      out.write(frame);

      if(frame_i % 100 == 0)
        Controller::ctrlInst()->updateProgress(frame_i); //update progress bar
    }

    Controller::ctrlInst()->updateProgress(frame_max - 1); //update progress bar

    //enable controls after converting
    Controller::ctrlInst()->controlsEnabled(false);

    // rewind video
    input.set(CV_CAP_PROP_POS_MSEC, 0);
    //display the first frame ov the video
    input >> frame;
    Controller::ctrlInst()->renderFrame(frame);
    input.set(CV_CAP_PROP_POS_FRAMES, 0); // rewind video

    return true;
}

Player::Player(QObject *parent) :
    QThread(parent)
{
}
