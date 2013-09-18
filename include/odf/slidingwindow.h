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

#ifndef ODF_SLIDINGWINDOW_H_
#define ODF_SLIDINGWINDOW_H_

#include <opencv2/opencv.hpp>
#include <odf/boundingbox.h>

namespace ODF
{
    class SlidingWindow
    {
    private:
        unsigned int width;
        unsigned int height;
        unsigned int area;
        unsigned int step_x;
        unsigned int step_y;

    public:
        /**
         * Create new sliding window with dimensions 'width' x 'height',
         * horizontal stepping equal to 'width' / 8 and and vertical stepping
         * equal to 'height' / 8.
         *
         * @param[in] width Window width.
         * @param[in] height Window height.
         */
        SlidingWindow(unsigned int width,
                      unsigned int height);

        /**
         * Create new sliding window with dimensions 'width' x 'height',
         * horizontal stepping equal to 'width' and and vertical stepping
         * equal to 'height'.
         *
         * @param[in] width Window width.
         * @param[in] height Window height.
         * @param[in] step_x Horizontal stepping.
         * @param[in] step_y Vertical stepping.
         */
        SlidingWindow(unsigned int width,
                      unsigned int height,
                      unsigned int step_x,
                      unsigned int step_y);

        /**
         * Move sliding window over the 'mask'. If the area covered by the
         * window exceeds 'threshold' percent, it is pushed inside bounding
         * box vector.
         *
         * @param[in] mask 8-bit image which contains only values 0 and 255.
         * @param[in] threshold Threshold for object detection.
         *
         * @throws logic_error if the mask is not in CV_8U format.
         */
        BoundingBoxVector run(const cv::Mat &mask,
                              double threshold) const;

        /**
         * Move sliding window over the 'tile' in the 'mask'. If the area
         * covered by the window exceeds 'threshold' percent it is pushed
         * inside bounding box vector.
         *
         * @param[in] mask 8-bit image which contains only values 0 and 255.
         * @param[in] threshold Threshold for object detection.
         * @param[in] tile Area that is searched at.
         */
        BoundingBoxVector run(const cv::Mat &mask,
                              double threshold,
                              const cv::Rect &tile) const;
    };
}

#endif /* ODF_SLIDINGWINDOW_H_ */
