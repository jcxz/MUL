#include "GaussianBlurFilter.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



bool GaussianBlurFilter::setConvKernels(void)
{
  /*
  float *ptr = m_conv.mapHorizontalFilterKernel(m_size);
  if (ptr == nullptr) return false;
  genGaussKernel1D(m_sigma, m_variance, ptr, m_size);
  m_conv.unmapHorizontalFilterKernel(ptr);

  ptr = m_conv.mapVerticalFilterKernel(m_size);
  if (ptr == nullptr) return false;
  genGaussKernel1D(m_sigma, m_variance, ptr, m_size);
  m_conv.unmapVerticalFilterKernel(ptr);
  */

  float *ptr_horz = m_conv.mapHorizontalFilterKernel(m_size);
  if (ptr_horz == nullptr) return false;

  float *ptr_vert = m_conv.mapVerticalFilterKernel(m_size);
  if (ptr_vert == nullptr)
  {
    m_conv.unmapHorizontalFilterKernel(ptr_horz);
    return false;
  }

  genGaussKernel1D(m_sigma, m_variance, ptr_horz, m_size);

  for (int i = 0; i < m_size; ++i)
  {
    ptr_vert[i] = ptr_horz[i];
  }

  m_conv.unmapHorizontalFilterKernel(ptr_horz);
  m_conv.unmapVerticalFilterKernel(ptr_vert);

  return true;
}


void GaussianBlurFilter::genGaussKernel1D(double sigma, double variance, float *dst, float size)
{
  double a = 1.0f / (sigma * sqrt(2.0f * M_PI));
  double b_inv = -1.0f / (2.0f * sigma * sigma);
  double sum = 0.0f;

  for (int i = 0; i < size; ++i)
  {
    double tmp = i - variance;
    tmp = a * exp(b_inv * tmp * tmp);
    dst[i] = tmp;
    sum += tmp;
  }

  double norm = 1.0f / sum;
  for (int i = 0; i < size; ++i)
  {
    dst[i] *= norm;
  }
}
