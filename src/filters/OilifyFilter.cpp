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

#include "OilifyFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"



bool OilifyFilter::init(void)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("oilify.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create Oilify filter program: " << m_program.log().toStdString());
    return false;
  }

  // create kernel
#ifndef OPTIMIZED_VER
  m_kernel = m_program.createKernel("oilify");
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create Oilify filter kernel: " << m_program.log().toStdString());
    return false;
  }
#else
  m_kernel_oilify = m_program.createKernel("oilify");
  if (m_kernel_oilify.isNull())
  {
    ERRORM("Failed to create Oilify filter kernel: " << m_program.log().toStdString());
    return false;
  }

  m_kernel_oilify_prepare = m_program.createKernel("oilify_prepare");
  if (m_kernel_oilify_prepare.isNull())
  {
    ERRORM("Failed to create oilify_prepare filter kernel: " << m_program.log().toStdString());
    return false;
  }
#endif

  setRadius(DEFAULT_RADIUS);
  setLevels(DEFAULT_LEVELS);

  return (m_good = true);
}



bool OilifyFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;
#ifndef OPTIMIZED_VER
  m_kernel.setArg(SRC_IDX, src);
  m_kernel.setArg(DST_IDX, dst);
  m_kernel.setGlobalWorkSize(w, h);
  m_kernel.run();
#else
  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Unnormalized_UInt8);
    m_tmp_buf = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf.isNull())
    {
      ERRORM("Failed to create temporary buffer for Oilify Filter: " << m_ctx->lastError());
      return false;
    }

    m_tmp_buf_w = w;
    m_tmp_buf_h = h;

    m_kernel_oilify_prepare.setGlobalWorkSize(w, h);
    m_kernel_oilify.setGlobalWorkSize(w, h);
  }

  m_kernel_oilify_prepare.setArg(SRC_IDX, src);
  m_kernel_oilify_prepare.setArg(DST_IDX, m_tmp_buf);
  m_kernel_oilify_prepare.run();

  m_kernel_oilify.setArg(SRC_IDX, m_tmp_buf);
  m_kernel_oilify.setArg(DST_IDX, dst);
  m_kernel_oilify.run();
#endif
  return true;
}
