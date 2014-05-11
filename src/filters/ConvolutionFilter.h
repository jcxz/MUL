#ifndef CONVOLUTIONFILTER_H
#define CONVOLUTIONFILTER_H

#include "Filter.h"
#include "debug.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"



class ConvolutionFilter : public Filter
{
  protected:
    static constexpr int SRC_IDX = 0;
    static constexpr int DST_IDX = 1;
    static constexpr int SAMPLER_IDX = 2;
    static constexpr int FILTER_IDX = 3;
    static constexpr int FILTER_SIZE_IDX = 4;
    static constexpr int FILTER_SIZE_HALF_IDX = 5;

  private:
    static constexpr int INVALID_FILTER_SIZE = -1;

  private:
    template <typename T>
    class FilterKernelMapProxy
    {
      public:
        FilterKernelMapProxy(void *ptr, QCLBuffer & buf)
          : m_ptr((T*) ptr), m_buf(&buf)
        { assert(m_buf != nullptr); }

        ~FilterKernelMapProxy(void) { m_buf->unmap(m_ptr); }

        operator T*(void) const { return m_ptr; }
        T *ptr(void) const { return m_ptr; }

      private:
        T *m_ptr;
        QCLBuffer *m_buf;
    };

  public:
    explicit ConvolutionFilter(QCLContext *ctx, QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge)
      : Filter(ctx)
      , m_program()
      , m_kernel()
      , m_sampler()
      , m_filter_ocl()
      , m_filter_size(INVALID_FILTER_SIZE)
    { init(mode); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override
    {
      if (!isGood()) return false;
      //conv2D(src, w, h, dst);
      return true;
    }

    bool setClampMode(QCLSampler::AddressingMode mode);
    bool setFilterKernel(const float *kernel, int size);
    FilterKernelMapProxy<float> mapFilterKernel(int size);

  protected:
#if 0
    void conv2D(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
    {
      m_kernel.setArg(SRC_IDX, src);
      m_kernel.setArg(DST_IDX, dst);
      m_kernel.setGlobalWorkSize(w, h);
      m_kernel.run();
    }

    void conv1D(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
    {
      // realokacia tmp bufferu

      m_kernel_horz.setArg(SRC_IDX, src);
      m_kernel_horz.setArg(DST_IDX, m_tmp_buf);
      m_kernel_horz.setGlobalWorkSize(w, h);
      QCLEvent evt(m_kernel_horz.run());

      evt.waitForFinished();

      m_kernel_vert.setArg(SRC_IDX, m_tmp_buf);
      m_kernel_vert.setArg(DST_IDX, dst);
      m_kernel_vert.setGlobalWorkSize(w, h);
      m_kernel_vert.run();
    }
#endif
  private:
    bool init(QCLSampler::AddressingMode mode = QCLSampler::ClampToEdge);

  private:
    QCLProgram m_program;
    QCLKernel m_kernel;
    QCLSampler m_sampler;
    QCLBuffer m_filter_ocl;
    int m_filter_size;
};

#endif
