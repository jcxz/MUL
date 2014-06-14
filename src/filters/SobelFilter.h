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
