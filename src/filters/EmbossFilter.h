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

#ifndef EMBOSSFILTER_H
#define EMBOSSFILTER_H

#include "Filter.h"
#include "GrayScaleFilter.h"
#include "Conv2DFilter.h"
#include "debug.h"
#include "opencl/qclimage.h"


class EmbossFilter : public Filter
{
  public:
    enum EmbossSize {
      NORMAL,
      LARGE
    };

    enum EmbossDirection {
    };

  public:
    explicit EmbossFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_gray_scale_filter(ctx)
      , m_conv2D_filter(ctx)
    {
      m_good = (m_gray_scale_filter.isGood() &&
                m_conv2D_filter.isGood() &&
                setSize(NORMAL));
                //setSize(LARGE));

      m_conv2D_filter.setBias(0.5f);
    }

    void setGrayLevel(float level) { m_conv2D_filter.setBias(level); }

    bool setSize(EmbossSize size)
    {
      if (size == LARGE)
      {
        static float kern[] = {  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
                                 1.0f,  1.0f,  1.0f,  0.0f, -1.0f,
                                 1.0f,  1.0f,  0.0f, -1.0f, -1.0f,
                                 1.0f,  0.0f, -1.0f, -1.0f, -1.0f,
                                 0.0f, -1.0f, -1.0f, -1.0f, -1.0f };
        return m_conv2D_filter.setFilterKernel(kern, 5);
      }
      else
      {
        static float kern[] = { 1.0f,  1.0f,  0.0f,
                                1.0f,  0.0f, -1.0f,
                                0.0f, -1.0f, -1.0f };
        return m_conv2D_filter.setFilterKernel(kern, 3);
      }
    }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override
    {
      //if (m_gray_scale_filter.run(src, w, h, dst))
      //{
      //  return m_conv2D_filter.run(dst, w, h, dst);
      //}
      return false;
    }

#if 1
    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override;
#else
    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override
    {
      // Pouzitie docasneho bufferu je istota, ale vizualne bolo toto takmer identicke,
      // asi GPU robi nejaku magiu ????
      if (m_gray_scale_filter.runCL(src, w, h, dst))
      {
        return m_conv2D_filter.runCL(dst, w, h, dst);
      }
      return false;
    }
#endif

  private:
    GrayScaleFilter m_gray_scale_filter;
    Conv2DFilter m_conv2D_filter;
    QCLImage2D m_tmp_buf;
    int m_tmp_buf_w;
    int m_tmp_buf_h;
};

#endif
