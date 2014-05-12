//#define UTEST

#ifdef UTEST

#include "FilterPipeline.h"
#include "TransformFilter.h"
#include "GrayScaleFilter.h"
#include "SeparableConv2DFilter.h"
#include "GaussianBlurFilter.h"
#include "Conv2DFilter.h"
#include "opencl/qclcontext.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <QImage>

#define PSTR(s) (s ? s : "NULL")

//#define IN_FRAME1 "data/lena.png"
//#define IN_FRAME1 "D:/AC601/obrazky/testovacie/intel_80486dx2_bottom_thumb.jpg"
#define IN_FRAME1 "D:/AC601/obrazky/PasoveFoto/orezane.jpg"
#define OUT_FRAME1 "bordel/frame1.png"

//#define IN_FRAME1 "D:/AC601/obrazky/testovacie/dog.jpg"
//#define OUT_FRAME1 "bordel/dog.jpg"

#define IN_FRAME2 "data/kvecina.jpg"
#define OUT_FRAME2 "bordel/frame2.jpg"

#define IN_VID1 "data/Megamind.avi"
#define OUT_VID1 "bordel/Megamind.avi"




static bool convertVideo(const char *ifilename, const char *ofilename, FilterPipeline *pipeline)
{
  assert(pipeline != nullptr);

  // open the input video file
  cv::VideoCapture in(ifilename);
  if (!in.isOpened())
  {
    std::cerr << "Failed to open input video file: " << PSTR(ifilename) << std::endl;
    return false;
  }

  // query for the video properties
  unsigned int frame_max = in.get(CV_CAP_PROP_FRAME_COUNT);
  int fourcc = in.get(CV_CAP_PROP_FOURCC);
  double fps = in.get(CV_CAP_PROP_FPS);
  cv::Size size(in.get(CV_CAP_PROP_FRAME_WIDTH), in.get(CV_CAP_PROP_FRAME_HEIGHT));

  std::cout << "Input video properties:" << std::endl;
  std::cout << "fourcc : " << fourcc << std::endl;
  std::cout << "fps    : " << fps << std::endl;
  std::cout << "size   : " << size << std::endl;
  std::cout << "format : " << in.get(CV_CAP_PROP_FORMAT) << std::endl;

  // open the output video file
  cv::VideoWriter out(ofilename, fourcc, fps, size);
  if (!out.isOpened())
  {
    std::cerr << "Failed to open output video file: " << PSTR(ofilename) << std::endl;
    return false;
  }

  // process the individual video frames
  cv::Mat frame;

  for (unsigned int frame_i = 0; frame_i < frame_max; ++frame_i)
  {
    if (!in.read(frame))
    {
      std::cerr << "WARNING: An error occured while reading frame # " << frame_i << std::endl;
      continue;
    }

    QImage res(pipeline->run(frame));

#if 0
    if ((frame_i % 10) == 0)
    {
      cv::imwrite("output/orig_frame_" + std::to_string(frame_i) + ".png", frame);
      res.save(QString("output/frame_%1.png").arg(frame_i));
    }
#endif
    res = res.convertToFormat(QImage::Format_RGB888);

    cv::Mat converted_frame(res.height(), res.width(), CV_8UC3, res.bits(), res.bytesPerLine());

    out.write(converted_frame);
  }

  return true;
}


static bool testVideoConversion(QCLContext *ctx)
{
  FilterPipeline pipeline(ctx);

  pipeline.addFilter("shake");

  return convertVideo(IN_VID1, OUT_VID1, &pipeline);
}


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
  pipeline.addFilter("dog");

#if 0
  GaussianBlurFilter *gf = static_cast<GaussianBlurFilter *>(pipeline.addFilter2("gaussianblur"));
  //gf->setSize(15);
  gf->setSigma(16.0f);
  gf->setVariance(16.0f);
#endif

  //pipeline.addFilter("sobel");
  //pipeline.addFilter2("oilify");

#if 0
  TransformFilter *filter = static_cast<TransformFilter *>(pipeline.addFilter2("transform"));
  filter->setMatrix(1.0f, 0.0f, -100.0f, 0.0f, 1.0f, -100.0f);
#endif

  //pipeline.addFilter2("oilify");

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
  //testVideoConversion(&ctx);

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
