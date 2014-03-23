#include "FilterGray.h"

#include <opencv2/core/core.hpp>



bool FilterGray::exec(cv::Mat & image)
{
  int in_step = image.step[1];
  int nrows = image.rows;
  int ncols = image.cols;

  for (int i = 0; i < nrows; ++i)
  {
    unsigned char * __restrict__ p_in = image.ptr<unsigned char>(i);

    for (int j = 0; j < ncols; ++j)
    {
      unsigned char g = (p_in[0] * 0.114 + p_in[1] * 0.587 + p_in[2] * 0.299) + 0.49999;
      p_in[0] = g;
      p_in[1] = g;
      p_in[2] = g;
      p_in += in_step;
    }
  }

  return true;
}
