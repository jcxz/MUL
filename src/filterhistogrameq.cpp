#include "filterhistogrameq.h"

FilterHistogramEq::FilterHistogramEq()
{
}


bool FilterHistogramEq::exec(cv::Mat & image)
{
    //convert to HSV
    cv::Mat imgHSV;
    cv::cvtColor(image, imgHSV, cv::COLOR_BGR2HSV);

    cv::Mat lum; //luminance
    cv::Mat eq_lum;

    std::vector<cv::Mat> spl;
    cv::split(imgHSV, spl);
    //extract luminance channel
    lum = spl[2];

    cv::Mat h = cv::Mat::zeros( 1, 256, CV_32SC1 ); //histogram
    eq_lum   = cv::Mat::zeros(lum.size(), CV_8UC1 );

    // image histogram

    for(int i = 0; i < lum.rows; i++) {
        for(int j = 0; j < lum.cols; j++) {
            h.at<int32_t>(lum.at<uchar>(i,j))++;
        }
    }


    // accumulated histogram

    for(int i = h.cols - 1; i >= 0; i--) {
        for(int j = 0; j < i; j++) {
            h.at<int32_t>(i) += h.at<int32_t>(j);
        }
    }


    // computation of new luminance values

    double hMin;
    cv::minMaxLoc(h, &hMin);

    double normCoef = lum.cols*lum.rows - hMin;

    for(int i = 0; i < h.cols; i++) {
        h.at<int32_t>(i) = cv::saturate_cast<int32_t>(((h.at<int32_t>(i) - hMin) / normCoef) * 255);
    }


    for(int i = 0; i < eq_lum.rows; i++) {
        for(int j = 0; j < eq_lum.cols; j++) {
            eq_lum.at<uchar>(i,j) = cv::saturate_cast<uchar>(h.at<int32_t>(lum.at<uchar>(i, j)));
        }
    }

    //merge new luminance values into one image
    cv::Mat tmp;
    spl[2] = eq_lum;
    cv::merge(spl, tmp);

    cv::cvtColor(tmp, image, cv::COLOR_HSV2BGR);

    return true;
}
