#ifndef GAUSSIANBLURFILTER_H
#define GAUSSIANBLURFILTER_H

#include "Filter.h"
#include "SeparableConv2DFilter.h"


class GaussianBlurFilter : public Filter
{
  public:
    explicit GaussianBlurFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_conv(ctx)
      , m_sigma(1.0f)
      , m_variance(0.0f)
      , m_size(3)
      , m_changed(true)
    { m_good = m_conv.isGood(); }

    void setSigma(double sigma) { m_sigma = sigma; m_changed = true; }
    void setVariance(double variance) { m_variance = variance; m_changed = true; }
    void setSize(int size) { m_size = size; m_changed = true; }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override
    {
      if (!isGood()) return false;
      if (m_changed)
      {
        if (!setConvKernels()) return false;
        m_changed = false;
      }
      return m_conv.runCL(src, w, h, dst);
    }

  private:
    bool setConvKernels(void);
    static void genGaussKernel1D(double sigma, double variance, float *dst, float size);

  private:
    SeparableConv2DFilter m_conv;
    double m_sigma;
    double m_variance;
    int m_size;
    bool m_changed;
};

#endif
