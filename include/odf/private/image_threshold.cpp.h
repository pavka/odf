/*
The MIT License (MIT)

Copyright (c) 2013 Pavel Březina

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef IMAGE_THRESHOLD_H_
#define IMAGE_THRESHOLD_H_

#include <odf/image.h>

namespace ODF
{
    template <unsigned int arity, typename Functor>
    cv::Mat Image::threshold(Functor &fn) const
    {
        return this->_threshold<arity, Functor>(fn, NULL, cv::COLOR_COLORCVT_MAX);
    }

    template <typename Functor>
    cv::Mat Image::threshold(Functor &fn) const
    {
        return this->_threshold<3, Functor>(fn, NULL, cv::COLOR_COLORCVT_MAX);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             const cv::Mat &mask) const
    {
        return this->_threshold<arity, Functor>(fn, &mask, cv::COLOR_COLORCVT_MAX);
    }

    template <typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             const cv::Mat &mask) const
    {
        return this->_threshold<3, Functor>(fn, &mask, cv::COLOR_COLORCVT_MAX);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             unsigned int convert_to) const
    {
        return this->_threshold<arity, Functor>(fn, NULL, convert_to);
    }

    template <typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             unsigned int convert_to) const
    {
        return this->_threshold<3, Functor>(fn, NULL, convert_to);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             unsigned int convert_to,
                             const cv::Mat &mask) const
    {
        return this->_threshold<arity, Functor>(fn, &mask, convert_to);
    }

    template <typename Functor>
    cv::Mat Image::threshold(Functor &fn,
                             unsigned int convert_to,
                             const cv::Mat &mask) const
    {
        return this->_threshold<3, Functor>(fn, &mask, convert_to);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     const cv::Vec<uchar, arity> &color)
    {
        return this->_threshold<arity, Functor>(fn, NULL, cv::COLOR_COLORCVT_MAX,
                                         color);
    }

    template <typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     const cv::Vec<uchar, 3> &color)
    {
        return this->_threshold<3, Functor>(fn, NULL, cv::COLOR_COLORCVT_MAX, color);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     const cv::Mat &mask,
                                     const cv::Vec<uchar, arity> &color)
    {
        return this->_threshold<arity, Functor>(fn, &mask, cv::COLOR_COLORCVT_MAX,
                                         color);
    }

    template <typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     const cv::Mat &mask,
                                     const cv::Vec<uchar, 3> &color)
    {
        return this->_threshold<3, Functor>(fn, &mask, cv::COLOR_COLORCVT_MAX, color);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     unsigned int convert_to,
                                     const cv::Vec<uchar, arity> &color)
    {
        return this->_threshold<arity, Functor>(fn, NULL, convert_to, color);
    }

    template <typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     unsigned int convert_to,
                                     const cv::Vec<uchar, 3> &color)
    {
        return this->_threshold<3, Functor>(fn, NULL, convert_to, color);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     unsigned int convert_to,
                                     const cv::Mat &mask,
                                     const cv::Vec<uchar, arity> &color)
    {
        return this->_threshold<arity, Functor>(fn, &mask, convert_to, color);
    }

    template <typename Functor>
    cv::Mat Image::thresholdAndPaint(Functor &fn,
                                     unsigned int convert_to,
                                     const cv::Mat &mask,
                                     const cv::Vec<uchar, 3> &color)
    {
        return this->_threshold<3, Functor>(fn, &mask, convert_to, color);
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::_threshold(Functor &threshold_fn,
                              const cv::Mat *input_mask,
                              unsigned int convert_to) const
    {
        cv::Vec<uchar, arity> value;
        uchar *iptr = NULL; /* image data pointer */
        uchar *mptr = NULL; /* mask data pointer */
        cv::Mat image;
        cv::Mat mask;
        bool bret;

        image = this->_thresholdGetImage(input_mask, convert_to);
        mask = cv::Mat::zeros(this->image.rows, this->image.cols, CV_8U);

        for (unsigned int i = 0; i < this->image.rows; i++) {
            iptr = image.ptr<uchar>(i);
            mptr = mask.ptr<uchar>(i);

            for (unsigned int j = 0, m = 0;
                 j < this->image.cols * arity;
                 j += arity, m++) {
                value = cv::Vec<uchar, arity>(&iptr[j]);
                if (threshold_fn(value)) {
                    mptr[m] = 255;
                }
            }
        }

        return mask;
    }

    template <unsigned int arity, typename Functor>
    cv::Mat Image::_threshold(Functor &threshold_fn,
                              const cv::Mat *input_mask,
                              unsigned int convert_to,
                              const cv::Vec<uchar, arity> &color)
    {
        cv::Vec<uchar, arity> value;
        uchar *optr = NULL; /* original data pointer */
        uchar *iptr = NULL; /* image data pointer */
        uchar *mptr = NULL; /* mask data pointer */
        cv::Mat image;
        cv::Mat mask;
        bool bret;

        image = this->_thresholdGetImage(input_mask, convert_to);
        mask = cv::Mat::zeros(this->image.rows, this->image.cols, CV_8U);

        for (unsigned int i = 0; i < this->image.rows; i++) {
            optr = this->image.ptr<uchar>(i);
            iptr = image.ptr<uchar>(i);
            mptr = mask.ptr<uchar>(i);

            for (unsigned int j = 0, m = 0;
                 j < this->image.cols * arity;
                 j += arity, m++) {
                value = cv::Vec<uchar, arity>(&iptr[j]);
                if (threshold_fn(value)) {
                    mptr[m] = 255;

                    for (unsigned int a = 0; a < arity; a++) {
                        optr[j + a] = color[a];
                    }
                }
            }
        }

        return mask;
    }
}

#endif /* IMAGE_THRESHOLD_H_ */
