#ifndef SEPIAFILTER_H
#define SEPIAFILTER_H

#include "PointFilter.h"


class SepiaFilter : public PointFilter
{
  public:
    explicit SepiaFilter(QCLContext *ctx)
      : PointFilter(ctx, CLSRCFILE("sepia.cl"), "sepia")
    { }
};

#endif
