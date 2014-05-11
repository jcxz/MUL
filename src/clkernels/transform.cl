#pragma OPENCL EXTENSION cl_amd_printf : enable


__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP |
                               CLK_FILTER_LINEAR;

#define M(y, x) matrix[(y) * 3 + (x)]

__kernel void transform(__read_only image2d_t src,
                        __write_only image2d_t dst,
                        __constant float *matrix)
{
#if 1
  // calculate destination image coordinates
  int2 dst_coords = (int2) (get_global_id(0), get_global_id(1));
  
  //printf("transform:\nM(0, 0)=%f, M(0, 1)=%f, M(0, 2)=%f\nM(1, 0)=%f, M(1, 1)=%f, M(1, 2)=%f\n",
  //       M(0, 0), M(0, 1), M(0, 2), M(1, 0), M(1, 1), M(1, 2));
  
  // calculate corresponding source image coordinates
  float2 src_coords = (float2) (M(0, 0) * dst_coords.x + M(0, 1) * dst_coords.y + M(0, 2),  //+ 100.0f,// + M(0, 2),
                                M(1, 0) * dst_coords.x + M(1, 1) * dst_coords.y + M(1, 2)); //+ 100.0f); //M(1, 2));
                                
  // copy the pixel value
  write_imagef(dst, dst_coords, read_imagef(src, sampler, src_coords));
#else
  int2 dst_coords = (int2) (get_global_id(0), get_global_id(1));
  float2 src_coords = (float2) (get_global_id(0), get_global_id(1));
  write_imagef(dst, dst_coords, read_imagef(src, sampler, src_coords));
#endif
}