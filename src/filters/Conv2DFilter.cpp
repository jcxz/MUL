#include "Conv2DFilter.h"
#include "opencl/qclcontext.h"

#include "debug.h"



bool Conv2DFilter::init(QCLSampler::AddressingMode mode)
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

  // create kernel
  m_kernel = m_program.createKernel("conv2D");
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // set default bias value
  m_kernel.setArg(BIAS_IDX, QVector4D(0.0f, 0.0f, 0.0f, 0.0f));

  return (m_good = setClampMode(mode));
}


bool Conv2DFilter::setClampMode(QCLSampler::AddressingMode mode)
{
  m_sampler = m_ctx->createSampler(false, mode, QCLSampler::Nearest);
  if (m_sampler.isNull())
  {
    ERRORM("Failed to create OpenCL sampler with given addressing mode: " << m_ctx->lastError());
    return false;
  }

  m_kernel.setArg(SAMPLER_IDX, m_sampler);

  return true;
}


bool Conv2DFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;
  m_kernel.setArg(SRC_IDX, src);
  m_kernel.setArg(DST_IDX, dst);
  m_kernel.setGlobalWorkSize(w, h);
  m_kernel.run();
  return true;
}
