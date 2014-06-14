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

/*!
  \author Pavel Csoka (xcsoka00@stud.fit.vutbr.cz)
  \date   14.5.2013
  \file   mattoqimage.c
  \brief  MatToQimage class implementation file
*/

#include "mattoqimage.h"
#include <iostream>

MatToQimage::MatToQimage()
{
    initColorTable();
}

void MatToQimage::initColorTable()
{
    for(int i = 0; i < 256; i++) {
        colorTable.push_back(qRgb(i, i, i));
    }
}

QImage MatToQimage::convert(const cv::Mat &mat)
{
    cv::Mat tmp;
    QImage qImage;

    if(mat.channels() == 1) {
        tmp = mat;
        // convert cv::Mat to QImage
        qImage = QImage((const unsigned char*)tmp.data, tmp.cols,
                              tmp.rows, tmp.step, QImage::Format_Indexed8);//.copy();
        qImage.setColorTable(colorTable);
    } else {
        cv::cvtColor(mat, tmp, CV_BGR2RGB);

        // convert cv::Mat to QImage
        qImage = QImage((const unsigned char*)tmp.data, tmp.cols,
                        tmp.rows, tmp.step, QImage::Format_RGB888);//.copy();
    }

    return qImage.copy();
}

cv::Mat MatToQimage::qImageToMat(QImage const& src)
{
    cv::Mat tmp;
    cv::Mat dst;

    if(src.format() == QImage::Format_ARGB32) {
        tmp = cv::Mat(src.height(), src.width(), CV_8UC4,
                    (unsigned char*)src.bits(),src.bytesPerLine());
        cvtColor(tmp, dst,CV_BGRA2BGR);

    } else {
        tmp = cv::Mat(src.height(), src.width(), CV_8UC3,
                    (unsigned char*)src.bits(),src.bytesPerLine());

        cvtColor(tmp, dst,CV_RGB2BGR);
    }
     return dst;
}
