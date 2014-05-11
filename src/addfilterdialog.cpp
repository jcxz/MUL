#include "addfilterdialog.h"
#include "ui_addfilterdialog.h"
#include "controller.h"

AddFilterDialog::AddFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFilterDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItem("Grayscale");
    ui->comboBox->addItem("Sepia");
}

AddFilterDialog::~AddFilterDialog()
{
    delete ui;
}

void AddFilterDialog::on_buttonBox_accepted()
{
    switch(ui->comboBox->currentIndex()) {
    case 0: {
        Controller::ctrlInst()->addFilter("grayscale");
    }break;
    case 1: {
        Controller::ctrlInst()->addFilter("sepia");
    }break;
    }
}
