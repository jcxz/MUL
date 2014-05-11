#ifndef COLORINVERTFILTER_H
#define COLORINVERTFILTER_H

#include "PointFilter.h"


class ColorInvertFilter : public PointFilter
{
  public:
    explicit ColorInvertFilter(QCLContext *ctx)
      : PointFilter(ctx, CLSRCFILE("colorinvert.cl"), "colorinvert")
    { }
};

#endif
