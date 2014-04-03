#include "converter.h"
#include "controller.h"

Converter::Converter(QObject *parent) :
    QThread(parent)
{
}

void Converter::convert(cv::VideoCapture *inputVideo, Filter *_filter, QString _fileName) {
    input = inputVideo;
    filter = _filter;
    fileName = _fileName;
    end = false;
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

        if(filter)
            filter->exec(frame);

        out.write(frame);

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
