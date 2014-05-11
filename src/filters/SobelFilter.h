#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"
#include "opencl/qclimage.h"


class SobelFilter : public Filter
{
  public:
    explicit SobelFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_program()
      , m_kernel_x()
      , m_kernel_y()
      , m_kernel_combine()
      , m_tmp_buf_x()
      , m_tmp_buf_y()
      , m_tmp_buf_w(-1)
      , m_tmp_buf_h(-1)
    { init(); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

  private:
    bool init(void);

  private:
    QCLProgram m_program;
    QCLKernel m_kernel_x;
    QCLKernel m_kernel_y;
    QCLKernel m_kernel_combine;
    QCLImage2D m_tmp_buf_x;
    QCLImage2D m_tmp_buf_y;
    int m_tmp_buf_w;
    int m_tmp_buf_h;
};

#endif
