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
