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
};

#endif // MATTOQIMAGE_H
