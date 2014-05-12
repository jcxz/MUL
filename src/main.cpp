#ifdef UTEST

#include "FilterPipeline.h"
#include "TransformFilter.h"
#include "GrayScaleFilter.h"
#include "SeparableConv2DFilter.h"
#include "GaussianBlurFilter.h"
#include "Conv2DFilter.h"
#include "opencl/qclcontext.h"

#include <iostream>
#include <QImage>


//#define IN_FRAME1 "data/lena.png"
//#define IN_FRAME1 "D:/AC601/obrazky/testovacie/intel_80486dx2_bottom_thumb.jpg"
#define IN_FRAME1 "D:/AC601/obrazky/PasoveFoto/orezane.jpg"
#define OUT_FRAME1 "bordel/frame1.png"

//#define IN_FRAME1 "D:/AC601/obrazky/testovacie/dog.jpg"
//#define OUT_FRAME1 "bordel/dog.jpg"

#define IN_FRAME2 "data/kvecina.jpg"
#define OUT_FRAME2 "bordel/frame2.jpg"



static bool convFrame(const char *srcfile, const char *dstfile, FilterPipeline & pipeline)
{
  std::cerr << __FUNCTION__ << std::endl;
  std::cerr << "srcfile: " << srcfile << std::endl;
  std::cerr << "dstfile: " << dstfile << std::endl;

  QImage src(srcfile);
  if (src.isNull())
  {
    std::cerr << "Failed to read image from file " << srcfile << std::endl;
    return false;
  }

  //if (!src.save("bordel/orig.png"))
  //{
  //  std::cerr << "Failed to save image to file " << "bordel/orig.png" << std::endl;
  //  return false;
  //}

  QImage dst(pipeline.run(src));

  if (dst.isNull())
  {
    std::cerr << "Failed to generate destination image" << std::endl;
    return false;
  }

  if (!dst.save(dstfile))
  {
    std::cerr << "Failed to save image to file " << dstfile << std::endl;
    return false;
  }

  return true;
}


static bool testFilterPipeline2Frames(QCLContext *ctx)
{
  /* setup filter pipeline */
  FilterPipeline pipeline(ctx);

  //pipeline.addFilter("grayscale");
  //pipeline.addFilter2("grayscale")->setCLContext(nullptr);

#if 0
  Conv2DFilter *conv2d_f = static_cast<Conv2DFilter *>(pipeline.addFilter2("conv2d"));
  if (conv2d_f == nullptr) return false;
  float kern_2d[] = { 1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f,
                      1.0f / 8.0f,  1.0f / 4.0f, 1.0f / 8.0f,
                      1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f };
  //float kern_2d[] = { -1.0f, 0.0f, 1.0f,
  //                    -1.0f, 0.0f, 1.0f,
  //                    -1.0f, 0.0f, 1.0f };
  //static float kern_2d[] = {  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
  //                            1.0f,  1.0f,  1.0f,  0.0f, -1.0f,
  //                            1.0f,  1.0f,  0.0f, -1.0f, -1.0f,
  //                            1.0f,  0.0f, -1.0f, -1.0f, -1.0f,
  //                            0.0f, -1.0f, -1.0f, -1.0f, -1.0f };
  conv2d_f->setFilterKernel(kern_2d, 3);
#endif

#if 0
  SeparableConv2DFilter *conv2d_sep_f = static_cast<SeparableConv2DFilter *>(pipeline.addFilter2("separableconv2d"));
  if (conv2d_sep_f == nullptr) return false;
  //float kern_2d_sep[] = { -1.0f, 0.0f, 1.0f };

  float kern_2d_sep[] = { 1.0f / 4.0f,  1.0f / 2.0f, 1.0f / 4.0f };
  //conv2d_sep_f->setVerticalFilterKernel(kern_2d_sep, 3);
  //conv2d_sep_f->setHorizontalFilterKernel(kern_2d_sep, 3);

  float *ptr = conv2d_sep_f->mapVerticalFilterKernel(3);
  ptr[0] = 1.0f / 4.0f; ptr[1] = 1.0f / 2.0f; ptr[2] = 1.0f / 4.0f;
  conv2d_sep_f->unmapVerticalFilterKernel(ptr);
  conv2d_sep_f->setHorizontalFilterKernel(kern_2d_sep, 3);

  //float kern_2d_sep[] = { 1.0f / 8.0f, 1.0f / 2.0f,  1.0f / 4.0f, 1.0f / 2.0f, 1.0f / 8.0f };
  //conv2d_sep_f->setVerticalFilterKernel(kern_2d_sep, 5);
  //conv2d_sep_f->setHorizontalFilterKernel(kern_2d_sep, 5);
#endif

  //pipeline.addFilter("emboss");
  //pipeline.addFilter("dog");

  GaussianBlurFilter *gf = static_cast<GaussianBlurFilter *>(pipeline.addFilter2("gaussianblur"));
  //gf->setSize(15);
  gf->setSigma(16.0f);
  gf->setVariance(16.0f);

  //pipeline.addFilter("sobel");

  //TransformFilter *filter = static_cast<TransformFilter *>(pipeline.addFilter2("transform"));
  //filter->setMatrix(1.0f, 0.0f, -100.0f, 0.0f, 1.0f, -100.0f);

  /* run the pipeline */
  bool ret = convFrame(IN_FRAME1, OUT_FRAME1, pipeline);

  ret = ret && convFrame(IN_FRAME2, OUT_FRAME2, pipeline);

  return ret;
}


int main(void)
{
  // create context
  QCLContext ctx;
  if (!ctx.create(QCLDevice::GPU))
  {
    std::cerr << "Failed to create OpenCL context" << std::endl;
    return 1;
  }

  testFilterPipeline2Frames(&ctx);

  return 0;
}


#else


#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // create context
    QCLContext ctx;
    if (!ctx.create(QCLDevice::GPU))
    {
      std::cerr << "Failed to create OpenCL context" << std::endl;
      return 1;
    }

    w.setCLContext(&ctx);

    return a.exec();
}


#endif
