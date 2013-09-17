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

#ifndef ODF_BOUNDINGBOX_H_
#define ODF_BOUNDINGBOX_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <odf/sat.h>

#define ODF_BB_INVALID_FILL_RATIO -1.0

namespace ODF
{
    class BoundingBox
    {
    private:
        cv::Rect bounding_box;
        cv::Rect best_fit_box;
        double fill_ratio;

    public:
        /**
         * Create new bounding box.
         *
         * @param[in] rect Initial box.
         */
        BoundingBox(const cv::Rect &rect);

        /**
         * Create new bounding box with initial fill ratio.
         *
         * @param[in] rect Initial box.
         * @param[in] fill_ratio Area that belongs to the bounded object [%].
         */
        BoundingBox(const cv::Rect &rect, double fill_ratio);

        /**
         * Does current bounding box intersect with 'rect'?
         *
         * @param[in] rect
         */
        bool doesIntersect(const cv::Rect &rect) const;

        /**
         * Union current bounding box with 'rect'. If given 'fill_ratio' is
         * better than current fill ratio, best fist will be changed.
         *
         * If 'fill_ratio' is -1.0, the value is ignored.
         *
         * @param[in] rect Rectangle to add.
         */
        void expand(const cv::Rect &rect);

        /**
         * Union current bounding box with 'rect'. If given 'fill_ratio' is
         * better than current fill ratio, best fist will be changed.
         *
         * @param[in] rect Rectangle to add.
         * @param[in] fill_ratio Area that belongs to the bounded object [%].
         */
        void expand(const cv::Rect &rect, double fill_ratio);

        /**
         * Expand bounding box only if 'rect' intersect current bounding box.
         *
         * If 'fill_ratio' is -1.0, the value is ignored.
         *
         * @param[in] rect Rectangle to add.
         */
        bool expandIfIntersect(const cv::Rect &rect);
        /**
         * Expand bounding box only if 'rect' intersect current bounding box.
         *
         * @param[in] rect Rectangle to add.
         * @param[in] fill_ratio Area that belongs to the bounded object [%].
         */
        bool expandIfIntersect(const cv::Rect &rect, double fill_ratio);


        /**
         * Get bounding box.
         */
        cv::Rect getBoundingBox() const;

        /**
         * Get box that has the best fill ratio.
         */
        cv::Rect getBestFitBox() const;

        /**
         * Get fill ratio of the best fit box.
         */
        double getFillRatio() const;
    };

    class BoundingBoxVector
    {
    private:
        std::vector<BoundingBox> vector;
        SAT sat;
        unsigned int area;
        bool area_set;

    public:
        typedef std::vector<BoundingBox>::iterator iterator;
        typedef std::vector<BoundingBox>::const_iterator const_iterator;
        typedef std::vector<BoundingBox>::reverse_iterator reverse_iterator;
        typedef std::vector<BoundingBox>::const_reverse_iterator const_reverse_iterator;

        /**
         * Create new bounding box vector.
         */
        BoundingBoxVector();

        /**
         * Create new bounding box vector. When a new rectangle is pushed,
         * it's fill ratio will be computed from given SAT.
         *
         * @param[in] sat Summed area table of object mask.
         */
        BoundingBoxVector(const SAT &sat);

        /**
         * Create new bounding box vector. When a new rectangle is pushed,
         * it's fill ratio will be computed from given SAT and area.
         *
         * @param[in] sat Summed area table of objects mask.
         * @param[in] area Referenced area size for fill ratio.
         */
        BoundingBoxVector(const SAT &sat, unsigned int area);

        /**
         * Push rectangle inside bounding box that intersects it.
         * Fill ratio will be computed if SAT and area where provided in
         * constructor.
         *
         * @param[in] rect Rectangle.
         */
        void push(const cv::Rect &rect);

        /**
         * Push rectangle inside bounding box that intersects it.
         *
         * @param[in] rect Rectangle.
         * @param[in] fill_ratio Area that belongs to the bounded object [%].
         */
        void push(const cv::Rect &rect, double fill_ratio);

        /**
         * @return Number of elements in the vector.
         */
        size_t size() const;

        /**
         * @return True if the vector is empty, false otherwise.
         */
        bool empty() const;

        /**
         * @return Iterator to beginning.
         */
        iterator begin();

        /**
         * @return Iterator to end.
         */
        iterator end();

        /**
         * @return Constant iterator to beginning.
         */
        const_iterator begin() const;

        /**
         * @return Constant iterator to end.
         */
        const_iterator end() const;

        /**
         * @return Reverse iterator to beginning.
         */
        reverse_iterator rbegin();

        /**
         * @return Reverse iterator to end.
         */
        reverse_iterator rend();

        /**
         * @return Constant reverse iterator to beginning.
         */
        const_reverse_iterator rbegin() const;

        /**
         * @return Constant reverse iterator to end.
         */
        const_reverse_iterator rend() const;
    };
}

#endif /* ODF_BOUNDINGBOX_H_ */
