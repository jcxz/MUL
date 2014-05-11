#ifndef FILTERPIPELINE_H
#define FILTERPIPELINE_H

#include "Filter.h"
#include "opencl/qclimage.h"

#include <vector>
#include <memory>


namespace cv { class Mat; }
class QImage;


// Class representing a graph of filters
class FilterPipeline
{
  public:
    typedef std::unique_ptr<Filter> tFilterPtr;

  public:
    static constexpr int INVALID_FILTER_INDEX = -1;

  public:
    FilterPipeline(QCLContext *ctx)
      : m_ctx(ctx)
      , m_filter_list()
      , m_gpu_img_w(0)
      , m_gpu_img_h(0)
      , m_cpu_img_w(0)
      , m_cpu_img_h(0)
    {
    }

    //EDITED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    bool isEmpty() {
        return (m_filter_list.size() == 0);
    }

    // filter graph management
    int filterIndex(Filter *filter)
    {
      for (unsigned i = 0; i < m_filter_list.size(); ++i)
      {
        if (m_filter_list[i].get() == filter) return i;
      }
      return INVALID_FILTER_INDEX;
    }

    Filter *filterByIndex(int idx)
    {
      return (unsigned(idx) < m_filter_list.size()) ? m_filter_list[idx].get() : nullptr;
    }

    int addFilter(const char *name) { return addFilter(createFilter(name)); }

    Filter *addFilter2(const char *name)
    {
      Filter *filter = createFilter(name);
      addFilter(filter);
      return filter;
    }

    int addFilter(Filter *filter);
    void insertFilter(int idx, Filter *filter);

    void removeFilter(Filter *filter);
    void removeFilter(int idx);

    void moveFilter(Filter *filter, int new_pos);
    void moveFilter(int cur_pos, int new_pos);

    // input image management
    void setImage(const cv::Mat & img);
    void setImage(const QImage & img);

    // pipeline execution
    QImage run(const cv::Mat & src);
    QImage run(const QImage & src);

  private:
    // a vector of filters
    typedef std::vector<tFilterPtr> tFilterList;
    typedef std::unique_ptr<unsigned char> tPixelsPtr;

  private:
    Filter* createFilter(const char *name) const;
    bool loadGPUImageBuffers(const unsigned char *pixels, int w, int h,
                             QCLImage2D *src, QCLImage2D *dst);
    bool loadCPUImageBuffers(const unsigned char *pixels, int w, int h,
                             tPixelsPtr *src, tPixelsPtr *dst);
    //QImage runFilters(const unsigned char *pixels, int w, int h);
    bool runFilters(const unsigned char *src, int w, int h, unsigned char *dst);

  private:
    QCLContext *m_ctx;          // OpenCL context
    tFilterList m_filter_list;  // a list of pipelined filters

    QCLImage2D m_gpu_img[2];  // the gpu image
    tPixelsPtr m_cpu_img[2];  // the cpu image
    int m_gpu_img_w;
    int m_gpu_img_h;
    int m_cpu_img_w;          // the size of the frame being processed
    int m_cpu_img_h;
};

#endif
