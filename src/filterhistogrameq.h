#ifndef FILTERHISTOGRAMEQ_H
#define FILTERHISTOGRAMEQ_H

#include <opencv2/opencv.hpp>
#include "Filter.h"

class FilterHistogramEq : public Filter
{
public:
    FilterHistogramEq();

    virtual bool init(void) { return true; }

    virtual bool exec(cv::Mat & image);
};

#endif // FILTERHISTOGRAMEQ_H
