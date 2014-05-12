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

    void setBias(float bias)
    { m_kernel.setArg(BIAS_IDX, QVector4D(bias, bias, bias, bias)); }

    void setConvKernel(QCLBuffer & filter, int filter_w)
    {
      m_kernel.setArg(FILTER_IDX, filter);
      m_kernel.setArg(FILTER_SIZE_IDX, filter_w);
      m_kernel.setArg(FILTER_SIZE_HALF_IDX, (filter_w - 1) / 2);
      m_filter_size = filter_w;
      m_filter_ocl = filter;
    }

    bool setClampMode(QCLSampler::AddressingMode mode);

    /**
     * @brief setFilterKernel sets the filter kernel to be used for convolution
     * @param kernel the kernel to be used
     * @param size the width of the kernel (number of columns)
     * @return true on success
     */
    bool setFilterKernel(const float *kernel, int size)
    {
      return ConvFilterBase::setFilterKernel(kernel, size * size, size,
                                             m_filter_ocl, m_kernel, m_filter_size);
    }

    /**
     * @brief mapFilterKernel maps the convolution kernel's buffer to memory.
     * In case the buffer is not created yet, it will create with given width.
     * @param size the size width of the kernel
     * @return a poitner to the mapped memory region, or nullptr on error
     */
    float *mapFilterKernel(int size)
    {
      return ConvFilterBase::mapFilterKernel(size, size * size,
                                             m_filter_ocl, m_kernel, m_filter_size);
    }

    void unmapFilterKernel(float *ptr) { m_filter_ocl.unmap(ptr); }

  private:
    bool init(QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge);

  private:
    QCLKernel m_kernel;
    QCLBuffer m_filter_ocl;
    int m_filter_size;
};

#endif
