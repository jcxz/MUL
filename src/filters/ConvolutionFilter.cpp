#include "ConvolutionFilter.h"
#include "opencl/qclcontext.h"

#include "debug.h"



bool ConvolutionFilter::init(QCLSampler::AddressingMode mode)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile("convolution.cl");
  if (m_program.isNull())
  {
    std::cerr << "Failed to create OpenCL program: " << m_program.log().toStdString() << std::endl;
    return false;
  }

  // create kernel
  m_kernel = m_program.createKernel("conv2D");
  if (m_kernel.isNull())
  {
    std::cerr << "Failed to create OpenCL program: " << m_program.log().toStdString() << std::endl;
    return false;
  }

  return (m_good = setClampMode(mode));
}


bool ConvolutionFilter::setClampMode(QCLSampler::AddressingMode mode)
{
  m_sampler = m_ctx->createSampler(true, mode, QCLSampler::Nearest);
  if (m_sampler.isNull())
  {
    ERRORM("Failed to create OpenCL sampler with given addressing mode: " << m_ctx->lastError());
    return false;
  }

  m_kernel.setArg(SAMPLER_IDX, m_sampler);

  return true;
}


bool ConvolutionFilter::setFilterKernel(const float *kernel, int size)
{
  if (size != m_filter_size)
  {
    m_filter_ocl = m_ctx->createBufferCopy(kernel, size * sizeof(float), QCLMemoryObject::ReadOnly);
    if (m_filter_ocl.isNull())
    {
      ERRORM("Failed to create convolution kernel buffer: " << m_ctx->lastError());
      return false;
    }

    m_filter_size = size;

    m_kernel.setArg(FILTER_IDX, m_filter_ocl);
    m_kernel.setArg(FILTER_SIZE_IDX, size);
    m_kernel.setArg(FILTER_SIZE_HALF_IDX, (size - 1) / 2);
  }
  else
  {
    if (!m_filter_ocl.write(kernel, size * sizeof(float)))
    {
      ERRORM("Failed to write convolution kernel data: " << m_ctx->lastError());
      return false;
    }
  }

  return true;
}


ConvolutionFilter::FilterKernelMapProxy<float> ConvolutionFilter::mapFilterKernel(int size)
{
  if (size != m_filter_size)
  {
    m_filter_ocl = m_ctx->createBufferDevice(size * sizeof(float), QCLBuffer::ReadOnly);
    if (m_filter_ocl.isNull())
    {
      ERRORM("Failed to create convolution kernel buffer: " << m_ctx->lastError());
      return FilterKernelMapProxy<float>(nullptr, m_filter_ocl);
    }

    m_filter_size = size;

    m_kernel.setArg(FILTER_IDX, m_filter_ocl);
    m_kernel.setArg(FILTER_SIZE_IDX, size);
    m_kernel.setArg(FILTER_SIZE_HALF_IDX, (size - 1) / 2);
  }

  return FilterKernelMapProxy<float>(m_filter_ocl.map(QCLBuffer::WriteOnly), m_filter_ocl);
}
