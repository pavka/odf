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

#ifndef ODF_IMAGE_H_
#define ODF_IMAGE_H_

#include <opencv2/opencv.hpp>
#include <list>
#include <vector>
#include <string>
#include <stdexcept>
#include <odf/boundingbox.h>

/*
 * HSV convertion macros.
 *
 * Normalize HSV values into OpenCV supported range.
 */

/**
 * Convert HSV hue from 0-360° to 0-180°.
 */
#define ODF_CONV_HUE(x) ((double)((double)x/2.0))

/**
 * Convert HSV saturation from 0-100% to 0-255.
 */
#define ODF_CONV_SAT(x) ((double)((double)x*255/100))

/**
 * Convert HSV value from 0-100% to 0-255.
 */
#define ODF_CONV_VAL(x) ODF_CONV_SAT(x)

namespace ODF
{
    /**
     * Indices of RGB values as returned in cv::Vec3b.
     */
    enum rgb_index {BLUE, GREEN, RED};

    /**
     * Indices of YCrCb values as returned in cv::Vec3b.
     */
    enum ycrcb_index {Y, Cr, Cb};

    /**
     * Indices of HSV values as returned in cv::Vec3b.
     */
    enum hsv_index {HUE, SAT, VAL};

    class Image
    {
    private:
        std::string filename;
        std::string path;
        std::string name;

        cv::Mat image;
        bool is_opened;
    public:
        static const cv::Scalar Red;
        static const cv::Scalar Green;
        static const cv::Scalar Blue;

        static const cv::Vec3b V_Red;
        static const cv::Vec3b V_Green;
        static const cv::Vec3b V_Blue;

        /**
         * Create a new image object associate with image 'filename'.
         * This image is not loaded into memory. To do so call Image::open().
         *
         * @param[in] filename Image file.
         *
         * @see Image::open()
         */
        Image(const std::string &filename);

        /**
         * Load the image into memory.
         * If the image is already opened, it is a noop.
         *
         * @see Image::close()
         */
        bool open();

        /**
         * Remove the image from memory.
         * If the image is already closed, it is a noop.
         *
         * @see Image::open()
         */
        void close();

        /**
         * @return True if the image is loaded into memory, false otherwise.
         */
        bool isOpen() const;

        /**
         * Display current image in a new window with title 'name'.
         *
         * @param[in] name Window title.
         *
         * @throws logic_error if the image is not opened.
         */
        void show(const std::string &name) const;

        /**
         * Display current image in a new window with title 'name'. The window
         * will be closed on a key press and the program is blocked until then.
         *
         * @param[in] name Window title.
         *
         * @throws logic_error if the image is not opened.
         */
        void showAndWait(const std::string &name) const;

        /**
         * Save the image into 'filename'. You can use following printf style
         * format:
         * * %n = file name of the image used in constructor
         * * %p = path to the image used in constructors
         * * %% = %
         *
         * @param[in] filename
         *
         * @throws logic_error if the image is not opened.
         */
        bool save(std::string filename) const;

        /**
         * Replace current image matrix with a new one.
         *
         * @param[in] new_image New image matrix.
         */
        void replaceImage(const cv::Mat &new_image);

        /**
         * Applies mask on current image matrix.
         *
         * @param[in] mask Image mask.
         *
         * @throws logic_error if the image is not opened.
         */
        void applyMask(const cv::Mat &mask);

        /**
         * Compute foreground mask.
         *
         * @param[in] sub Background subtractor.
         *
         * @throws logic_error if the image is not opened.
         */
        cv::Mat getForegroundMask(cv::BackgroundSubtractor *sub) const;

        /**
         * Compute foreground mask using several background subtractors.
         *
         * This can be used when processing an image sequence where lighting
         * condition change in time. In this case you can provide several
         * subtractors for different lighting conditions.
         *
         * @param[in] subs Vector of pointers to background subtractors.
         *
         * @throws logic_error if the image is not opened.
         */
        cv::Mat getForegroundMask(std::vector<cv::BackgroundSubtractor*> subs) const;

        /**
         * Paint rectangle in image for each bounding box in 'objects'.
         *
         * @param[in] objects
         * @param[in] color
         * @param[in] thickness
         *
         * @throws logic_error if the image is not opened.
         */
        void highlightObjects(const BoundingBoxVector &objects,
                              const cv::Scalar &color,
                              int thickness = 2);

        /**
         * Paint rectangle in image for each bounding box in 'objects'.
         *
         * @param[in] objects
         * @param[in] color
         * @param[in] thickness
         *
         * @throws logic_error if the image is not opened.
         */
        void highlightObjects(const std::vector<cv::Rect> &objects,
                              const cv::Scalar &color,
                              int thickness = 2);

        /**
         * Threshold image using function 'fn'.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat threshold(Functor &fn) const;

        /**
         * Threshold image using function 'fn'.
         *
         * Arity of the image color space is 3.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, 3> &);
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat threshold(Functor &fn) const;

        /**
         * Threshold image using function 'fn'. Apply mask on the image first,
         * then threshold.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] mask Image mask.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat threshold(Functor &fn,
                          const cv::Mat &mask) const;

        /**
         * Threshold image using function 'fn'. Apply mask on the image first,
         * then threshold.
         *
         * Arity of the image color space is 3.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] mask Image mask.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat threshold(Functor &fn,
                          const cv::Mat &mask) const;

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first, then threshold.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat threshold(Functor &fn,
                          unsigned int convert_to) const;

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first, then threshold.
         *
         * Arity of the image color space is 3.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat threshold(Functor &fn,
                          unsigned int convert_to) const;

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first and apply 'mask', then threshold.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] mask Image mask.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat threshold(Functor &fn,
                          unsigned int convert_to,
                          const cv::Mat &mask) const;

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first and apply 'mask', then threshold.
         *
         * Arity of the image color space is 3.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] mask Image mask.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat threshold(Functor &fn,
                          unsigned int convert_to,
                          const cv::Mat &mask) const;

        /**
         * Threshold image using function 'fn'.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  const cv::Vec<uchar, arity> &color);

        /**
         * Threshold image using function 'fn'.
         *
         * Arity of the image color space is 3.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  const cv::Vec<uchar, 3> &color);

        /**
         * Threshold image using function 'fn'. Apply mask on the image first,
         * then threshold.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] mask Image mask.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  const cv::Mat &mask,
                                  const cv::Vec<uchar, arity> &color);

        /**
         * Threshold image using function 'fn'. Apply mask on the image first,
         * then threshold.
         *
         * Arity of the image color space is 3.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] mask Image mask.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  const cv::Mat &mask,
                                  const cv::Vec<uchar, 3> &color);

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first, then threshold.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  unsigned int convert_to,
                                  const cv::Vec<uchar, arity> &color);

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first, then threshold.
         *
         * Arity of the image color space is 3.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  unsigned int convert_to,
                                  const cv::Vec<uchar, 3> &color);

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first and apply 'mask', then threshold.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] mask Image mask.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <unsigned int arity, typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  unsigned int convert_to,
                                  const cv::Mat &mask,
                                  const cv::Vec<uchar, arity> &color);

        /**
         * Threshold image using function 'fn'. Convert the image to
         * 'convert_to' format first and apply 'mask', then threshold.
         *
         * Arity of the image color space is 3.
         *
         * This method will also paint a point into the original image
         * for each pixel that conforms the threshold. This is useful for
         * debugging.
         *
         * @param[in] fn bool threshold(const cv::Vec<uchar, arity> &);
         * @param[in] convert_to cv::COLOR_*2* values.
         * @param[in] mask Image mask.
         * @param[in] color Color of the point.
         *
         * @return Threshold mask.
         */
        template <typename Functor>
        cv::Mat thresholdAndPaint(Functor &fn,
                                  unsigned int convert_to,
                                  const cv::Mat &mask,
                                  const cv::Vec<uchar, 3> &color);

        /**
         * Get image name.
         */
        const std::string & getName() const;

        /**
         * Get image location.
         */
        const std::string & getPath() const;

        /**
         * Get image matrix.
         *
         * @throws logic_error if the image is not opened.
         */
        const cv::Mat & getImage() const;

    private:
        void assertIsOpen() const throw (std::logic_error);

        cv::Mat _thresholdGetImage(const cv::Mat *mask,
                                   unsigned int convert_to) const;

        template <unsigned int arity, typename Functor>
        cv::Mat _threshold(Functor &threshold_fn,
                           const cv::Mat *input_mask,
                           unsigned int convert_to) const;

        template <unsigned int arity, typename Functor>
        cv::Mat _threshold(Functor &threshold_fn,
                           const cv::Mat *input_mask,
                           unsigned int convert_to,
                           const cv::Vec<uchar, arity> &color);
    };

    class ImageSequence : public std::list<Image>
    {
    public:
        typedef void (*Callback)(Image *image);

        /**
         * Create an empty image sequence.
         */
        ImageSequence();

        /**
         * Load image sequence from directory 'dirpath'. The images are not
         * opened, this is left for the user.
         *
         * This method will load files, which names create an +1 arithmetic
         * sequence. Each file name consist of a number, zero padded from
         * left side to 'digits_num' digits.
         *
         * E. g.:
         * ImageSequence("/my/images", "png", 3, 1, 100);
         * * will load images 001.png - 100.png
         *
         * @param[in] dirpath Directory where the images are located.
         * @param[in] extension Image extension.
         * @param[in] num_digits Number of digits in image name (0 means do not pad).
         * @param[in] range_start Range start.
         * @param[in] range_end Range end.
         */
        ImageSequence(const std::string &dirpath,
                      const std::string &extension,
                      unsigned short num_digits,
                      unsigned int range_start,
                      unsigned int range_end);

        /**
         * Load image sequence from directory 'dirpath'. The images are not
         * opened, this is left for the user.
         *
         * This method will load files, which names create an +1 arithmetic
         * sequence. Each file name consist of a number, zero padded from
         * left side to 'digits_num' digits, and having 'prefix'.
         *
         * E. g.:
         * ImageSequence("/my/images", "png", "CAM-", 3, 1, 100);
         * * will load images CAM-001.png - CAM-100.png
         *
         * @param[in] dirpath Directory where the images are located.
         * @param[in] extension Image extension.
         * @param[in] prefix Prefix.
         * @param[in] num_digits Number of digits in image name (0 means do not pad).
         * @param[in] range_start Range start.
         * @param[in] range_end Range end.
         */
        ImageSequence(const std::string &dirpath,
                      const std::string &extension,
                      const std::string &prefix,
                      unsigned short num_digits,
                      unsigned int range_start,
                      unsigned int range_end);

        /**
         * Load image sequence from directory 'dirpath'. The images are not
         * opened, this is left for the user.
         *
         * This method will load files, which names create an +1 arithmetic
         * sequence. Each file name consist of a number, zero padded from
         * left side to 'digits_num' digits, and having 'prefix' and 'suffix'.
         *
         * E. g.:
         * ImageSequence("/my/images", "png", "CAM-", "-orig", 3, 1, 100);
         * * will load images CAM-001-orig.png - CAM-100-orig.png
         *
         * @param[in] dirpath Directory where the images are located.
         * @param[in] extension Image extension.
         * @param[in] prefix Prefix.
         * @param[in] suffix Suffix.
         * @param[in] num_digits Number of digits in image name (0 means do not pad).
         * @param[in] range_start Range start.
         * @param[in] range_end Range end.
         */
        ImageSequence(const std::string &dirpath,
                      const std::string &extension,
                      const std::string &prefix,
                      const std::string &suffix,
                      unsigned short num_digits,
                      unsigned int range_start,
                      unsigned int range_end);

        /**
         * Run 'callback' on each image in image sequence.
         * This method does not manipulate with any image object -- most
         * importantly it does not open, save nor close the image. This is left
         * on the callback when needed.
         *
         * @param[in] callback Function to be invoked.
         */
        void run(Callback callback);

        /**
         * Run 'callback' on each image in image sequence.
         * This method does not manipulate with any image object -- most
         * importantly it does not open, save nor close the image. This is left
         * on the callback when needed.
         *
         * @param[in] callback Functor to be invoked.
         */
        template <typename Functor>
        void run(Functor &callback)
        {
            iterator it;

            for (it = this->begin(); it != this->end(); it++) {
                callback(&(*it));
            }
        }

    private:
        void createSequence(std::string dirpath,
                            const std::string &extension,
                            const std::string &prefix,
                            const std::string &suffix,
                            unsigned short num_digits,
                            unsigned int range_start,
                            unsigned int range_end);
    };
}

/* include definition of templated methods */
#include <odf/private/image_threshold.cpp.h>

#endif /* ODF_IMAGE_H_ */
