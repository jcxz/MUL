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
    }
}
