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

#include "SeparableConv2DFilter.h"
#include "opencl/qclcontext.h"

#include "debug.h"



bool SeparableConv2DFilter::init(QCLSampler::AddressingMode mode)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("convolution.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernels
  m_kernel_horz = m_program.createKernel("conv2D_horizontal");
  if (m_kernel_horz.isNull())
  {
    ERRORM("Failed to create horizontal 2D convolution kernel: " << m_program.log().toStdString());
    return false;
  }

  m_kernel_vert = m_program.createKernel("conv2D_vertical");
  if (m_kernel_vert.isNull())
  {
    ERRORM("Failed to create vertical 2D convolution kernel: " << m_program.log().toStdString());
    return false;
  }

  // set default bias value
  m_kernel_horz.setArg(BIAS_IDX, QVector4D(0.0f, 0.0f, 0.0f, 0.0f));
  m_kernel_vert.setArg(BIAS_IDX, QVector4D(0.0f, 0.0f, 0.0f, 0.0f));

  return (m_good = setClampMode(mode));
}


bool SeparableConv2DFilter::setClampMode(QCLSampler::AddressingMode mode)
{
  m_sampler = m_ctx->createSampler(false, mode, QCLSampler::Nearest);
  if (m_sampler.isNull())
  {
    ERRORM("Failed to create OpenCL sampler with given addressing mode: " << m_ctx->lastError());
    return false;
  }

  m_kernel_horz.setArg(SAMPLER_IDX, m_sampler);
  m_kernel_vert.setArg(SAMPLER_IDX, m_sampler);

  return true;
}


bool SeparableConv2DFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  // realokacia tmp bufferu
  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);
    m_tmp_buf = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf.isNull())
    {
      ERRORM("Failed to create temporary buffer for separable convolution: " << m_ctx->lastError());
      return false;
    }

    m_kernel_horz.setGlobalWorkSize(w, h);
    m_kernel_vert.setGlobalWorkSize(w, h);

    m_tmp_buf_w = w;
    m_tmp_buf_h = h;
  }

  //QCLEventList evt_list;

  m_kernel_horz.setArg(SRC_IDX, src);
  m_kernel_horz.setArg(DST_IDX, m_tmp_buf);
  m_kernel_horz.run();
  //evt_list.append(m_kernel_horz.run());

  m_kernel_vert.setArg(SRC_IDX, m_tmp_buf);
  m_kernel_vert.setArg(DST_IDX, dst);
  m_kernel_vert.run();
  //m_kernel_vert.run(evt_list);

  return true;
}


void SeparableConv2DFilter::setConvKernels(QCLBuffer & filter_horz, QCLBuffer & filter_vert, int filter_w)
{
  m_kernel_horz.setArg(FILTER_IDX, filter_horz);
  m_kernel_horz.setArg(FILTER_SIZE_IDX, filter_w);
  m_kernel_horz.setArg(FILTER_SIZE_HALF_IDX, (filter_w - 1) / 2);
  m_filter_size_horz = filter_w;
  m_filter_ocl_horz = filter_horz;

  m_kernel_vert.setArg(FILTER_IDX, filter_vert);
  m_kernel_vert.setArg(FILTER_SIZE_IDX, filter_w);
  m_kernel_vert.setArg(FILTER_SIZE_HALF_IDX, (filter_w - 1) / 2);
  m_filter_size_vert = filter_w;
  m_filter_ocl_vert = filter_vert;
}
