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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "controller.h"
#include "addfilterdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;

    void setCLContext(QCLContext *ctx);
private slots:
    void on_btnPlay_clicked();

    void on_btnStop_clicked();

    void on_btnOpen_clicked();

    void on_btnPause_clicked();

    void on_rbGrayFlt_toggled(bool);

    void on_rbNoneFlt_toggled(bool);

    void on_btnConvert_clicked();

    void on_btnStopConvert_clicked();

    void on_rbHistFlt_toggled(bool);

    void on_pbAddFilter_clicked();

    void on_pbDeleteFilter_clicked();

private:
    AddFilterDialog addFltDlg;
    void displayCurrFilters();
    std::vector<std::string> fltNames;
};

#endif // MAINWINDOW_H
