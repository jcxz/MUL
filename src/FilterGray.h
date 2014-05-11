#ifndef FILTERGRAY_H
#define FILTERGRAY_H

#include "Filter.h"
#include <opencv2/core/core.hpp>


class FilterGray : public Filter
{
  public:
    virtual bool init(void) { return true; }

    virtual bool exec(cv::Mat & image);

  private:
};

#endif
