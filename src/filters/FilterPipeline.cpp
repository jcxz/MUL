#include "FilterPipeline.h"
#include "GrayScaleFilter.h"
#include "SepiaFilter.h"
#include "ColorInvertFilter.h"
#include "TransformFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"

#include <QImage>
#include <cstring>



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
    ERRORM("Filter named " << name << " does not exist");
    return nullptr;
  }

  if (!filter->isGood())
  {
    delete filter;
    ERRORM("Failed to initialize filter " << name);
    return nullptr;
  }

  return filter;
}


bool FilterPipeline::loadGPUImageBuffers(const QImage & img, QCLImage2D *src, QCLImage2D *dst)
{
  QSize img_size(img.size());

  DBGM("loadGPUImageBuffers");
  DBGM("Saved size (width x heigth): " << m_gpu_buf_size.width() << "x" << m_gpu_buf_size.height());
  DBGM("Frame size (width x heigth): " << img_size.width() << "x" << img_size.height());
  DBGM("QImage::Format_ARGB32 : " << QImage::Format_ARGB32);
  DBGM("QImage::Format_RGB32  : " << QImage::Format_RGB32);
  DBGM("img.format()          : " << img.format());

  if (img_size != m_gpu_buf_size)
  {
    INFOM("Reallocating GPU buffers");

    // allocate the input image
    if ((img.format() != QImage::Format_ARGB32) && (img.format() != QImage::Format_RGB32))
    {
      INFOM("Converting GPU source buffer format");
      *src = m_ctx->createImage2DCopy(img.convertToFormat(QImage::Format_ARGB32), QCLMemoryObject::ReadOnly);
    }
    else
    {
      *src = m_ctx->createImage2DCopy(img, QCLMemoryObject::ReadOnly);
    }

    if (src->isNull())
    {
      ERRORM("Failed to allocate source image on GPU: " << m_ctx->lastError());
      return false;
    }

    // allocate the output image
    QCLImageFormat out_fmt(QCLImageFormat::Order_BGRA, QCLImageFormat::Type_Normalized_UInt8);
    *dst = m_ctx->createImage2DDevice(out_fmt, img_size, QCLMemoryObject::WriteOnly);
    if (dst->isNull())
    {
      ERRORM("Failed to allocate destination image on GPU: " << m_ctx->lastError());
      return false;
    }

    // remember the image parameters for the next time
    m_gpu_buf_size = img_size;
  }
  else
  {
    INFOM("Reusing GPU source buffer");

    bool ret = false;

    if ((img.format() != QImage::Format_ARGB32) && (img.format() != QImage::Format_RGB32))
    {
      INFOM("Converting input image pixel format to GPU pixel format");
      ret = src->write(img.convertToFormat(QImage::Format_ARGB32));
    }
    else
    {
      ret = src->write(img);
    }

    if (!ret)
    {
      ERRORM("Failed to write image data to source GPU buffer: " << m_ctx->lastError());
      return false;
    }
  }

  return true;
}


bool FilterPipeline::loadCPUImageBuffers(const QImage & img, QImage *src, QImage *dst)
{
  QSize img_size(img.size());

  DBGM("loadCPUImageBuffers");
  DBGM("Saved size (width x heigth): " << m_cpu_buf_size.width() << "x" << m_cpu_buf_size.height());
  DBGM("Frame size (width x heigth): " << img_size.width() << "x" << img_size.height());
  DBGM("QImage::Format_ARGB32 : " << QImage::Format_ARGB32);
  DBGM("QImage::Format_RGB32  : " << QImage::Format_RGB32);
  DBGM("img.format()          : " << img.format());

  // allocate the input image
  if ((img.format() != QImage::Format_ARGB32) && (img.format() != QImage::Format_RGB32))
  {
    INFOM("Converting CPU image format to ARGB32");
    *src = img.convertToFormat(QImage::Format_ARGB32);
    if (src->isNull())
    {
      ERRORM("Failed to convert source CPU image buffer");
      return false;
    }
  }
  else
  {
    INFOM("Using the CPU image format as is");
    *src = img;
  }

  // allocate the output image
  if (img_size != m_cpu_buf_size)
  {
    INFOM("Reallocating output CPU buffer");

    *dst = QImage(img_size, QImage::Format_ARGB32);

    // remember the image size for the next time
    m_cpu_buf_size = img_size;
  }

  return true;
}


QImage FilterPipeline::run(const QImage & src)
{
  enum DeviceInUse {
    NONE,
    GPU,
    CPU
  } device = NONE;

  int w = src.width();
  int h = src.height();

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
        if (!loadGPUImageBuffers(src, m_gpu_buf + gpu_src, m_gpu_buf + gpu_dst))
        {
          ERRORM("Failed to allocate images on GPU");
          return QImage();
        }
        gpu_buf_loaded = true;
      }

      // upload the data from cpu back to gpu
      if (device == CPU)
      {
        // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
        if (!m_gpu_buf[gpu_src].write(m_cpu_buf[cpu_src]))
        {
          WARNM("Failed to write data back from GPU: " << m_ctx->lastError());
        }
      }

      // run the filter
      if (!filter->runCL(m_gpu_buf[gpu_src], w, h, m_gpu_buf[gpu_dst]))
      {
        WARNM("Failed to run the OpenCL version of the filter");
      }

      // swap the source and the destination buffers
      gpu_dst = !gpu_dst;
      gpu_src = !gpu_src;

      device = GPU;
    }
    else
    {
      // reallocate the storage on the CPU
      if (!cpu_buf_loaded)
      {
        if (!loadCPUImageBuffers(src, m_cpu_buf + cpu_src, m_cpu_buf + cpu_dst))
        {
          ERRORM("Failed to allocate images on CPU");
          return QImage();
        }
        cpu_buf_loaded = true;
      }

      // download the data from gpu back to cpu
      if (device == GPU)
      {
        // use gpu_src because gpu_dst has been swapped to gpu_src at the end of the last iteration
        if (!m_gpu_buf[gpu_src].read(m_cpu_buf + cpu_src))
        {
          WARNM("Failed to read data back from GPU: " << m_ctx->lastError());
        }
      }

      // run the filter
      if (!filter->run(m_cpu_buf[cpu_src].constBits(), w, h, m_cpu_buf[cpu_dst].bits()))
      {
        WARNM("Failed to run the CPU version of the filter");
      }

      // swap the source and the destination buffers
      cpu_dst = !cpu_dst;
      cpu_src = !cpu_src;

      device = CPU;
    }
  }

  if (device == GPU)
  {
    return m_gpu_buf[gpu_src].toQImage(false);
  }
  else if (device == CPU)
  {
    return m_cpu_buf[cpu_src];
  }
  else
  {
    std::cerr << "WARNING: There are no filters in the pipeline" << std::endl;
  }

  return QImage();
}
