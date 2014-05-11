__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_NEAREST;
 
                               
__kernel void sobel_x(__read_only image2d_t src, __write_only image2d_t dst)
{
  // TODO: maybe without the sampler it will be a bit faster
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 c1 = read_imagef(src, sampler, coords - (int2) (1, 0));
  float4 c2 = read_imagef(src, sampler, coords + (int2) (1, 0));
  write_imagef(dst, coords, fabs(c1 - c2));
}


__kernel void sobel_y(__read_only image2d_t src, __write_only image2d_t dst)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 c1 = read_imagef(src, sampler, coords - (int2) (0, 1));
  float4 c2 = read_imagef(src, sampler, coords + (int2) (0, 1));
  write_imagef(dst, coords, fabs(c1 - c2));
}


__kernel void sobel_combine(__read_only image2d_t src_x,
                            __read_only image2d_t src_y,
                            __write_only image2d_t dst)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 c1 = read_imagef(src_x, coords);
  float4 c2 = read_imagef(src_y, coords);
  float4 res = clamp(c1 + c2, 0.0f, 1.0f);
  res.w = 1.0f;  
  write_imagef(dst, coords, res);
}