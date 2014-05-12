#ifndef ADDFILTERDIALOG_H
#define ADDFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class AddFilterDialog;
}

class AddFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFilterDialog(QWidget *parent = 0);
    ~AddFilterDialog();

private slots:
    void on_buttonBox_accepted();

    void on_spKernelSize_valueChanged(int arg1);

    void on_spConvKernelSize_valueChanged(int arg1);

private:
    Ui::AddFilterDialog *ui;
};

#endif // ADDFILTERDIALOG_H
