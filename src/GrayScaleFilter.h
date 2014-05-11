#ifndef GRAYSCALEFILTER_H
#define GRAYSCALEFILTER_H

#include "PointFilter.h"


class GrayScaleFilter : public PointFilter
{
  public:
    explicit GrayScaleFilter(QCLContext *ctx)
      : PointFilter(ctx, "clkernels/grayscale.cl", "grayscale")
    { }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override;
};

#endif
