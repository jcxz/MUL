/*!
  \author Pavel Csoka (xcsoka00@stud.fit.vutbr.cz)
  \date   14.5.2013
  \file   cameraplay.c
  \brief  CameraPlay class implementation file
*/

#include "cameraplay.h"
#include "controller.h"

CameraPlay::CameraPlay(QObject *parent) :
    QThread(parent)
{
    inputCapture = NULL;
    distRem = NULL;
    renderer = NULL;
    processingSpeed = 1000 / 30;
    undistort = false;
    warpPerspective = false;
    displayCorners = false;
    detectLaser = false;
    displayImage = true;
}

CameraPlay::CameraPlay(cv::VideoCapture *input, DistortionRemover *distRem_)
{
    inputCapture = input;
    distRem = distRem_;
    processingSpeed = 1000 / 30;
    undistort = false;
    warpPerspective = false;
    displayCorners = false;
    detectLaser = false;
    displayImage = true;
}

void CameraPlay::setEssentials(cv::VideoCapture *input, DistortionRemover *distRem_,
                               ProjectiveTransform *projTransform_,
                               LaserDetector *laserDetector_,
                               GLRenderer *renderer_, unsigned fps)
{
    inputCapture = input;
    distRem = distRem_;
    projTransform = projTransform_;
    laserDetector = laserDetector_;
    renderer = renderer_;
    this->setFPS(fps);
    //laserDetector->setInputCapture(inputCapture);
}

void CameraPlay::setDisplayImage(bool yes)
{
    displayImage = yes;
    if(renderer != NULL) {        
        if(yes) {
            renderer->play(processingSpeed);
        } else {
            renderer->stop();
        }
    }
}

void CameraPlay::stop()
{
    stopMutex.lock();
        stopped = true;
        this->quit();
        this->wait();
    stopMutex.unlock();
}

void CameraPlay::play()
{
    stopMutex.lock();
        if(displayImage)
            renderer->play(processingSpeed);
        stopped = false;
        this->start(QThread::HighPriority);
    stopMutex.unlock();
}

cv::Mat CameraPlay::getCurrentFrame()
{
    cv::Mat frame_;
    frameMutex.lock();
        frame_ = frame.clone();
    frameMutex.unlock();
    return frame_;
}

void CameraPlay::run()
{
    cv::Mat /*frame,*/ undistorted, warped, corners, tmp;
    double duration; //for time measurment

    if(inputCapture == NULL || distRem == NULL)
        cerr << "CameraPlay: inputCapture or distRem not set!" << endl;

    for(;;) {
        //time measurment
        duration = static_cast<double>(cv::getTickCount());
        if(stopped)
            break;

        if(!inputCapture->isOpened()) {
            cerr << "CameraPlay: Camera not opened." << endl;
            stopped = true;
            break;
        }
frameMutex.lock();
        (*inputCapture) >> frame;
#if 0
        cv::flip(frame, tmp, 1);
         frame =tmp;
#endif
        if(undistort) {
            undistorted = distRem->remap(frame);
            frame = undistorted;
        }

        if(warpPerspective) {
            warped = projTransform->applyPerspectiveTransform(frame);
            frame = warped;
        }

        if(detectLaser) {
            //laser detection
            laserDetector->detect(frame, displayImage);
        }

        if(displayImage) {
            if(displayCorners) {
                corners = projTransform->getFoundCorners();

                //            if(!corners.empty()) {
                //                cv::add(frame, corners, tmp);
                //                frame = tmp;
                //            }
                if(corners.empty()) {
                    corners = frame;
                }

                //emit sendImage(MatToQimage::convert(corners));
                //emit sendImage(Controller::ctrlInst()->convertMatToQimage(corners));
                renderer->renderImage(Controller::ctrlInst()->convertMatToQimage(corners));
            } else {
                renderer->renderImage(Controller::ctrlInst()->convertMatToQimage(frame));
                //emit sendImage(MatToQimage::convert(frame));
                //emit sendImage(Controller::ctrlInst()->convertMatToQimage(frame));
            }
        }
frameMutex.unlock();

        //time measurment
        duration = static_cast<double>(cv::getTickCount()) - duration;
        duration /= cv::getTickFrequency(); // time elapsed in seconds
        duration *= 1000; // time in ms
        //msleep(30);
        //qDebug("fps: %i", 1000/(static_cast<int>(duration) + 30));

        //if the time of execution is lower than targeted time,
        //wait the rest of the time, else continue immediately
        if(duration < processingSpeed) {
            msleep(processingSpeed - duration);
            //qDebug("fps: %i", static_cast<int>(1000/processingSpeed));
            emit sendPorcessingFPS(static_cast<int>(1000/processingSpeed));
        } else {
            //msleep(5);
            //qDebug("fps: %i", 1000/static_cast<int>(duration));
            emit sendPorcessingFPS(1000/static_cast<int>(duration));
        }
    }
}
