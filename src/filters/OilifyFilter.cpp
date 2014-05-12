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
  m_kernel = m_program.createKernel("oilify");
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create Oilify filter kernel: " << m_program.log().toStdString());
    return false;
  }

  setRadius(DEFAULT_RADIUS);
  setLevels(DEFAULT_LEVELS);

  return (m_good = true);
}



bool OilifyFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;
  m_kernel.setArg(SRC_IDX, src);
  m_kernel.setArg(DST_IDX, dst);
  m_kernel.setGlobalWorkSize(w, h);
  m_kernel.run();
  return true;
}
