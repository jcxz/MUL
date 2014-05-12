/* TODO: if these kernels were slow, they could be sped up by exploiting
         the HW texture interpolation trick
 */

//#pragma OPENCL EXTENSION cl_amd_printf : enable

#define FILTER(row, col) filter[row * filter_size + col]

//__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
//                               CLK_ADDRESS_CLAMP |
//                               //CLK_ADDRESS_CLAMP_TO_EDGE |
//                               CLK_FILTER_NEAREST;


__kernel void conv2D(__read_only image2d_t src,
                     __write_only image2d_t dst,
                     sampler_t sampler,
                     __constant float *filter,
                     const int filter_size,
                     const int filter_size_half,
                     float4 bias)
{  
  int2 coords = (int2) (get_global_id(0) + filter_size_half,
                        get_global_id(1) + filter_size_half);
  float4 sum = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  
  for (int i = 0; i < filter_size; ++i)
  {
    for (int j = 0; j < filter_size; ++j)
    {
      float4 color = read_imagef(src, sampler, coords - (int2) (i, j));
      sum += color * FILTER(i, j);
      //sum = fabs(sum + color * FILTER(i, j));  
    }
  }
  
  sum = clamp(sum + bias, 0.0f, 1.0f);
  //sum = min(fabs(sum + bias), 1.0f);
  //sum = min(sum + bias, 1.0f);
  sum.w = 1.0f;
  
  write_imagef(dst, coords, sum);
  
  //write_imagef(dst, coords, (float4) (sum.xyz, 1.0f));
}



__kernel void conv2D_horizontal(__read_only image2d_t src,
                                __write_only image2d_t dst,
                                //__constant sampler_t sampler,
                                sampler_t sampler,
                                __constant float *filter,
                                const int filter_size,
                                const int filter_size_half,
                                float4 bias)
{
  int2 coords = (int2) (get_global_id(0) + filter_size_half, get_global_id(1));
  float4 sum = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  
  for (int i = 0; i < filter_size; ++i)
  {
    float4 color = read_imagef(src, sampler, coords - (int2) (i, 0));
    sum += color * filter[i];   
  }
  
  sum = clamp(sum + bias, 0.0f, 1.0f);
  sum.w = 1.0f;
  
  write_imagef(dst, coords, sum);
  
  //write_imagef(dst, coords, (float4) (sum.xyz, 1.0f));
}




__kernel void conv2D_vertical(__read_only image2d_t src,
                              __write_only image2d_t dst,
                              //__constant sampler_t sampler,
                              sampler_t sampler,
                              __constant float *filter,
                              const int filter_size,
                              const int filter_size_half,
                              float4 bias)
{
  // TODO: try to optimize this, maybe by transposing the input image
  int2 coords = (int2) (get_global_id(0), get_global_id(1) + filter_size_half);
  float4 sum = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  
  for (int i = 0; i < filter_size; ++i)
  {
    float4 color = read_imagef(src, sampler, coords - (int2) (0, i));
    sum += color * filter[i];   
  }
  
  sum = clamp(sum + bias, 0.0f, 1.0f);
  sum.w = 1.0f;
  
  write_imagef(dst, coords, sum);
  
  //write_imagef(dst, coords, (float4) (sum.xyz, 1.0f));
}