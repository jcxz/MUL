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

#ifndef FILTER_H
#define FILTER_H

#include <stdexcept>
#include <string>

// A macro to resolve path to OpenCL kernel source files
#define FILTER_H_STR_(x) #x
#define FILTER_H_STR(x) FILTER_H_STR_(x)
#define CLSRCFILE(filename) FILTER_H_STR(CLSOURCEPATH) "/" filename
//#define CLSOURCE(filename) (CLSOURCEPATH "/" filename)


class QCLContext;
class QCLImage2D;


/**
 * @brief Exception type thrown by all the filter classes.
 */
class FilterException : public std::runtime_error
{
  public:
    explicit FilterException(const char *msg = "")
      : std::runtime_error(msg)
    { }

    explicit FilterException(std::string msg)
      : std::runtime_error(msg)
    { }
};


/**
 * @brief Abstract Base filter class
 */
class Filter
{
  public:
    /**
     * @brief Filter constructor
     * @param ctx a pointer to the OpenCL context.
     * In case OpenCL is not supported by your filter
     * pass nullptr to this argument.
     */
    explicit Filter(QCLContext *ctx) : m_ctx(ctx), m_good(false) { }

    /**
     * @brief ~Filter destructor
     */
    virtual ~Filter(void) { }

    QCLContext *clContext(void) const { return m_ctx; }
    void setCLContext(QCLContext *ctx) { m_ctx = ctx; }

    /**
     * @brief hasOpenCL this function can be used to query
     * the filter for OpenCL support
     * @return true in case OpenCL is supported, false otherwise
     */
    bool hasOpenCL(void) const { return m_ctx != nullptr; }

    /**
     * @brief isGood determines whether initialization succeeded
     * @return true in case the Filter is usable, false otherwise
     */
    bool isGood(void) const { return m_good; }

    /**
     * @brief operator bool determines whether the filter is usable
     */
    operator bool(void) const { return m_good; }

    /**
     * @brief run runs the filter on specified image
     * @param src the array of pixels of input image,
     * the pixel channels are given in bgra order
     * @param w width of the input image
     * @param h heigth of the input image
     * @param dst the array of pixels of output image,
     * the output pixels must be in bgra order
     * @return true on success, false otherwise
     */
    virtual bool run(const unsigned char *src, int w, int h, unsigned char *dst) = 0;

    /**
     * @brief runCL runs the OpenCL version of filter on the specified image
     * @param src the image to be filtered
     * @param dst the resulting image
     * @return true on success, false otherwise
     */
    virtual bool runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst) = 0;

  protected:
    QCLContext *m_ctx; // OpenCL context
    bool m_good;       // determines whether the initialization failed or not
};

#endif
