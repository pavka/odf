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

#include <string>
#include <sstream>
#include <iomanip>
#include <odf/image.h>

using namespace ODF;

const cv::Scalar Image::Red = cv::Scalar(0, 0, 255, 0);
const cv::Scalar Image::Green = cv::Scalar(0, 255, 0, 0);
const cv::Scalar Image::Blue = cv::Scalar(255, 0, 0, 0);

const cv::Vec3b Image::V_Red = cv::Vec3b(0, 0, 255);
const cv::Vec3b Image::V_Green = cv::Vec3b(0, 255, 0);
const cv::Vec3b Image::V_Blue = cv::Vec3b(255, 0, 0);

Image::Image(const std::string &filename)
    : filename(filename),
      is_opened(false)
{
    size_t pos = this->filename.find_last_of("/\\");

    if (pos == std::string::npos) {
        this->path = std::string();
        this->name = this->filename;
    } else {
        this->path = this->filename.substr(0, pos);
        this->name = this->filename.substr(pos + 1);
    }
}

bool Image::open()
{
    if (this->is_opened) {
        return true;
    }

    this->image = cv::imread(this->filename);
    if (this->image.data == NULL) {
        return false;
    }

    this->is_opened = true;

    return true;
}

void Image::close()
{
    if (!this->is_opened) {
        return;
    }

    this->image.release();
    this->is_opened = false;
}

bool Image::isOpen() const
{
    return this->is_opened;
}

void Image::show(const std::string &name) const
{
    this->assertIsOpen();

    cv::namedWindow(name);
    cv::imshow(name, this->image);
}

void Image::showAndWait(const std::string &name) const
{
    this->assertIsOpen();

    cv::namedWindow(name);
    cv::imshow(name, this->image);
    cv::waitKey();
    cv::destroyWindow(name);
}

bool Image::save(std::string filename) const
{
    this->assertIsOpen();

    size_t pos = filename.find('%');
    while (pos != std::string::npos) {
        switch (filename[pos + 1]) {
        case 'n':
            filename.replace(pos, 2, this->name);
            break;
        case 'p':
            filename.replace(pos, 2, this->path);
            break;
        case '%':
            filename.replace(pos, 2, "%");
            break;
        }

        pos = filename.find('%', pos);
    }

    return cv::imwrite(filename, this->image);
}

void Image::replaceImage(const cv::Mat &new_image)
{
    if (this->is_opened) {
        this->close();
    }

    this->image = new_image;
    this->is_opened = true;
}

void Image::applyMask(const cv::Mat &mask)
{
    this->assertIsOpen();

    cv::Mat new_image;

    this->image.copyTo(new_image, mask);
    this->replaceImage(new_image);
}

cv::Mat Image::getForegroundMask(cv::BackgroundSubtractor *sub) const
{
    std::vector<cv::BackgroundSubtractor*> subs;
    subs.push_back(sub);

    return this->getForegroundMask(subs);
}

cv::Mat Image::getForegroundMask(std::vector<cv::BackgroundSubtractor*> subs) const
{
    this->assertIsOpen();

    std::vector<cv::BackgroundSubtractor*>::iterator it;
    cv::Mat kernel;
    cv::Mat tmpmask;
    cv::Mat mask;

    mask = cv::Mat::ones(this->image.rows, this->image.cols, CV_8U);
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    for (it = subs.begin(); it != subs.end(); it++) {
        /* we want a very slow learning rate */
        (*it)->operator()(this->image, tmpmask, 0.00001);

        cv::morphologyEx(tmpmask, tmpmask, cv::MORPH_OPEN, kernel);
        mask &= tmpmask;
    }

    return mask;
}

void Image::highlightObjects(const BoundingBoxVector &objects,
                             const cv::Scalar &color,
                             int thickness /*= 2*/)
{
    this->assertIsOpen();

    BoundingBoxVector::const_iterator it;

    for (it = objects.begin(); it != objects.end(); it++) {
        cv::rectangle(this->image, it->getBoundingBox(), color, thickness);
    }
}

void Image::highlightObjects(const std::vector<cv::Rect> &objects,
                             const cv::Scalar &color,
                             int thickness /*= 2*/)
{
    this->assertIsOpen();

    std::vector<cv::Rect>::const_iterator it;

    for (it = objects.begin(); it != objects.end(); it++) {
        cv::rectangle(this->image, *it, color, thickness);
    }
}

const std::string & Image::getName() const
{
    return this->name;
}

const std::string & Image::getPath() const
{
    return this->name;
}

const cv::Mat & Image::getImage() const
{
    this->assertIsOpen();

    return this->image;
}

void Image::assertIsOpen() const throw (std::logic_error)
{
    if (!this->is_opened) {
        throw std::logic_error("Image is not opened");
    }
}

cv::Mat Image::_thresholdGetImage(const cv::Mat *mask,
                                  unsigned int convert_to) const
{
    const cv::Mat *ptr = &this->image;
    cv::Mat converted_image;
    cv::Mat masked_image;

    if (convert_to != cv::COLOR_COLORCVT_MAX) {
        cv::cvtColor(this->image, converted_image, convert_to);
        ptr = &converted_image;
    }

    if (mask != NULL) {
        ptr->copyTo(masked_image, *mask);
        ptr = &masked_image;
    }

    return *ptr;
}

ImageSequence::ImageSequence()
    : list<Image>()
{
    /* noop */
}

ImageSequence::ImageSequence(const std::string &dirpath,
                             const std::string &extension,
                             unsigned short num_digits,
                             unsigned int range_start,
                             unsigned int range_end)
    : list<Image>()
{
    this->createSequence(dirpath, extension, "", "", num_digits,
                         range_start, range_end);
}

ImageSequence::ImageSequence(const std::string &dirpath,
                             const std::string &extension,
                             const std::string &prefix,
                             unsigned short num_digits,
                             unsigned int range_start,
                             unsigned int range_end)
    : list<Image>()
{
    this->createSequence(dirpath, extension, prefix, "",
                         num_digits, range_start, range_end);
}

ImageSequence::ImageSequence(const std::string &dirpath,
                             const std::string &extension,
                             const std::string &prefix,
                             const std::string &suffix,
                             unsigned short num_digits,
                             unsigned int range_start,
                             unsigned int range_end)
    : list<Image>()
{
    this->createSequence(dirpath, extension, prefix, suffix,
                         num_digits, range_start, range_end);
}

void ImageSequence::run(Callback callback)
{
    iterator it;

    for (it = this->begin(); it != this->end(); it++) {
        callback(&(*it));
    }
}

void ImageSequence::createSequence(std::string dirpath,
                                   const std::string &extension,
                                   const std::string &prefix,
                                   const std::string &suffix,
                                   unsigned short num_digits,
                                   unsigned int range_start,
                                   unsigned int range_end)
{
    std::stringstream ss;

    if (!dirpath.empty()
            && (*(dirpath.rbegin()) != '/' || *(dirpath.rbegin()) != '\\')) {
        dirpath.push_back('/');
    }

    if (num_digits != 0) {
        ss << std::setw(num_digits) << std::setfill('0');
    }

    for (unsigned int i = range_start; i <= range_end; i++) {
        ss << dirpath << prefix << std::setw(num_digits) << std::setfill('0') << i << suffix << '.' << extension;
        this->push_back(Image(ss.str()));
        ss.str("");
    }
}
