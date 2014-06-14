/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

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
    static void invertMatrix(float *mat);

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
      invertMatrix(m_matrix);
      m_matrix_changed = true;
    }

    // TODO: invert the input matrix
    void setMatrix(const float m[6])
    {
      for (int i = 0; i < 6; ++i) m_matrix[i] = m[i];
      invertMatrix(m_matrix);
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
