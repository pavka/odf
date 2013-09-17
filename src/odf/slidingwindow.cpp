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

#include <odf/slidingwindow.h>
#include <odf/boundingbox.h>
#include <odf/sat.h>

using namespace ODF;

SlidingWindow::SlidingWindow(unsigned int width,
                             unsigned int height)
    : width(width),
      height(height),
      area(width * height),
      step_x(width / 8),
      step_y(height / 8)
{
    /* noop */
}

SlidingWindow::SlidingWindow(unsigned int width,
                             unsigned int height,
                             unsigned int step_x,
                             unsigned int step_y)
    : width(width),
      height(height),
      area(width * height),
      step_x(step_x),
      step_y(step_y)
{
    /* noop */
}

BoundingBoxVector SlidingWindow::run(const cv::Mat &mask,
                                     double threshold) const
{
    return this->run(mask, threshold, cv::Rect(0, 0, mask.cols, mask.rows));
}

BoundingBoxVector SlidingWindow::run(const cv::Mat &mask,
                                     double threshold,
                                     const cv::Rect &tile) const
{
    BoundingBoxVector bb;
    SAT sat(mask);
    cv::Point tl, br; /* window coordinates */
    cv::Rect window;
    int height = tile.y + tile.height;
    int width = tile.x + tile.width;
    double fill_ratio;

    for (tl.y = tile.y; tl.y < height; tl.y += this->step_y) {
        /* move and check bottom right y */
        br.y = tl.y + this->height;
        if (br.y >= height) {
            br.y = height - 1;
            if (br.y <= tl.y) {
                break;
            }
        }

        for (tl.x = tile.x; tl.x < width; tl.x += this->step_x) {
            /* move and check bottom right x */
            br.x = tl.x + this->width;
            if (br.x >= width) {
                br.x = width - 1;
                if (br.x <= tl.x) {
                    break;
                }
            }

            window = cv::Rect(tl, br);
            fill_ratio = sat.fillRatio(window, this->area);
            if (fill_ratio > threshold) {
                bb.push(window, fill_ratio);
            }
        }
    }

    return bb;
}
