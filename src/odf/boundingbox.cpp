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

#include <odf/boundingbox.h>

using namespace ODF;

BoundingBox::BoundingBox(const cv::Rect &rect)
    : bounding_box(rect),
      best_fit_box(rect),
      fill_ratio(0.0)
{
    /* noop */
}

BoundingBox::BoundingBox(const cv::Rect &rect, double fill_ratio)
    : bounding_box(rect),
      best_fit_box(rect),
      fill_ratio(fill_ratio)
{
    /* noop */
}

bool BoundingBox::doesIntersect(const cv::Rect &rect) const
{
    cv::Rect intersection;

    intersection = this->bounding_box & rect;
    if (intersection.area() > 0) {
        return true;
    }

    return false;
}

void BoundingBox::expand(const cv::Rect &rect)
{
    this->expand(rect, ODF_BB_INVALID_FILL_RATIO);
}

void BoundingBox::expand(const cv::Rect &rect, double fill_ratio)
{
    this->bounding_box |= rect;

    if (fill_ratio == ODF_BB_INVALID_FILL_RATIO
            || this->fill_ratio >= fill_ratio) {
        return;
    }

    this->fill_ratio = fill_ratio;
    this->best_fit_box = rect;
}

bool BoundingBox::expandIfIntersect(const cv::Rect &rect)
{
    return this->expandIfIntersect(rect, ODF_BB_INVALID_FILL_RATIO);
}

bool BoundingBox::expandIfIntersect(const cv::Rect &rect, double fill_ratio)
{
    bool intersect;

    intersect = this->doesIntersect(rect);
    if (intersect) {
        this->expand(rect, fill_ratio);
    }

    return intersect;
}

cv::Rect BoundingBox::getBoundingBox() const
{
    return this->bounding_box;
}

cv::Rect BoundingBox::getBestFitBox() const
{
    return this->bounding_box;
}

double BoundingBox::getFillRatio() const
{
    return this->getFillRatio();
}

BoundingBoxVector::BoundingBoxVector()
    : vector(),
      sat(),
      area(0),
      area_set(false)
{
    /* noop */
}

BoundingBoxVector::BoundingBoxVector(const SAT &sat)
    : vector(),
      sat(sat),
      area(0),
      area_set(false)
{
    /* noop */
}

BoundingBoxVector::BoundingBoxVector(const SAT &sat, unsigned int area)
    : vector(),
      sat(sat),
      area(area),
      area_set(true)
{
    /* noop */
}

void BoundingBoxVector::push(const cv::Rect &rect)
{
    double fill_ratio;

    if (this->area == 0) {
        fill_ratio = ODF_BB_INVALID_FILL_RATIO;
    } else {
        fill_ratio = this->area_set ? this->sat.fillRatio(rect, this->area)
                                    : this->sat.fillRatio(rect);
    }
    this->push(rect, fill_ratio);
}

void BoundingBoxVector::push(const cv::Rect &rect, double fill_ratio)
{
    iterator it;
    bool found;

    for (it = this->vector.begin(); it != this->vector.end(); it++) {
        found = it->expandIfIntersect(rect, fill_ratio);
        if (found) {
            break;
        }
    }

    if (!found) {
        this->vector.push_back(BoundingBox(rect, fill_ratio));
    }
}

size_t BoundingBoxVector::size() const
{
    return this->vector.size();
}

bool BoundingBoxVector::empty() const
{
    return this->vector.empty();
}

BoundingBoxVector::iterator BoundingBoxVector::begin()
{
    return this->vector.begin();
}

BoundingBoxVector::iterator BoundingBoxVector::end()
{
    return this->vector.end();
}

BoundingBoxVector::const_iterator BoundingBoxVector::begin() const
{
    return this->vector.begin();
}

BoundingBoxVector::const_iterator BoundingBoxVector::end() const
{
    return this->vector.end();
}

BoundingBoxVector::reverse_iterator BoundingBoxVector::rbegin()
{
    return this->vector.rbegin();
}

BoundingBoxVector::reverse_iterator BoundingBoxVector::rend()
{
    return this->vector.rend();
}

BoundingBoxVector::const_reverse_iterator BoundingBoxVector::rbegin() const
{
    return this->vector.rbegin();
}

BoundingBoxVector::const_reverse_iterator BoundingBoxVector::rend() const
{
    return this->vector.rend();
}
