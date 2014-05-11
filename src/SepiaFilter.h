#ifndef SEPIAFILTER_H
#define SEPIAFILTER_H

#include "PointFilter.h"


class SepiaFilter : public PointFilter
{
  public:
    explicit SepiaFilter(QCLContext *ctx)
      : PointFilter(ctx, "clkernels/sepia.cl", "sepia")
    { }
};

#endif
