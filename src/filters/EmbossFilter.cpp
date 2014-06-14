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

#include "EmbossFilter.h"
#include "opencl/qclcontext.h"



#if 1
bool EmbossFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);
    m_tmp_buf = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf.isNull())
    {
      ERRORM("Failed to create temporary buffer for Emboss Filter: " << m_ctx->lastError());
      return false;
    }

    m_tmp_buf_w = w;
    m_tmp_buf_h = h;
  }


  if (m_gray_scale_filter.runCL(src, w, h, m_tmp_buf))
  {
    return m_conv2D_filter.runCL(m_tmp_buf, w, h, dst);
  }

  return false;
}
#endif
