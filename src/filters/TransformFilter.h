#ifndef TRANSFORMFILTER_H
#define TRANSFORMFILTER_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"


class TransformFilter : public Filter
{
  private:
    inline float M(int y, int x) const { return m_matrix[y * 3 + x]; }
    inline float & M(int y, int x) { return m_matrix[y * 3 + x]; }

  public:
    explicit TransformFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_program()
      , m_kernel()
      , m_matrix{ 1.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f }
      , m_matrix_ocl()
      , m_matrix_changed(false)
    { init(); }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;

    // TODO: invert the input matrix
    void setMatrix(float m11, float m12, float m13,
                   float m21, float m22, float m23)
    {
      M(0, 0) = m11; M(0, 1) = m12; M(0, 2) = m13;
      M(1, 0) = m21; M(1, 1) = m22; M(1, 2) = m23;
      m_matrix_changed = true;
    }

    // TODO: invert the input matrix
    void setMatrix(const float m[6])
    {
      for (int i = 0; i < 6; ++i) m_matrix[i] = m[i];
      m_matrix_changed = true;
    }

  private:
    bool init(void);

  private:
    QCLProgram m_program;
    QCLKernel m_kernel;
    float m_matrix[6];
    QCLBuffer m_matrix_ocl;
    bool m_matrix_changed;
};

#endif
