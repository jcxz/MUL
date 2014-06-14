/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

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
