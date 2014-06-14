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

#define EXPERIMENTAL

#ifndef EXPERIMENTAL
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               //CLK_ADDRESS_CLAMP |
                               CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_NEAREST;

#define MAX_LEVELS 100

// tato funkcia pixelu priradi priemernu farbu z najcastejsie sa vyskytujucej urovne jasu
// 1.) Najprv vypocita intenzitu kazdeho pixlu v okoli pocitaneho pixlu a nakvantuje
//     ich podla poctu zadanych urovni
// 2.) Potom pixeli v okoli roztriedi do tabulky, ktora urcuje castost vyskytu
//     kazdej intenzitnej urovne v okoli pocitaneho pixelu
// 3.) Nakoniec pixelu priradi priemer z pixelov v najcastejsie sa vyskytujucej urovni jasu
__kernel void oilify(__read_only image2d_t src,
                     __write_only image2d_t dst,
                     int radius,
                     int levels)
{
  //float4 *hist = (float4 *) alloca(sizeof(float4) * (levels));
  float4 hist[MAX_LEVELS];

  for (int i = 0; i < levels; ++i)
  {
    hist[i] = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  }
  
  float max_inten = 0.0f;
  int max_idx = 0;
  levels = levels - 1;
  
  int2 coords = (int2) (get_global_id(0), get_global_id(1));

  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      float4 color = read_imagef(src, sampler, coords + (int2) (i, j));
      //int inten = (int) round((color.x + color.y + color.z) / (3.0f * levels));
      int inten = (int) round((color.x + color.y + color.z) * levels / 3.0f);
      //int inten = (int) round(((double) (src[fidx] + src[fidx + 1] + src[fidx + 2]) / 3.0 * (levels)) / 255.0);
       
      hist[inten].xyz += color.xyz;
      hist[inten].w += 1;   // toto mi pocita pocet vyskytov daneho intenzitneho levelu
       
      if (hist[inten].w > max_inten)
      {
        max_inten = hist[inten].w;
        max_idx = inten;
      }
    }
  }
  
  // toto mi vypocita priemernu farbu pixelov najcastejsej intenzity 
  float4 res = (float4) (hist[max_idx].xyz / max_inten, 1.0f);
  
  write_imagef(dst, coords, clamp(res, 0.0f, 1.0f));
}



#else


// Experimental optimized version


__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP |
                               //CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_NEAREST;

#define MAX_LEVELS 30 //100

__kernel void oilify_prepare(__read_only image2d_t src,
                             __write_only image2d_t dst,
                             int levels_one_less)   // toto je rovne (levels - 1)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 color = read_imagef(src, coords);

  uint4 res = convert_uint4(color * 255);
  res.w = (uint) round((color.x + color.y + color.z) * levels_one_less * (1.0f / 3.0f));      // pouzit native_divide

  write_imageui(dst, coords, res);
}


__kernel void oilify(__read_only image2d_t src,
                     __write_only image2d_t dst,
                     int radius,
                     int levels)
{
  uint4 hist[MAX_LEVELS];

  for (int i = 0; i < levels; ++i)
  {
    hist[i] = (uint4) (0, 0, 0, 0);
  }

  uint4 max = 0;
  int2 coords = (int2) (get_global_id(0), get_global_id(1));

  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      uint4 color = read_imageui(src, sampler, coords + (int2) (i, j));
      
      // mozno pouzit prefetching, alebo transponovat stlpce

      hist[color.w].xyz += color.xyz;
      hist[color.w].w += 1;

      if (hist[color.w].w > max.w)
      {
        max = hist[color.w];
      }
    }
  }

  float4 res = convert_float4(max);
  res = native_divide(res, res.w);                // pouzit native_divide
  res *= (1.0f / 255.0f);
  res.w = 1.0f;

  write_imagef(dst, coords, clamp(res, 0.0f, 1.0f));
}



// tato verzia by bola vyhodna v pripade, ze pocet urovni (levels)
// je mensia ako obsah radiusu (cize ako (radius + radius + 1) * (radius + radius + 1))
__kernel void oilify2(__read_only image2d_t src,
                      __write_only image2d_t dst,
                      int radius,
                      int levels)
{
  uint4 hist[MAX_LEVELS];

  for (int i = 0; i < levels; ++i)
  {
    hist[i] = (uint4) (0, 0, 0, 0);
  }

  int2 coords = (int2) (get_global_id(0), get_global_id(1));

  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      uint4 color = read_imageui(src, sampler, coords + (int2) (i, j));
      hist[color.w].xyz += color.xyz;
      hist[color.w].w += 1;
    }
  }

  uint4 max = 0;

  for (int i = 0; i < levels; ++i)
  {
    if (hist[i].w > max.w)
    {
      max = hist[i];
    }
  }

  float4 res = convert_float4(max);
  res /= res.w;                // pouzit native_divide
  res.w = 1.0f;

  write_imagef(dst, coords, clamp(res, 0.0f, 1.0f));
}
#endif
