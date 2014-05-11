#include "FilterPipeline.h"
#include "GrayScaleFilter.h"
#include "SepiaFilter.h"
#include "ColorInvertFilter.h"
#include "TransformFilter.h"
#include "opencl/qclcontext.h"

#include <QImage>
#include <cstring>
#include <iostream>
#include <QDebug>



Filter* FilterPipeline::createFilter(const char *name) const
{
  Filter *filter = nullptr;

  if (strcmp(name, "grayscale") == 0)
  {
    filter = new GrayScaleFilter(m_ctx);
  }
  else if (strcmp(name, "sepia") == 0)
  {
    filter = new SepiaFilter(m_ctx);
  }
  else if (strcmp(name, "colorinvert") == 0)
  {
    filter = new ColorInvertFilter(m_ctx);
  }
  else if (strcmp(name, "transform") == 0)
  {
    filter = new TransformFilter(m_ctx);
  }
  else
  {
    std::cerr << "Filter named " << name << " does not exist" << std::endl;
    return nullptr;
  }

  if (!filter->isGood())
  {
    delete filter;
    std::cerr << "Failed to initialize filter " << name << std::endl;
    return nullptr;
  }

  return filter;
}


int FilterPipeline::addFilter(Filter *filter)
{
  int idx = m_filter_list.size();
  m_filter_list.push_back(tFilterPtr(filter));
  return idx;
}


QImage FilterPipeline::run(const cv::Mat & src)
{
  return QImage();
}


QImage FilterPipeline::run(const QImage & src)
{
  int w = src.width();
  int h = src.height();
  QImage res(w, h, QImage::Format_ARGB32);

  std::cerr << "QImage::Format_ARGB32 == " << QImage::Format_ARGB32 << std::endl;
  std::cerr << "QImage::Format_RGB32 == " << QImage::Format_RGB32 << std::endl;

  if ((src.format() != QImage::Format_ARGB32) && (src.format() != QImage::Format_RGB32))
  {
    std::cerr << "converting format from " << src.format() << std::endl;
    if (!runFilters(src.convertToFormat(QImage::Format_ARGB32).constBits(),
                    w, h, res.bits()))
    {
      return QImage();
    }
  }
  else
  {
    std::cerr << "using img without conversion: " << src.format() << std::endl;
    if (!runFilters(src.constBits(), w, h, res.bits()))
    {
      return QImage();
    }
  }

  return res;
}


bool FilterPipeline::loadGPUImageBuffers(const unsigned char *pixels, int w, int h,
                                         QCLImage2D *src, QCLImage2D *dst)
{
  if ((m_gpu_img_w != w) || (m_gpu_img_h != h))
  {
    QCLImageFormat fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);

    // allocate the input image
    *src = m_ctx->createImage2DCopy(fmt, pixels, QSize(w, h), QCLMemoryObject::ReadWrite);
    if (src->isNull())
    {
      std::cerr << "Failed to allocate image on GPU: " << m_ctx->lastError() << std::endl;
      return false;
    }

    // allocate the output image
    *dst = m_ctx->createImage2DDevice(fmt, QSize(w, h), QCLMemoryObject::ReadWrite);
    if (dst->isNull())
    {
      std::cerr << "Failed to allocate destination image on GPU: " << m_ctx->lastError() << std::endl;
      return false;
    }

    // rember the image size for the next time
    m_gpu_img_w = w;
    m_gpu_img_h = h;
  }
  else
  {
    if (!src->write(pixels, QRect(0, 0, w, h), w * 4 * sizeof(unsigned char)))
    {
      std::cerr << "Failed to write image data to GPU buffer: " << m_ctx->lastError() << std::endl;
      return false;
    }
  }

  return true;
}


bool FilterPipeline::loadCPUImageBuffers(const unsigned char *pixels, int w, int h,
                                         tPixelsPtr *src, tPixelsPtr *dst)
{
  if ((m_cpu_img_w != w) || (m_cpu_img_h != h))
  {
    // allocate the input image
    src->reset(new unsigned char[w * h * 4]);
    if (!src)
    {
      std::cerr << "Failed to allocate memory for image buffer on CPU" << std::endl;
      return false;
    }

    // allocate the output image
    dst->reset(new unsigned char[w * h * 4]);
    if (!dst)
    {
      std::cerr << "Failed to allocate memory for destination image on CPU" << std::endl;
      return false;
    }

    // remember the image size for the next time
    m_cpu_img_w = w;
    m_cpu_img_h = h;
  }

  std::memcpy(src->get(), pixels, w * h * 4 * sizeof(unsigned char));

  return true;
}


bool FilterPipeline::runFilters(const unsigned char *src, int w, int h, unsigned char *dst)
{
  bool using_gpu = false;

  bool cpu_buf_loaded = false;  // whether the CPU buffer is allocated
  bool gpu_buf_loaded = false;  // whether the GPU buffer is allocated

  int gpu_src = 0;  // which GPU buffer plays the source role
  int gpu_dst = 1;  // which GPU buffer plays the destination role

  int cpu_src = 0;  // which CPU buffer plays the source role
  int cpu_dst = 1;  // which CPU buffer plays the destination role

  for (unsigned int i = 0; i < m_filter_list.size(); ++i)
  {
    Filter *filter = m_filter_list[i].get();

    if (filter->hasOpenCL())
    {
      // reallocate the storage on the GPU
      if (!gpu_buf_loaded)
      {
        if (!loadGPUImageBuffers(src, w, h, m_gpu_img + gpu_src, m_gpu_img + gpu_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on GPU" << std::endl;
          return false;
        }
        gpu_buf_loaded = true;
      }

      // upload the data from cpu back to gpu
      if (!using_gpu)
      {
        if (m_cpu_img[cpu_src].get() != nullptr)
        {
          // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
          if (!m_gpu_img[gpu_src].write(m_cpu_img[cpu_src].get(), QRect(0, 0, w, h), w * 4 * sizeof(unsigned char)))
          {
            std::cerr << "WARNING: Failed to write data back from GPU: " << m_ctx->lastError() << std::endl;
          }
        }
      }

      // run the filter
      if (!filter->runCL(m_gpu_img[gpu_src], w, h, m_gpu_img[gpu_dst]))
      {
        std::cerr << "WARNING: Failed to run the OpenCL version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      gpu_dst = !gpu_dst;
      gpu_src = !gpu_src;

      using_gpu = true;
    }
    else
    {
      // reallocate the storage on the CPU
      if (!cpu_buf_loaded)
      {
        if (!loadCPUImageBuffers(src, w, h, m_cpu_img + cpu_src, m_cpu_img + cpu_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on CPU" << std::endl;
          return false;
        }
        cpu_buf_loaded = true;
      }

      // download the data from gpu back to cpu
      if (using_gpu)
      {
        //if (!m_gpu_img[gpu_src].isNull())
        //{
          // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
          if (!m_gpu_img[gpu_src].read(m_cpu_img[cpu_src].get(), QRect(0, 0, w, h), w * 4 * sizeof(unsigned char)))
          {
            std::cerr << "WARNING: Failed to read data back from GPU: " << m_ctx->lastError() << std::endl;
          }
        //}
      }

      // run the filter
      if (!filter->run(m_cpu_img[cpu_src].get(), w, h, m_cpu_img[cpu_dst].get()))
      {
        std::cerr << "WARNING: Failed to run the CPU version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      cpu_dst = !cpu_dst;
      cpu_src = !cpu_src;

      using_gpu = false;
    }
  }

  if (using_gpu)
  {
    if (m_gpu_img[gpu_src].isNull()) return true;

    // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
    if (!m_gpu_img[gpu_src].read(dst, QRect(0, 0, w, h), w * 4 * sizeof(unsigned char)))
    {
      std::cerr << "Failed to read data back from GPU: " << m_ctx->lastError() << std::endl;
      return false;
    }
  }
  else
  {
    unsigned char *p = m_cpu_img[cpu_src].get();
    if (p == nullptr) return true;
    // these copying could yet be optimized out
    std::memcpy(dst, p, w * h * 4 * sizeof(unsigned char));
  }

  return true;
}







#if 0
bool FilterPipeline::runFilters(const unsigned char *src, int w, int h, unsigned char *dst)
{
#if 0
  int gpu_img_src = 0;
  int gpu_img_dst = 1;
  bool gpu_img_allocated = false;

  for (unsigned int i = 0; i < m_filter_list.size(); ++i)
  {
    Filter *filter = m_filter_list[i].get();

    if (filter->hasOpenCL())
    {
      // reallocate the storage on the GPU
      if (!gpu_img_allocated)
      {
        if (!loadGPUImageBuffers(pixels, w, h, m_gpu_img + gpu_img_src, m_gpu_img + gpu_img_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on GPU" << std::endl;
          return QImage();
        }
        gpu_img_allocated = true;
      }

      // run the filter
      if (!filter->runCL(m_gpu_img[gpu_img_src], w, h, m_gpu_img[gpu_img_dst]))
      {
        std::cerr << "WARNING: Failed to run the OpenCL version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      gpu_img_dst = !gpu_img_dst;
      gpu_img_src = !gpu_img_src;
    }
  }

  return m_gpu_img[gpu_img_src].toQImage(false);
#elif 1
  bool using_gpu = false;

  bool cpu_buf_loaded = false;  // whether the CPU buffer is allocated
  bool gpu_buf_loaded = false;  // whether the GPU buffer is allocated

  int gpu_src = 0;  // which GPU buffer plays the source role
  int gpu_dst = 1;  // which GPU buffer plays the destination role

  int cpu_src = 0;  // which CPU buffer plays the source role
  int cpu_dst = 1;  // which CPU buffer plays the destination role

  for (unsigned int i = 0; i < m_filter_list.size(); ++i)
  {
    Filter *filter = m_filter_list[i].get();

    if (filter->hasOpenCL())
    {
      // reallocate the storage on the GPU
      if (!gpu_buf_loaded)
      {
        if (!loadGPUImageBuffers(src, w, h, m_gpu_img + gpu_src, m_gpu_img + gpu_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on GPU" << std::endl;
          return false;
        }
        gpu_buf_loaded = true;
      }

      // run the filter
      if (!filter->runCL(m_gpu_img[gpu_src], w, h, m_gpu_img[gpu_dst]))
      {
        std::cerr << "WARNING: Failed to run the OpenCL version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      gpu_dst = !gpu_dst;
      gpu_src = !gpu_src;

      using_gpu = true;
    }
    else
    {
      // reallocate the storage on the CPU
      if (!cpu_buf_loaded)
      {
        if (!loadCPUImageBuffers(src, w, h, m_cpu_img + cpu_src, m_cpu_img + cpu_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on CPU" << std::endl;
          return false;
        }
        cpu_buf_loaded = true;
      }

      // run the filter
      if (!filter->run(m_cpu_img[cpu_src].get(), w, h, m_cpu_img[cpu_dst].get()))
      {
        std::cerr << "WARNING: Failed to run the CPU version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      cpu_dst = !cpu_dst;
      cpu_src = !cpu_src;

      using_gpu = false;
    }
  }

  if (using_gpu)
  {
    // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
    if (!m_gpu_img[gpu_src].read(dst, QRect(0, 0, w, h), w * h * 4 * sizeof(unsigned char)))
    {
      std::cerr << "Failed to read data back from GPU: " << m_ctx->lastError() << std::endl;
      return false;
    }
  }
  else
  {
    // these copying could yet be optimized out
    std::memcpy(dst, m_cpu_img[cpu_src].get(), w * h * 4 * sizeof(unsigned char));
  }

  return true;
#elif 0
  QImage tmp;
  QImage result;

  int gpu_result = true;   // whether the result is a buffer on GPU or in a buffer on CPU

  int gpu_img_src = 0;
  int gpu_img_dst = 1;
  bool gpu_img_allocated = false;

  const unsigned char *cpu_img_src = pixels;
  const unsigned char *cpu_img_dst = nullptr;
  bool cpu_img_allocated = false;

  for (unsigned int i = 0; i < m_filter_list.size(); ++i)
  {
    Filter *filter = m_filter_list[i].get();

    if (filter->hasOpenCL())
    {
      // reallocate the storage on the GPU
      if (!gpu_img_allocated)
      {
        if (!loadGPUImageBuffers(pixels, w, h, m_gpu_img + gpu_img_src, m_gpu_img + gpu_img_dst))
        {
          std::cerr << "ERROR: Failed to allocate images on GPU" << std::endl;
          return QImage();
        }
        gpu_img_allocated = true;
      }

      // run the filter
      if (!filter->runCL(m_gpu_img[gpu_img_src], w, h, m_gpu_img[gpu_img_dst]))
      {
        std::cerr << "WARNING: Failed to run the OpenCL version of the filter" << std::endl;
      }

      // swap the source and the destination buffers
      gpu_img_dst = !gpu_img_dst;
      gpu_img_src = !gpu_img_src;

      gpu_result = true;
    }
    else
    {
      // reallocate the storage on the CPU
      if (!cpu_img_allocated)
      {
        result = QImage(w, h, QImage::Format_ARGB32);
        tmp = QImage(w, h, QImage::Format_ARGB32);
        if ((result.isNull()) || (tmp.isNull()))
        {
          std::cerr << "ERROR: Failed to allocate buffers on CPU" << std::endl;
          return QImage();
        }
        cpu_img_dst = result;
        cpu_img_allocated = true;
      }

      gpu_result = false;
    }
  }

  if (gpu_result)
  {
    if (!cpu_img_allocated)
    {
      result = QImage(w, h, QImage::Format_ARGB32);
    }

    m_gpu_img[gpu_img_dst].read(&result, QRect(0, 0, w, h));
  }

  return result;
#endif
}
#endif
