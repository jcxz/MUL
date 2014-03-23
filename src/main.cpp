#include "FilterGray.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstring>


#define PSTR(s) (s ? s : "NULL")



static void usage(const char *app_name)
{
  std::cout << app_name << " input_video_file output_video_file filter_type" << std::endl;
  std::cout << "filter_type      ... Gray (grayscale filter)" << std::endl;
}


static Filter *createFilterFromString(const char *filtername)
{
  Filter *f = nullptr;

  if (strcmp(filtername, "Gray") == 0)
  {
    return new FilterGray();
  }

  return f;
}


/*
 * Ooopsie filtrujeme obraz, ale zvuk nam akosi zmizol - treba vyriesit ...
 */
static bool convertVideo(const char *ifilename, const char *ofilename, Filter *filter)
{
  assert(filter != nullptr);

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
      std::cerr << "WARNING: An error occured while reading frame n. " << frame_i << std::endl;
      continue;
    }

    filter->exec(frame);

    out.write(frame);
  }

  return true;
}



int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    std::cerr << "ERROR: Not enough command line arguments" << std::endl;
    usage(argv[0]);
    return 1;
  }

  Filter *filter = createFilterFromString(argv[3]);
  if (filter == nullptr)
  {
    std::cerr << "ERROR: Wrong filter name" << std::endl;
    return 1;
  }

  int ret = convertVideo(argv[1], argv[2], filter) ? 0 : 1;

  delete filter;

  std::cout << "Video successfully filtered" << std::endl;

  return ret;
}
