#ifndef POINTFILTER_H
#define POINTFILTER_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"


class PointFilter : public Filter
{
  public:
    PointFilter(QCLContext *ctx, const char *program, const char *kernel)
      : Filter(ctx)
      , m_program()
      , m_kernel()
    { init(program, kernel); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

  private:
    bool init(const char *program, const char *kernel);

  private:
    QCLProgram m_program;
    QCLKernel m_kernel;
};

#endif
