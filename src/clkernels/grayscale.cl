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

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_NONE |
                               CLK_FILTER_NEAREST;

__kernel void grayscale(__read_only image2d_t src, __write_only image2d_t dst)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 color = read_imagef(src, sampler, coords);
  color.xyz = (color.x * 0.299f + color.y * 0.587f + color.z * 0.114f);
  write_imagef(dst, coords, color);
}
