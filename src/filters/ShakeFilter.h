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

#ifndef SHAKEFILTER_H
#define SHAKEFILTER_H

#include "Filter.h"
#include "TransformFilter.h"
#include "opencl/qclcontext.h"
#include <ctime>
#include <cstdlib>

#include <chrono>
#include <random>


class ShakeFilter : public Filter
{
  public:
    explicit ShakeFilter(QCLContext *ctx)
      : Filter(ctx)
      , m_transform(ctx)
      , m_intensity(0.3f)
      //, m_rng(std::chrono::system_clock::now().time_since_epoch().count())
      //, m_distrib(0.0f, 1.5f)
      //, m_rng()
    {
      srand(time(nullptr));
      m_good = m_transform.isGood();
    }

    /**
     * @brief setIntensity sets shaking intensity
     * @param intensity this the intesity value from 0.0 to 1.0f,
     * 1.0f meaning the highest intensity and 0.0f meaning no shaking
     */
    void setIntensity(float intensity) { m_intensity = intensity; }

    virtual bool run(const unsigned char * /* src */, int /* w */, int /* h */, unsigned char * /* dst */) override
    { return false; }

    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) override
    {
      if (!isGood()) return false;

      if (random(0.0f, 1.0f) <= m_intensity)
      {
        float dx = random(-5.0f, 5.0f);
        float dy = random(-5.0f, 5.0f);
        //float dx = m_distrib(m_rng);
        //float dy = m_distrib(m_rng);
        m_transform.setMatrix(1.0f, 0.0f, dx,
                              0.0f, 1.0f, dy);

        return m_transform.runCL(src, w, h, dst);
      }
      else
      {
        return src.copyTo(QRect(0, 0, w, h), dst, QPoint(0, 0));
      }
    }

  private:
    static float random(float min, float max)
    {
      return (float(rand()) / float(RAND_MAX)) * (max - min) + min;
    }

  private:
    TransformFilter m_transform;
    float m_intensity;
#if 0
    std::default_random_engine m_rng;
    //std::uniform_real_distribution m_rng;
    std::normal_distribution<float> m_distrib;
#endif
};

#endif
