#include "SobelFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"



bool SobelFilter::init(void)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("sobel.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernels
  m_kernel_x = m_program.createKernel("sobel_x");
  if (m_kernel_x.isNull())
  {
    ERRORM("Failed to create sobel kernel for the horizontal direction: " << m_ctx->lastError());
    return false;
  }

  m_kernel_y = m_program.createKernel("sobel_y");
  if (m_kernel_y.isNull())
  {
    ERRORM("Failed to create sobel kernel for the vertical direction: " << m_ctx->lastError());
    return false;
  }

  m_kernel_combine = m_program.createKernel("sobel_combine");
  if (m_kernel_combine.isNull())
  {
    ERRORM("Failed to create sobel kernel for combining: " << m_ctx->lastError());
    return false;
  }

  return (m_good = true);
}


bool SobelFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  // realokacia tmp bufferu
  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);

    m_tmp_buf_x = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf_x.isNull())
    {
      ERRORM("Failed to create temporary buffer for x direction of sobel oprator: " << m_ctx->lastError());
      return false;
    }

    m_tmp_buf_y = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf_y.isNull())
    {
      ERRORM("Failed to create temporary buffer for y direction of sobel oprator: " << m_ctx->lastError());
      return false;
    }

    m_kernel_x.setGlobalWorkSize(w, h);
    m_kernel_y.setGlobalWorkSize(w, h);
    m_kernel_combine.setGlobalWorkSize(w, h);

    m_tmp_buf_w = w;
    m_tmp_buf_h = h;
  }

  QCLEventList evt_list;

  m_kernel_x.setArg(0, src);
  m_kernel_x.setArg(1, m_tmp_buf_x);
  evt_list.append(m_kernel_x.run());

  m_kernel_y.setArg(0, src);
  m_kernel_y.setArg(1, m_tmp_buf_y);
  evt_list.append(m_kernel_y.run());

  m_kernel_combine.setArg(0, m_tmp_buf_x);
  m_kernel_combine.setArg(1, m_tmp_buf_y);
  m_kernel_combine.setArg(2, dst);
  m_kernel_combine.run(evt_list);

  return true;
}
