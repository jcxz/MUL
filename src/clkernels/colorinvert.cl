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