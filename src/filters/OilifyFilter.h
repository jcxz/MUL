#ifndef OILIFYFILTER_H
#define OILIFYFILTER_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"


class OilifyFilter : public Filter
{
  private:
    static constexpr int SRC_IDX = 0;
    static constexpr int DST_IDX = 1;
    static constexpr int RADIUS_IDX = 2;
    static constexpr int LEVELS_IDX = 3;

    static constexpr int DEFAULT_LEVELS = 30;
    static constexpr int DEFAULT_RADIUS = 4;

  public:
    explicit OilifyFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_program()
      , m_kernel()
    { init(); }

    void setRadius(int radius) { m_kernel.setArg(RADIUS_IDX, radius); }
    void setLevels(int levels) { m_kernel.setArg(LEVELS_IDX, levels); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

  private:
    bool init(void);

  private:
    QCLProgram m_program;
    QCLKernel m_kernel;
};

#endif
