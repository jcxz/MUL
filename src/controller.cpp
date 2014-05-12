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
        converter.convert(player.getCurrInput(), player.getCurrFilter(),
                          fileName, pipeline);
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

void Controller::addFilter(std::string fltName, std::vector<std::string> args) {
    Filter *flt;
    std::string name;
    bool resume = false;

    if((flt = pipeline->addFilter2(fltName.c_str()))) {
        if((resume = player.isRunning())) {
            pause();
        }

        try {
        if (fltName == "grayscale")
        {
          name = "Grayscale";
        }
        else if (fltName ==  "sepia")
        {
          name = "Sepia";
        }
        else if (fltName == "colorinvert")
        {
          name = "Color inversion";
        }
        else if (fltName == "transform")
        {
          name = "Transformation";
          //set arguments
          std::string::size_type sz;
          TransformFilter *filter = dynamic_cast<TransformFilter*>(flt);
          float m[args.size()];
          for(int i = 0; i < args.size(); i++) {
              if(!args[i].empty())
                m[i] = stof(args[i], &sz);
              else m[i] = 0;
          }
          filter->setMatrix(m);
          //DEBUG
          for(int i = 0; i < args.size(); i++) {
              std::cout << m[i] << std::endl;
          }
        }
        else if (fltName == "conv2d")
        {
            name = "Convolution 2D";
            //set arguments
            //stored row by row
            std::string::size_type sz;
            Conv2DFilter *filter = dynamic_cast<Conv2DFilter*>(flt);
            float kernel[args.size()];
            //try {
            for(int i = 0; i < args.size(); i++) {
                if(!args[i].empty())
                  kernel[i] = stof(args[i], &sz);
                else kernel[i] = 0;
            }
//            } catch (const std::invalid_argument& ia) {
//                Controller::ctrlInst()->writeMsg(ia.w);
//            }

            for(int i = 0; i < args.size(); i++) {
                std::cout << kernel[i] << std::endl;
            }

            filter->setFilterKernel(kernel, sqrt(args.size()));
        }
        else if (fltName == "separableconv2d")
        {
            name = "Separable Convolution 2D";
            //set arguments
            std::string::size_type sz;
            SeparableConv2DFilter *filter = dynamic_cast<SeparableConv2DFilter*>(flt);
            int size = args.size() / 2;
            float horizKernel[size];
            float verticKernel[size];

            for(int i = 0; i < size; i++) {
                if(!args[i].empty()) {
                  horizKernel[i] = stof(args[i], &sz);
                  verticKernel[i] = stof(args[i + size], &sz);
                } else {
                    horizKernel[i] = 0;
                    verticKernel[i] = 0;
                }
            }
            //DEBUG
            for(int i = 0; i < size; i++) {
                std::cout << horizKernel[i] << ":vert=" << verticKernel[i] << std::endl;
            }
            filter->setHorizontalFilterKernel(horizKernel, size);
            filter->setVerticalFilterKernel(verticKernel, size);
        }
        else if (fltName == "sobel")
        {
            name = "Sobel Operator";


        }
        else if (fltName == "emboss")
        {
            name = "Embossing";

            //set arguments
            std::string::size_type sz;
            EmbossFilter *filter = dynamic_cast<EmbossFilter*>(flt);

            //DEBUG
            for(int i = 0; i < args.size(); i++) {
                std::cout << args[i] << std::endl;
            }

            //first gray level
            float grayLevel = stof(args[0], &sz);
            filter->setGrayLevel(grayLevel);

            //second size
            //0 = EmbossFilter::NORMAL
            //1 = EmbossFilter::LARGE
            switch(stoi(args[1], &sz)) {
            case 0: {
                filter->setSize(EmbossFilter::NORMAL);
            }break;
            case 1: {
                filter->setSize(EmbossFilter::LARGE);
            }break;
            }
        }
        else if (fltName == "dog")
        {
            name = "Difference of Gaussians";
            //set arguments
            std::string::size_type sz;
            DoGFilter *filter = dynamic_cast<DoGFilter*>(flt);
            //first factor
            float factor = stof(args[0], &sz);
            filter->setFactor(factor);

            //second bias
            float bias = stof(args[1], &sz);
            filter->setBias(bias);

            //DEBUG
           // for(int i = 0; i < args.size(); i++) {
                std::cout << factor << std::endl;
                std::cout << bias << std::endl;
            //}
        }
        else if (fltName == "gaussianblur")
        {
            name = "Gaussian Blur";
            //set arguments
            std::string::size_type sz;
            GaussianBlurFilter *filter = dynamic_cast<GaussianBlurFilter*>(flt);
            //first sigma
            double sigma = stod(args[0], &sz);
            filter->setSigma(sigma);

            //second variance
            double variance = stod(args[1], &sz);
            filter->setVariance(variance);

            //third size
            int size = stoi(args[2], &sz);
            filter->setSize(size);

            //DEBUG
            for(int i = 0; i < args.size(); i++) {
                std::cout << sigma << std::endl;
                std::cout << variance << std::endl;
                std::cout << size << std::endl;
            }
        }
        else {
            std::cerr << "Controller: Invalid filter name!" << std::endl;
        }


        filterVec.push_back(std::pair<std::string, Filter*>(name, flt));

        if(resume)
            play();

        Controller::ctrlInst()->writeMsg("Filter successfully added.");
        } catch (const std::invalid_argument) {
            Controller::ctrlInst()->writeMsg("Invalid filter argument (i.e. not a number)");
        }
    }else {
        std::cerr << "Controller: Error adding filter" << std::endl;
    }
}

void Controller::removeFilter(int index) {
    bool resume = false;
    if((resume = player.isRunning())) {
        pause();
    }

    pipeline->removeFilter(filterVec[index].second);
    filterVec.erase(filterVec.begin() + index);

    if(resume)
        play();
}

std::vector<std::pair<std::string, Filter*> > Controller::getActiveFilters() {
    return filterVec;
}
