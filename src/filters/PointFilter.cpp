#include "PointFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"



bool PointFilter::init(const char *program, const char *kernel)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(program);
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernel
  m_kernel = m_program.createKernel(kernel);
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create OpenCL kernel: " << m_program.log().toStdString());
    return false;
  }

  return (m_good = true);
}


bool PointFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;
  m_kernel.setArg(0, src);
  m_kernel.setArg(1, dst);
  m_kernel.setGlobalWorkSize(w, h);
  m_kernel.run();
  return true;
}
