
// NOTE for both kernels:
//   the last element of factor must be 0 and the last element of bias
//   must be 1 in order get the correct value of the alpha channel 

__kernel void dog_subtract(__read_only image2d_t src1,
                           __read_only image2d_t src2,
                           __write_only image2d_t dst,
                           float4 factor,
                           float4 bias)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 c1 = read_imagef(src1, coords);
  float4 c2 = read_imagef(src2, coords);
  float4 c =  (c1 - c2) * factor + bias; 
  write_imagef(dst, coords, clamp(c, 0.0f, 1.0f));
}


__kernel void dog_subtract_inv(__read_only image2d_t src1,
                               __read_only image2d_t src2,
                               __write_only image2d_t dst,
                               float4 factor,
                               float4 bias)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 c1 = read_imagef(src1, coords);
  float4 c2 = read_imagef(src2, coords);
  float4 c =  1.0f - clamp((c2 - c1) * factor + bias, 0.0f, 1.0f);
  write_imagef(dst, coords, (float4) (c.xyz, 1.0f));
  //float4 c =  (c2 - c1) * factor + bias; 
  //write_imagef(dst, coords, 1.0f - clamp(c, 0.0f, 1.0f));
}