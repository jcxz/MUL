#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //instantiate the controller
    Controller::instantiate();
    //set up displaying window
    Controller::ctrlInst()->setMainWindow(this);

    ui->statusBar->addWidget(ui->lblStatusBar);
    ui->lblStatusBar->setText("");
}

MainWindow::~MainWindow()
{
    Controller::destroy();
    delete ui;
}

void MainWindow::on_btnPlay_clicked()
{
    Controller::ctrlInst()->play();
}

void MainWindow::on_btnStop_clicked()
{
    Controller::ctrlInst()->stop();
}

void MainWindow::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Video File"), "", tr("Video Files (*.avi)"));
    if(fileName != NULL)
        Controller::ctrlInst()->openVideoFile(fileName);
}

void MainWindow::on_btnPause_clicked()
{
    Controller::ctrlInst()->pause();
}

void MainWindow::on_rbGrayFlt_toggled(bool /*checked*/)
{
    Controller::ctrlInst()->selectFilter(Controller::GRAY_FLT);
}

void MainWindow::on_rbNoneFlt_toggled(bool /*checked*/)
{
    Controller::ctrlInst()->selectFilter(Controller::NONE_FLT);
}

void MainWindow::on_btnConvert_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Convert Video To File"),
                                "./converted.avi",
                                tr("Video Files (*.avi)"));
    if(fileName != NULL)
        Controller::ctrlInst()->convertToFile(fileName);
}

void MainWindow::on_btnStopConvert_clicked()
{
    Controller::ctrlInst()->stopConvertToFile();
}

void MainWindow::on_rbHistFlt_toggled(bool /*checked*/)
{
    Controller::ctrlInst()->selectFilter(Controller::HIST_FLT);
}
