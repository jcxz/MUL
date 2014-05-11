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
