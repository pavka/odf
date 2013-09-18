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

#ifndef ODF_SAT_H_
#define ODF_SAT_H_

#include <opencv2/opencv.hpp>
#include <stdexcept>

namespace ODF
{
    class SAT
    {
    private:
        cv::Mat sat;
        bool have_sat;

    public:
        /**
         * Create an empty summed area table that contains only zeros.
         */
        SAT();

        /**
         * Create summed area table for given mask.
         *
         * @param[in] mask 8-bit image which contains only values 0 and 255.
         *
         * @throws logic_error if the mask is not in CV_8U format.
         */
        SAT(const cv::Mat &mask) throw (std::logic_error);

        /**
         * Computes how many pixels of the rectangle area is covered by
         * input mask. Referenced area size is calculated from the rectangle
         * dimensions.
         *
         * @param[in] rect Rectangle placed somewhere inside the mask.
         *
         * @return Filled ratio in %.
         */
        double fillRatio(const cv::Rect &rect) const;

        /**
         * Computes how many pixels of the area is covered by
         * input mask in rectangle 'rect'.
         *
         * @param[in] rect Rectangle placed somewhere inside the mask.
         * @param[in] area Referenced area size.
         *
         * @return Filled ratio in %.
         */
        double fillRatio(const cv::Rect &rect, unsigned int area) const;
    };
}

#endif /* ODF_SAT_H_ */
