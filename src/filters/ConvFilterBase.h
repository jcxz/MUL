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

#ifndef CONVFILTERBASE_H
#define CONVFILTERBASE_H

#include "Filter.h"
#include "opencl/qclprogram.h"
#include "opencl/qclkernel.h"
#include "debug.h"



class ConvFilterBase : public Filter
{
  protected:
    static constexpr int SRC_IDX = 0;
    static constexpr int DST_IDX = 1;
    static constexpr int SAMPLER_IDX = 2;
    static constexpr int FILTER_IDX = 3;
    static constexpr int FILTER_SIZE_IDX = 4;
    static constexpr int FILTER_SIZE_HALF_IDX = 5;
    static constexpr int BIAS_IDX = 6;

    static constexpr int INVALID_FILTER_SIZE = -1;

  public:
    explicit ConvFilterBase(QCLContext *ctx)
      : Filter(ctx)
      , m_program()
      , m_sampler()
    { }

    virtual bool run(const unsigned char * src, int w, int h, unsigned char * dst) override = 0;
    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override = 0;

  protected:
    // Nastavuje
    bool setFilterKernel(const float *filter, int size, int filter_w,
                         QCLBuffer & filter_ocl,
                         QCLKernel & kernel_ocl,
                         int & filter_ocl_size);

    float *mapFilterKernel(int size, int filter_w,
                           QCLBuffer & filter_ocl,
                           QCLKernel & kernel_ocl,
                           int & filter_ocl_size);

  protected:
    QCLProgram m_program;
    QCLSampler m_sampler;
};

#endif
