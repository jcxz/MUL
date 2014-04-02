#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDebug>

#include <QObject>
#include <opencv2/opencv.hpp>
#include "player.h"
#include "mattoqimage.h"
#include "FilterGray.h"

#include <QtConcurrent/QtConcurrent>

class MainWindow;

class Controller : public QObject
{
    Q_OBJECT
private:
    //pointer to the singleton
    static Controller *singleton; /*!< Holds class one and only instance */

    /*!
     \brief Private constructor
    */
    Controller();

    /*!
     \brief Private destructor
    */
    ~Controller();

    /*!
     \brief private copy constructor
    to make sure no one can create a copy of instance
    */
    Controller(const Controller& other) {}

    /*!
     \brief private operator=
     to make sure no one can create a copy of instance
    */
    Controller& operator= (const Controller& other) {}


    MainWindow *mainWin; //pointer to main window, to get access to ui

    //GLRenderer renderer;
    MatToQimage converter;
    Player player;


public:
    /*!
     \brief Creates Controller instance
     It must be called on application start before any mathod of Controlled
     is called
    */
    static void instantiate()
    {
        if(singleton == 0)
            singleton = new Controller();
    }

    /*!
     \brief Gets access to singleton instance
     \return Controller Pointer to Controller instance
    */
    static Controller *ctrlInst()
    {
        return singleton;
    }

    /*!
     \brief Release the singleton instance of this controller
    */
    static void destroy()
    {
        if(singleton != 0) {
            delete singleton;
            singleton = 0;
        }
    }

    typedef enum{
        NONE_FLT = 0,
        GRAY_FLT
    }filterT;

    void setMainWindow(MainWindow *ptr) {
        mainWin = ptr;
    }

    void play();
    void stop();
    void pause();

    void renderFrame(cv::Mat &frame);

    void writeMsg(std::string msg);
    void openVideoFile(QString &fileName);

    void selectFilter(filterT flt);
    void convertToFile(QString &fileName);
    void controlsEnabled(bool value);
    void setMaxProgress(uint maxVal);
    void updateProgress(uint currVal);
signals:

public slots:

};

#endif // CONTROLLER_H
