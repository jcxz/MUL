/*!
  \author Pavel Csoka (xcsoka00@stud.fit.vutbr.cz)
  \date   14.5.2013
  \file   mattoqimage.c
  \brief  MatToQimage class implementation file
*/

#include "mattoqimage.h"

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
