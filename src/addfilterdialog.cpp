#include "addfilterdialog.h"
#include "ui_addfilterdialog.h"
#include "controller.h"

AddFilterDialog::AddFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFilterDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
//    ui->comboBox->addItem("Grayscale");
//    ui->comboBox->addItem("Sepia");
}

AddFilterDialog::~AddFilterDialog()
{
    delete ui;
}

void AddFilterDialog::on_buttonBox_accepted()
{
    std::vector<std::string> args;
    switch(ui->comboBox->currentIndex()) {
    case 0: {
        Controller::ctrlInst()->addFilter("grayscale", args);
    }break;
    case 1: {
        Controller::ctrlInst()->addFilter("sepia", args);
    }break;
    case 2: {
        Controller::ctrlInst()->addFilter("colorinvert", args);
    }break;
    case 3: {
        args.push_back(ui->leTrans1->text().toStdString());
        args.push_back(ui->leTrans2->text().toStdString());
        args.push_back(ui->leTrans3->text().toStdString());
        args.push_back(ui->leTrans4->text().toStdString());
        args.push_back(ui->leTrans5->text().toStdString());
        args.push_back(ui->leTrans6->text().toStdString());
        Controller::ctrlInst()->addFilter("transform", args);
    }break;
    case 4: {
        if(ui->tabConvKernel->columnCount() != ui->tabConvKernel->rowCount())
            std::cerr << "AddFilterDialog: Convolution kernel not square!"
                         << std::endl;
        //kernel values
        //stored row by row
        for(int i = 0; i < ui->tabConvKernel->rowCount(); i++) {
            for(int j = 0; j < ui->tabConvKernel->columnCount(); j++) {
                args.push_back(ui->tabConvKernel->item(i, i)->text().toStdString());
            }
        }
        Controller::ctrlInst()->addFilter("conv2d", args);
    }break;
    case 5: {
        //first horizontal kernel values
        for(int i = 0; i < ui->tabHorizKernel->columnCount(); i++) {
            args.push_back(ui->tabHorizKernel->item(0, i)->text().toStdString());
        }
        //then vertical kernel values
        for(int i = 0; i < ui->tabVerticalKernel->columnCount(); i++) {
            args.push_back(ui->tabVerticalKernel->item(0, i)->text().toStdString());
        }
        Controller::ctrlInst()->addFilter("separableconv2d", args);
    }break;
    case 6: {
        Controller::ctrlInst()->addFilter("sobel", args);
    }break;
    case 7: {
        Controller::ctrlInst()->addFilter("emboss", args);
    }break;
    }
}

void AddFilterDialog::on_spKernelSize_valueChanged(int arg1)
{
    ui->tabHorizKernel->setColumnCount(arg1);
    ui->tabVerticalKernel->setColumnCount(arg1);
}

void AddFilterDialog::on_spConvKernelSize_valueChanged(int arg1)
{
    ui->tabConvKernel->setColumnCount(arg1);
    ui->tabConvKernel->setRowCount(arg1);
}
