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

__kernel void colorinvert(__read_only image2d_t src, __write_only image2d_t dst)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 color = read_imagef(src, sampler, coords);
  //color.xyz = (1.0f - color.xzy);
  
  //color.x = 1.0f - color.x;
  //color.y = 1.0f - color.y;
  //color.z = 1.0f - color.z;
  //write_imagef(dst, coords, color);
  float4 res = (float4) (1.0f - color.xyz, color.w);
  write_imagef(dst, coords, res);
}
