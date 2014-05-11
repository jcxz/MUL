#ifndef CONV2DFILTER_H
#define CONV2DFILTER_H

#include "ConvFilterBase.h"
#include "debug.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"



class Conv2DFilter : public ConvFilterBase
{

  public:
    explicit Conv2DFilter(QCLContext *ctx, QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge)
      : ConvFilterBase(ctx)
      , m_kernel()
      , m_filter_ocl()
      , m_filter_size(INVALID_FILTER_SIZE)
    { init(mode); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

    bool setClampMode(QCLSampler::AddressingMode mode);

    bool setFilterKernel(const float *kernel, int size)
    { return ConvFilterBase::setFilterKernel(kernel, size, m_filter_ocl, m_kernel, m_filter_size); }

    FilterKernelMapProxy<float> mapFilterKernel(int size)
    { return ConvFilterBase::mapFilterKernel(size, m_filter_ocl, m_kernel, m_filter_size); }

  private:
    bool init(QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge);

  private:
    QCLKernel m_kernel;
    QCLBuffer m_filter_ocl;
    int m_filter_size;
};

#endif
