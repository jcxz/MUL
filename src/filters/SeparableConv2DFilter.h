#ifndef SEPARABLECONV2DFILTER_H
#define SEPARABLECONV2DFILTER_H

#include "ConvFilterBase.h"
#include "debug.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"
#include "opencl/qclimage.h"



class SeparableConv2DFilter : public ConvFilterBase
{
  public:
    explicit SeparableConv2DFilter(QCLContext *ctx, QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge)
      : ConvFilterBase(ctx)
      , m_kernel_horz()
      , m_kernel_vert()
      , m_filter_ocl_horz()
      , m_filter_ocl_vert()
      , m_filter_size_horz(INVALID_FILTER_SIZE)
      , m_filter_size_vert(INVALID_FILTER_SIZE)
      , m_tmp_buf()
      , m_tmp_buf_w(-1)
      , m_tmp_buf_h(-1)
    { init(mode); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

    bool setClampMode(QCLSampler::AddressingMode mode);

    bool setHorizontalFilterKernel(const float *kernel, int size)
    { return setFilterKernel(kernel, size, m_filter_ocl_horz, m_kernel_horz, m_filter_size_horz); }

    bool setVerticalFilterKernel(const float *kernel, int size)
    { return setFilterKernel(kernel, size, m_filter_ocl_vert, m_kernel_vert, m_filter_size_vert); }

    FilterKernelMapProxy<float> mapHorizontalFilterKernel(int size)
    { return mapFilterKernel(size, m_filter_ocl_horz, m_kernel_horz, m_filter_size_horz); }

    FilterKernelMapProxy<float> mapVerticalFilterKernel(int size)
    { return mapFilterKernel(size, m_filter_ocl_vert, m_kernel_vert, m_filter_size_vert); }

  private:
    bool init(QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge);

  private:
    QCLKernel m_kernel_horz;     // OpenCL kernel for horizontal direction
    QCLKernel m_kernel_vert;     // OpenCl kernel for vertical direction
    QCLBuffer m_filter_ocl_horz;
    QCLBuffer m_filter_ocl_vert;
    int m_filter_size_horz;
    int m_filter_size_vert;
    QCLImage2D m_tmp_buf;
    int m_tmp_buf_w;
    int m_tmp_buf_h;
};

#endif
