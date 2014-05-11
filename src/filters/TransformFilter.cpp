#include "TransformFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"



bool TransformFilter::init(void)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("transform.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernel
  m_kernel = m_program.createKernel("transform");
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create a memory object for the matrix
  m_matrix_ocl = m_ctx->createBufferCopy(m_matrix, sizeof(m_matrix), QCLMemoryObject::ReadOnly);
  if (m_matrix_ocl.isNull())
  {
    ERRORM("Failed to create matrix buffer");
    return false;
  }

  m_kernel.setArg(2, m_matrix_ocl);

  return (m_good = true);
}


bool TransformFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  m_kernel.setArg(0, src);
  m_kernel.setArg(1, dst);
  m_kernel.setGlobalWorkSize(w, h);

  if (m_matrix_changed)
  {
    m_matrix_ocl.write(m_matrix, sizeof(m_matrix));
    m_matrix_changed = false;
  }

  m_kernel.run();

  return true;
}
