#ifdef UTEST

#include "FilterPipeline.h"
#include "TransformFilter.h"
#include "GrayScaleFilter.h"
#include "SeparableConv2DFilter.h"
#include "Conv2DFilter.h"
#include "opencl/qclcontext.h"

#include <iostream>
#include <QImage>


#define IN_FRAME1 "data/lena.png"
#define OUT_FRAME1 "bordel/frame1.png"

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

#if 1
  Conv2DFilter *conv2d_f = static_cast<Conv2DFilter *>(pipeline.addFilter2("conv2d"));
  if (conv2d_f == nullptr) return false;
  float kern_2d[] = { 1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f,
                      1.0f / 8.0f,  1.0f / 4.0f, 1.0f / 8.0f,
                      1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f };
  conv2d_f->setFilterKernel(kern_2d, 3);
#endif

  //SeparableConv2DFilter *conv_f = static_cast<SeparableConv2DFilter *>(pipeline.addFilter2(""));

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
