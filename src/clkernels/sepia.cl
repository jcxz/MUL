__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                               CLK_ADDRESS_NONE |
                               CLK_FILTER_NEAREST;

__kernel void sepia(__read_only image2d_t src, __write_only image2d_t dst)
{
  int2 coords = (int2) (get_global_id(0), get_global_id(1));
  float4 src_col = read_imagef(src, sampler, coords);
  
  float4 dst_col = (float4) ((0.393f * src_col.x) + (0.769f * src_col.y) + (0.189f * src_col.z), // red
                             (0.349f * src_col.x) + (0.686f * src_col.y) + (0.168f * src_col.z), // green
                             (0.272f * src_col.x) + (0.534f * src_col.y) + (0.131f * src_col.z), // blue
                             src_col.w);
  
  dst_col = min(dst_col, 1.0f);
  
  write_imagef(dst, coords, dst_col);
}
