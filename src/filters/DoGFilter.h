#ifndef DOGFILTER_H
#define DOGFILTER_H

#include "Filter.h"
#include "GrayScaleFilter.h"
//#include "SeparableConv2DFilter.h"
#include "Conv2DFilter.h"
#include "opencl/qclimage.h"


class DoGFilter : public Filter
{
  private:
    static constexpr int SRC1_IDX = 0;
    static constexpr int SRC2_IDX = 1;
    static constexpr int DST_IDX = 2;
    static constexpr int FACTOR_IDX = 3;
    static constexpr int BIAS_IDX = 4;

    static constexpr int DEFAULT_FACTOR = 30.0f; //1.2f; //100.0f;
    static constexpr int DEFAULT_BIAS = 0.0f; //100.0f;

  public:
    explicit DoGFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_grayscale(ctx)
      , m_conv(ctx)
      , m_invert(false)
      , m_tmp_buf_w(-1)
      , m_tmp_buf_h(-1)
    { init(); }

    void setInverted(bool invert) { m_invert = invert; }
    void setFactor(float factor)
    {
      QVector4D v(factor, factor, factor, 0.0f);
      m_kernel_sub.setArg(FACTOR_IDX, v);
      m_kernel_sub_inv.setArg(FACTOR_IDX, v);
    }

    void setBias(float bias)
    {
      bias = bias / 255.0f;
      QVector4D v(bias, bias, bias, 1.0f);
      m_kernel_sub.setArg(BIAS_IDX, v);
      m_kernel_sub_inv.setArg(BIAS_IDX, v);
    }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

  private:
    bool init(void);

  private:
    static constexpr int CONV_KERN1_W = 3;
    static constexpr int CONV_KERN2_W = 5;

  private:
    GrayScaleFilter m_grayscale;
    //SeparableConv2DFilter m_conv;
    Conv2DFilter m_conv;

    QCLBuffer m_conv_kern1;
    QCLBuffer m_conv_kern2;

    QCLProgram m_program;
    QCLKernel m_kernel_sub;
    QCLKernel m_kernel_sub_inv;
    bool m_invert;

    QCLImage2D m_tmp_buf1;
    QCLImage2D m_tmp_buf2;

    int m_tmp_buf_w;
    int m_tmp_buf_h;
};

#endif
