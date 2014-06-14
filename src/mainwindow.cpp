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
    //initialise vital

    ui->statusBar->addWidget(ui->lblStatusBar);
    ui->lblStatusBar->setText("");

}

MainWindow::~MainWindow()
{
    Controller::destroy();
    delete ui;
}

void MainWindow::setCLContext(QCLContext *ctx) {
    Controller::ctrlInst()->setCLContext(ctx);
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
    if(fileName != NULL) {
        Controller::ctrlInst()->convertToFile(fileName);
    }
}

void MainWindow::on_btnStopConvert_clicked()
{
    Controller::ctrlInst()->stopConvertToFile();
}

void MainWindow::on_rbHistFlt_toggled(bool /*checked*/)
{
    Controller::ctrlInst()->selectFilter(Controller::HIST_FLT);
}

void MainWindow::displayCurrFilters() {
    //display list of active filters
    std::vector<std::pair<std::string, Filter*> > filterVec;
    filterVec = Controller::ctrlInst()->getActiveFilters();

    ui->lwFilters->clear();

    int i = 1;
    for(auto it = filterVec.begin(); it != filterVec.end(); it++) {
        ui->lwFilters->addItem(QString::number(i) + ": " + QString::fromStdString(it->first));
        i++;
    }
}

void MainWindow::on_pbAddFilter_clicked()
{
    if(addFltDlg.exec()) {
        std::cout << "addFilterDialog accepted" << std::endl;
        displayCurrFilters();
    }
}

void MainWindow::on_pbDeleteFilter_clicked()
{
    if(ui->lwFilters->currentRow() >= 0) {
        Controller::ctrlInst()->removeFilter(ui->lwFilters->currentRow());
        displayCurrFilters();
    }
}
