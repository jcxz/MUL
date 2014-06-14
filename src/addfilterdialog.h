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
