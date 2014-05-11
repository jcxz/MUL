#include "controller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

Controller *Controller::singleton = 0;

Controller::Controller()
{
}

void Controller::setCLContext(QCLContext *ctx) {
    // setup filter pipeline
    pipeline = new FilterPipeline(ctx);
    //pass pipeline to player
    player.setFilterPipeline(pipeline);
}

void Controller::setMainWindow(MainWindow *ptr) {
    mainWin = ptr;

    //connect signals from Converter to MainWindow
    connect(&converter, SIGNAL(sendMaxProgress(int)),
            mainWin->ui->pbConvert, SLOT(setMaximum(int)));
    connect(&converter, SIGNAL(sendProgress(int)),
            mainWin->ui->pbConvert, SLOT(setValue(int)));
}

Controller::~Controller()
{
    delete pipeline;
    stop(); //stop video playing thread
    //stop conversion thread
    converter.stop();
}

void Controller::renderFrame(const cv::Mat &frame)
{
    mainWin->ui->widget->renderPixmap(QPixmap::fromImage(matToQimg.convert(frame)));
}

void Controller::renderQFrame(const QImage &frame)
{
    mainWin->ui->widget->renderPixmap(QPixmap::fromImage(frame));
}

void Controller::play() {
    if(!player.isRunning())
        player.play();
    else
        std::cerr << "Controller: Trying to start already running thread of class Player" << std::endl;
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
    mainWin->ui->lblStatusBar->setText(QString::fromStdString(msg));
    qDebug() << msg.c_str();
}

void Controller::controlsEnabled(bool value)
{
    mainWin->ui->btnConvert->setEnabled(value);
    mainWin->ui->btnStopConvert->setEnabled(!value);
    mainWin->ui->btnOpen->setEnabled(value);

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
        //player.setFilter(new FilterGray());
        pipeline->addFilter("grayscale");
        break;
    case HIST_FLT:
        //player.setFilter(new FilterHistogramEq());
        break;
    default:
        player.setFilter(NULL);
    }
}

void Controller::convertToFile(QString &fileName)
{
    controlsEnabled(false);
    qDebug() << "CONVERT" << converter.isFinished();
    if(!converter.isRunning()) {
        pause(); //pause playing if it is running
        converter.convert(player.getCurrInput(), player.getCurrFilter(), fileName);
    }
    else
        std::cerr << "Controller: Trying to start already running thread of class Converter" << std::endl;
    //mainWin->ui->btnConvert->setText("");
}

void Controller::stopConvertToFile()
{
    converter.stop();
    converter.wait();
    controlsEnabled(true);
    qDebug() <<"STOP" << converter.isFinished();
}

void Controller::convertFinished(bool result)
{
    controlsEnabled(true);
    writeMsg(result ? "Video successfully converted" : "Video conversion incomplete!");
}

void Controller::addFilter(std::string fltName) {
    Filter *flt;
    if(flt = pipeline->addFilter2(fltName.c_str())) {
        filterVec.push_back(std::pair<std::string, Filter*>(fltName, flt));
    }else {
        std::cerr << "Controller: Error adding filter" << std::endl;
    }
}

void Controller::removeFilter(int index) {
    pipeline->removeFilter(filterVec[index].second);
}

std::vector<std::pair<std::string, Filter*> > Controller::getActiveFilters() {
    return filterVec;
}
