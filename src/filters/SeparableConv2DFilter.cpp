#include "SeparableConv2DFilter.h"
#include "opencl/qclcontext.h"

#include "debug.h"



bool SeparableConv2DFilter::init(QCLSampler::AddressingMode mode)
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

  // create kernels
  m_kernel_horz = m_program.createKernel("conv2D_horizontal");
  if (m_kernel_horz.isNull())
  {
    ERRORM("Failed to create horizontal 2D convolution kernel: " << m_program.log().toStdString());
    return false;
  }

  m_kernel_vert = m_program.createKernel("conv2D_vertical");
  if (m_kernel_vert.isNull())
  {
    ERRORM("Failed to create vertical 2D convolution kernel: " << m_program.log().toStdString());
    return false;
  }

  return (m_good = setClampMode(mode));
}


bool SeparableConv2DFilter::setClampMode(QCLSampler::AddressingMode mode)
{
  m_sampler = m_ctx->createSampler(false, mode, QCLSampler::Nearest);
  if (m_sampler.isNull())
  {
    ERRORM("Failed to create OpenCL sampler with given addressing mode: " << m_ctx->lastError());
    return false;
  }

  m_kernel_horz.setArg(SAMPLER_IDX, m_sampler);
  m_kernel_vert.setArg(SAMPLER_IDX, m_sampler);

  return true;
}


bool SeparableConv2DFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  // realokacia tmp bufferu
  if ((m_tmp_buf_w != w) || (m_tmp_buf_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);
    m_tmp_buf = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLImage2D::ReadWrite);
    if (m_tmp_buf.isNull())
    {
      ERRORM("Failed to create temporary buffer for separable convolution: " << m_ctx->lastError());
      return false;
    }
    m_tmp_buf_w = w;
    m_tmp_buf_h = h;
  }

  m_kernel_horz.setArg(SRC_IDX, src);
  m_kernel_horz.setArg(DST_IDX, m_tmp_buf);
  m_kernel_horz.setGlobalWorkSize(w, h);
  QCLEvent evt(m_kernel_horz.run());

  evt.waitForFinished();

  m_kernel_vert.setArg(SRC_IDX, m_tmp_buf);
  m_kernel_vert.setArg(DST_IDX, dst);
  m_kernel_vert.setGlobalWorkSize(w, h);
  m_kernel_vert.run();

  return true;
}
