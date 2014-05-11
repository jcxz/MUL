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
