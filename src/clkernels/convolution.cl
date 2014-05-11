
#define FILTER(row, col) filter[row * filter_size + col]

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP |
                               //CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_NEAREST;


__kernel void conv2D(__read_only image2d_t src,
                     __write_only image2d_t dst,
                     __constant float *filter,
                     const int filter_size,
                     const int filter_size_half)
{
  int2 coords = (int2) (get_global_id(0) + filter_size_half,
                        get_global_id(1) + filter_size_half);
  float4 sum = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  
  for (int i = 0; i < filter_size; ++i)
  {
    for (int j = 0; j < filter_size; ++j)
    {
      float4 color = read_imagef(src, sampler, coords - (int2) (i, j));
      //float f = FILTER(i, j);
      sum += color * FILTER(i, j);
      //sum += color * ((float4) (f));    
    }
  }
  
  write_imagef(dst, coords, (float4) (sum.xyz, 1.0f));
}