#ifdef UTEST

#include "FilterPipeline.h"
#include "TransformFilter.h"
#include "GrayScaleFilter.h"
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
  std::cerr << __FUNCTION__ << std::endl;

  // setup filter pipeline
  FilterPipeline pipeline(ctx);

  pipeline.addFilter("grayscale");

  //Filter *f = pipeline.addFilter2("grayscale");
  //f->setCLContext(nullptr);

  TransformFilter *filter = static_cast<TransformFilter *>(pipeline.addFilter2("transform"));
  filter->setMatrix(1.0f, 0.0f, -100.0f, 0.0f, 1.0f, -100.0f);

  pipeline.moveFilter(0, 1);

  //pipeline.addFilter("grayscale");

  //Filter *f = pipeline.addFilter2("grayscale");
  //f->setCLContext(nullptr);

  //filter = static_cast<TransformFilter *>(pipeline.addFilter2("transform"));
  //filter->setMatrix(0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f);

  // run the pipeline
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
