#include "controller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Controller *Controller::singleton = 0;

Controller::Controller()
{
}

Controller::~Controller()
{
    stop();
}

void Controller::renderFrame(cv::Mat &frame)
{
    mainWin->ui->label->setPixmap(
                QPixmap::fromImage(
                    converter.convert(frame)).scaled(mainWin->ui->label->width(),
                                                     mainWin->ui->label->height(),
                                                     Qt::KeepAspectRatio));
}

void Controller::play() {
    player.play();
}

void Controller::stop() {
    player.stop();
}

void Controller::pause() {
    player.pause();
}

void Controller::openVideoFile(QString &fileName)
{
    if(!player.setVideoFile(fileName)) {
        writeMsg("Player: Error opening video file");
        controlsEnabled(false);
    }
    else {
        controlsEnabled(true);
        writeMsg("Video successfully opened");
    }
}

void Controller::writeMsg(std::string msg)
{
    //write msg to status bar
    mainWin->ui->statusBar->setToolTip(QString::fromStdString(msg));
    qDebug() << msg.c_str();
}

void Controller::updateProgress(uint currVal)
{
    mainWin->ui->pbConvert->setValue(currVal);
}

void Controller::setMaxProgress(uint maxVal)
{
    mainWin->ui->pbConvert->setMaximum(maxVal);
}

void Controller::controlsEnabled(bool value)
{
    mainWin->ui->btnConvert->setEnabled(value);
    mainWin->ui->btnPause->setEnabled(value);
    mainWin->ui->btnPlay->setEnabled(value);
    mainWin->ui->btnStop->setEnabled(value);
}

void Controller::selectFilter(filterT flt)
{
    switch(flt) {
    case NONE_FLT:
        player.setFilter(NULL);
        break;
    case GRAY_FLT:
        player.setFilter(new FilterGray());
        break;
    default:
        player.setFilter(NULL);
    }
}

void Controller::convertToFile(QString &fileName)
{
    //run convertion in separate thread
     QFuture<void> convertThread = QtConcurrent::run(&player, &Player::convertToFile, fileName);
}
