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

#ifndef GRAYSCALEFILTER_H
#define GRAYSCALEFILTER_H

#include "PointFilter.h"


class GrayScaleFilter : public PointFilter
{
  public:
    explicit GrayScaleFilter(QCLContext *ctx)
      : PointFilter(ctx, CLSRCFILE("grayscale.cl"), "grayscale")
    { }

    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) override;
};

#endif
