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

#include "DoGFilter.h"
#include "opencl/qclcontext.h"



bool DoGFilter::init(void)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("dog.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernels
  m_kernel_sub = m_program.createKernel("dog_subtract");
  if (m_kernel_sub.isNull())
  {
    ERRORM("Failed to create dog_subtract kernel: " << m_ctx->lastError());
    return false;
  }

  m_kernel_sub_inv = m_program.createKernel("dog_subtract_inv");
  if (m_kernel_sub_inv.isNull())
  {
    ERRORM("Failed to create dog_subtract_inv kernel: " << m_ctx->lastError());
    return false;
  }

  // create convolution kernels
#if 1
  float kern1[] = { 1, 2, 1,
                    2, 4, 2,
                    1, 2, 1 };
  for (int i = 0; i < 9; ++i) kern1[i] /= 16.0f;

#if 0
  float kern2[] = { 2,  4,  5,  4, 2,
                    4,  9, 12,  9, 4,
                    5, 12, 15, 12, 5,
                    4,  9, 12,  9, 4,
                    2,  4,  5,  4, 2 };
  for (int i = 0; i < 25; ++i) kern2[i] /= 159.0f;
#else
  float kern2[] = { 1,  4,  6,  4, 1,
                    4, 16, 24, 16, 4,
                    6, 24, 36, 24, 6,
                    4, 16, 24, 16, 4,
                    1,  4,  6,  4, 1 };
  for (int i = 0; i < 25; ++i) kern2[i] /= 256.0f;
#endif

  m_conv_kern1 = m_ctx->createBufferCopy(kern1, sizeof(kern1), QCLBuffer::ReadOnly);
  if (m_conv_kern1.isNull())
  {
    ERRORM("Failed to create horizontal convolution kernel buffer 1: " << m_ctx->lastError());
    return false;
  }

  m_conv_kern2 = m_ctx->createBufferCopy(kern2, sizeof(kern2), QCLBuffer::ReadOnly);
  if (m_conv_kern2.isNull())
  {
    ERRORM("Failed to create horizontal convolution kernel buffer 2: " << m_ctx->lastError());
    return false;
  }

#else
  float kern1[] = { 2.0f / 8.0f, 4.0f / 8.0f, 2.0f / 8.0f };
  m_conv_kern1 = m_ctx->createBufferCopy(kern1, sizeof(kern1), QCLBuffer::ReadOnly);
  if (m_conv_kern1.isNull())
  {
    ERRORM("Failed to create horizontal convolution kernel buffer 1: " << m_ctx->lastError());
    return false;
  }

  float kern2[] = { 6.0f / 96.0f, 24.0f / 96.0f, 36.0f / 96.0f, 24.0f / 96.0f, 6.0f / 96.0f, };
  m_conv_kern2 = m_ctx->createBufferCopy(kern2, sizeof(kern2), QCLBuffer::ReadOnly);
  if (m_conv_kern2.isNull())
  {
    ERRORM("Failed to create horizontal convolution kernel buffer 2: " << m_ctx->lastError());
    return false;
  }
#endif

  setFactor(DEFAULT_FACTOR);
  setBias(DEFAULT_BIAS);

  return (m_good = true);
}


bool DoGFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  // realokacia tmp bufferov
  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);

    m_tmp_buf1 = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf1.isNull())
    {
      ERRORM("Failed to create temporary buffer for x direction of sobel oprator: " << m_ctx->lastError());
      return false;
    }

    m_tmp_buf2 = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf2.isNull())
    {
      ERRORM("Failed to create temporary buffer for y direction of sobel oprator: " << m_ctx->lastError());
      return false;
    }

    m_kernel_sub.setGlobalWorkSize(w, h);
    m_kernel_sub_inv.setGlobalWorkSize(w, h);

    m_tmp_buf_w = w;
    m_tmp_buf_h = h;
  }

#if 1
  if (!m_grayscale.runCL(src, w, h, dst)) return false;

  //m_conv.setConvKernels(m_conv_kern1, m_conv_kern1, CONV_KERN1_W);
  m_conv.setConvKernel(m_conv_kern1, CONV_KERN1_W);
  if (!m_conv.runCL(dst, w, h, m_tmp_buf1)) return false;

  //m_conv.setConvKernels(m_conv_kern2, m_conv_kern2, CONV_KERN2_W);
  m_conv.setConvKernel(m_conv_kern2, CONV_KERN2_W);
  if (!m_conv.runCL(dst, w, h, m_tmp_buf2)) return false;
#else
  m_conv.setConvKernel(m_conv_kern1, CONV_KERN1_W);
  if (!m_conv.runCL(src, w, h, m_tmp_buf1)) return false;
  m_conv.setConvKernel(m_conv_kern2, CONV_KERN2_W);
  if (!m_conv.runCL(src, w, h, m_tmp_buf2)) return false;
#endif

#if 1
  if (m_invert)
  {
    m_kernel_sub_inv.setArg(SRC1_IDX, m_tmp_buf1);
    m_kernel_sub_inv.setArg(SRC2_IDX, m_tmp_buf2);
    m_kernel_sub_inv.setArg(DST_IDX, dst);
    m_kernel_sub_inv.run();
  }
  else
  {
    m_kernel_sub.setArg(SRC1_IDX, m_tmp_buf1);
    m_kernel_sub.setArg(SRC2_IDX, m_tmp_buf2);
    m_kernel_sub.setArg(DST_IDX, dst);
    m_kernel_sub.run();
  }
#endif
  return true;
}
