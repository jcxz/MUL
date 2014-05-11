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
