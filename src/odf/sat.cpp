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

#include <odf/sat.h>

using namespace ODF;

SAT::SAT()
    : sat(),
      have_sat(false)
{
    /* noop */
}

SAT::SAT(const cv::Mat &mask) throw (std::logic_error)
    : have_sat(true)
{
    if (mask.type() != CV_8U) {
        throw std::logic_error("Mask is not of CV_8U type");
    }

    cv::integral(mask / 255, this->sat);
}

double SAT::fillRatio(const cv::Rect &rect) const
{
    return this->fillRatio(rect, rect.area());
}

double SAT::fillRatio(const cv::Rect &rect, unsigned int area) const
{
    int sum;
    int x1 = rect.x;
    int y1 = rect.y;
    int x2 = x1 + rect.width;
    int y2 = y1 + rect.height;

    if (!this->have_sat) {
        return 0.0;
    }

    sum = sat.at<int>(y2, x2) - sat.at<int>(y2, x1) - sat.at<int>(y1, x2)
          + sat.at<int>(y1, x1);

    return sum * 100.0 / area;
}
