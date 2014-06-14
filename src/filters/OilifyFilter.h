#ifndef OILIFYFILTER_H
#define OILIFYFILTER_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"
#include "opencl/qclimage.h"


#define OPTIMIZED_VER


class OilifyFilter : public Filter
{
  private:
    static constexpr int SRC_IDX = 0;
    static constexpr int DST_IDX = 1;
    static constexpr int RADIUS_IDX = 2;
    static constexpr int LEVELS_IDX = 3;

#ifdef OPTIMIZED_VER
    static constexpr int LEVELS_ONE_LESS_IDX = 2;
#endif
    static constexpr int DEFAULT_LEVELS = 30;
    static constexpr int DEFAULT_RADIUS = 4;

  public:
    explicit OilifyFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_program()
#ifndef OPTIMIZED_VER
      , m_kernel()
#else
      , m_kernel_oilify()
      , m_kernel_oilify_prepare()
      , m_tmp_buf()
      , m_tmp_buf_w(0)
      , m_tmp_buf_h(0)
#endif
    { init(); }

#ifndef OPTIMIZED_VER
    void setRadius(int radius) { m_kernel.setArg(RADIUS_IDX, radius); }
    void setLevels(int levels) { m_kernel.setArg(LEVELS_IDX, levels); }
#else
    void setRadius(int radius) { m_kernel_oilify.setArg(RADIUS_IDX, radius); }
    void setLevels(int levels)
    {
      m_kernel_oilify.setArg(LEVELS_IDX, levels);
      //m_kernel_oilify_prepare.setArg(LEVELS_IDX, levels);
      m_kernel_oilify_prepare.setArg(LEVELS_ONE_LESS_IDX, levels - 1);
    }
#endif

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

  private:
    bool init(void);

  private:
    QCLProgram m_program;
#ifndef OPTIMIZED_VER
    QCLKernel m_kernel;
#else
    QCLKernel m_kernel_oilify;
    QCLKernel m_kernel_oilify_prepare;
    QCLImage2D m_tmp_buf;
    int m_tmp_buf_w;
    int m_tmp_buf_h;
#endif
};

#endif
