/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILTERPIPELINE_H
#define FILTERPIPELINE_H

#include "Filter.h"
#include "opencl/qclimage.h"

#include <opencv2/core/core.hpp>
#include <QImage>
#include <vector>
#include <memory>



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
      , m_gpu_buf_size()
      , m_cpu_buf_size()
    {
    }

    /*** filter graph management ***/

    // query methods
    inline bool isEmpty(void) const { return m_filter_list.empty(); }
    inline int filterIndex(Filter *filter) const;

    inline bool isValidFilterIndex(int idx) const
    { return (unsigned(idx) < m_filter_list.size()); }

    inline Filter *filterByIndex(int idx) const
    { return (isValidFilterIndex(idx)) ? m_filter_list[idx].get() : nullptr; }

    // insertion and creation methods
    int addFilter(const char *name) { return addFilter(createFilter(name)); }
    inline Filter *addFilter2(const char *name);
    inline int addFilter(Filter *filter);
    inline Filter *insertFilter(int idx, const char *name);
    inline void insertFilter(int idx, Filter *filter);

    // removal methods
    void clear(void) { m_filter_list.clear(); }
    void removeFilter(Filter *filter) { removeFilter(filterIndex(filter)); }
    inline void removeFilter(int idx);

    // shuffle methods
    void moveFilter(Filter *filter, int new_pos)
    { return moveFilter(filterIndex(filter), new_pos); }

    inline void moveFilter(int cur_pos, int new_pos);

    /*** Pipeline execution methods ***/

    // input image management
    //void setImage(const cv::Mat & img);
    //void setImage(const QImage & img);

    // pipeline execution
    QImage run(const cv::Mat & src)
    {
      // this should only use the image data provided and not copy them
      return run(QImage(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888));
    }

    QImage run(const QImage & src);

  private:
    // a vector of filters
    typedef std::vector<tFilterPtr> tFilterList;
    typedef std::unique_ptr<unsigned char> tPixelsPtr;

  private:
    Filter* createFilter(const char *name) const;

    bool loadGPUImageBuffers(const QImage & img, QCLImage2D *src, QCLImage2D *dst);
    bool loadCPUImageBuffers(const QImage & img, QImage *src, QImage *dst);

  private:
    QCLContext *m_ctx;          // OpenCL context
    tFilterList m_filter_list;  // a list of pipelined filters

    QCLImage2D m_gpu_buf[2];    // the gpu image buffer
    QImage m_cpu_buf[2];        // the cpu image buffer
    QSize m_gpu_buf_size;       // the buffer size
    QSize m_cpu_buf_size;       // the buffer size
};


/*** Implementation of the inline methods ***/

inline int FilterPipeline::filterIndex(Filter *filter) const
{
  for (unsigned i = 0; i < m_filter_list.size(); ++i)
  {
    if (m_filter_list[i].get() == filter) return i;
  }
  return INVALID_FILTER_INDEX;
}


inline Filter *FilterPipeline::addFilter2(const char *name)
{
  Filter *filter = createFilter(name);
  addFilter(filter);
  return filter;
}


inline int FilterPipeline::addFilter(Filter *filter)
{
  if (filter == nullptr) return INVALID_FILTER_INDEX;
  int idx = m_filter_list.size();
  m_filter_list.push_back(tFilterPtr(filter));
  return idx;
}


inline Filter *FilterPipeline::insertFilter(int idx, const char *name)
{
  Filter *f = createFilter(name);
  insertFilter(idx, f);
  return f;
}


inline void FilterPipeline::insertFilter(int idx, Filter *filter)
{
  if (isValidFilterIndex(idx))
  {
    m_filter_list.insert(m_filter_list.begin() + idx, tFilterPtr(filter));
  }
}


inline void FilterPipeline::removeFilter(int idx)
{
  if (isValidFilterIndex(idx))
  {
    m_filter_list.erase(m_filter_list.begin() + idx);
  }
}


inline void FilterPipeline::moveFilter(int cur_pos, int new_pos)
{
  if ((isValidFilterIndex(cur_pos)) && isValidFilterIndex(new_pos))
  {
    m_filter_list[cur_pos].swap(m_filter_list[new_pos]);
  }
}

#endif
