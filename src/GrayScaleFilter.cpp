#include "GrayScaleFilter.h"


bool GrayScaleFilter::run(const unsigned char *src, int w, int h, unsigned char *dst)
{
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned char g = src[1] * 0.114 + src[2] * 0.587 + src[3] * 0.299;
      dst[0] = g;        // b
      dst[1] = g;        // g
      dst[2] = g;        // r
      dst[3] = src[3];   // a
      dst += 4;
      src += 4;
    }
  }

  return true;
}
