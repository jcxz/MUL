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

#include "ConvFilterBase.h"
#include "opencl/qclcontext.h"



bool ConvFilterBase::setFilterKernel(const float *filter, int size, int filter_w,
                                     QCLBuffer & filter_ocl,
                                     QCLKernel & kernel_ocl,
                                     int & filter_ocl_size)
{
  if (size != filter_ocl_size)
  {
    filter_ocl = m_ctx->createBufferCopy(filter, size * sizeof(float), QCLMemoryObject::ReadOnly);
    if (filter_ocl.isNull())
    {
      ERRORM("Failed to create convolution kernel buffer: " << m_ctx->lastError());
      return false;
    }

    filter_ocl_size = size;

    kernel_ocl.setArg(FILTER_IDX, filter_ocl);
    kernel_ocl.setArg(FILTER_SIZE_IDX, filter_w);
    kernel_ocl.setArg(FILTER_SIZE_HALF_IDX, (filter_w - 1) / 2);
  }
  else
  {
    if (!filter_ocl.write(filter, size * sizeof(float)))
    {
      ERRORM("Failed to write convolution kernel data: " << m_ctx->lastError());
      return false;
    }
  }

  return true;
}


float *ConvFilterBase::mapFilterKernel(int size, int filter_w,
                                       QCLBuffer & filter_ocl,
                                       QCLKernel & kernel_ocl,
                                       int & filter_ocl_size)
{
  if (size != filter_ocl_size)
  {
    filter_ocl = m_ctx->createBufferDevice(size * sizeof(float), QCLBuffer::ReadOnly);
    if (filter_ocl.isNull())
    {
      ERRORM("Failed to create convolution kernel buffer: " << m_ctx->lastError());
      return nullptr;
    }

    filter_ocl_size = size;

    kernel_ocl.setArg(FILTER_IDX, filter_ocl);
    kernel_ocl.setArg(FILTER_SIZE_IDX, filter_w);
    kernel_ocl.setArg(FILTER_SIZE_HALF_IDX, (filter_w - 1) / 2);
  }

  return (float *) filter_ocl.map(QCLBuffer::WriteOnly);
}
