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
  \file   mattoqimage.h
  \brief  MatToQimage class header file
*/

#ifndef MATTOQIMAGE_H
#define MATTOQIMAGE_H

#include <QImage>
#include <QVector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*!
 \brief Class for conterting cv::Mat image to QImage

*/
class MatToQimage
{
private:
//    MatToQimage();

    QVector<QRgb> colorTable;

    void initColorTable();
public:
    MatToQimage();
    QImage convert(const cv::Mat &mat);
    static cv::Mat qImageToMat(const QImage &src);
};

#endif // MATTOQIMAGE_H
