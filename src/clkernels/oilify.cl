
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
// 3.) Nakoniec pixelu priradi priemer z pixel v najcastejsie sa vyskytujucej urovni jasu
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