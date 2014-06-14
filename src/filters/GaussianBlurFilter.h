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

#ifndef GAUSSIANBLURFILTER_H
#define GAUSSIANBLURFILTER_H

#include "Filter.h"
#include "SeparableConv2DFilter.h"


class GaussianBlurFilter : public Filter
{
  public:
    explicit GaussianBlurFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_conv(ctx)
      , m_sigma(1.0f)
      , m_variance(0.0f)
      , m_size(3)
      , m_changed(true)
    { m_good = m_conv.isGood(); }

    void setSigma(double sigma) { m_sigma = sigma; m_changed = true; }
    void setVariance(double variance) { m_variance = variance; m_changed = true; }
    void setSize(int size) { m_size = size; m_changed = true; }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override
    {
      if (!isGood()) return false;
      if (m_changed)
      {
        if (!setConvKernels()) return false;
        m_changed = false;
      }
      return m_conv.runCL(src, w, h, dst);
    }

  private:
    bool setConvKernels(void);
    static void genGaussKernel1D(double sigma, double variance, float *dst, float size);

  private:
    SeparableConv2DFilter m_conv;
    double m_sigma;
    double m_variance;
    int m_size;
    bool m_changed;
};

#endif
