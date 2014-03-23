#ifndef FILTER_H
#define FILTER_H

#include <stdexcept>


namespace cv { class Mat; }


/**
 * @brief Exception type thrown by all the filter classes.
 */
class FilterException : public std::runtime_error
{
};


/**
 * @brief Abstract Base filter class
 */
class Filter
{
  public:
    //explicit Filter(CLContext *ctx);

    /**
     * @brief ~Filter destructor
     */
    virtual ~Filter(void) { }

    /**
     * @brief init all additional one time initializations should
     * be implemented in this function, which shall in turn be called
     * once before the filter is used.
     * @return true on success, false on error
     */
    virtual bool init(void) = 0;

    /**
     * @brief exec filters the specified image
     * @param image the image to be filtered (servers as input as well as output)
     * @return true on success, false otherwise
     */
    virtual bool exec(cv::Mat & image) = 0;

  private:
    //CLContext *m_ctx; // OpenCL context
};

#endif
