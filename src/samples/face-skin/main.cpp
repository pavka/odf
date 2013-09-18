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

#include <opencv2/opencv.hpp>
#include <odf/odf.h>
#include <ostream>
#include <stdexcept>

#include "common/options.h"
#include "common/range.h"
#include "common/hsv.h"

using namespace std;
using namespace ODF;

#define WINDOW_HEIGHT   30
#define WINDOW_WIDTH    30
#define WINDOW_STEP_X   (WINDOW_WIDTH / 8)
#define WINDOW_STEP_Y   (WINDOW_HEIGHT / 8)
#define THRESHOLD       30

class ProcessImage
{
private:
    vector<cv::BackgroundSubtractor*> bg;
    Options opts;
    ostream *out;

public:
    ProcessImage(ostream &out, const Options &opts) throw (runtime_error)
        : out(&out), opts(opts)
    {
        cv::Mat image;
        cv::Mat mask;

        /* prepare background samples
         * see OpenCV documentation on background subtractors */
        for (unsigned int i = 0; i < this->opts.backgrounds.size(); i++) {
            image = cv::imread(this->opts.backgrounds[i]);
            if (image.data == NULL) {
                throw runtime_error("Unable to read background information "
                                    "from " + this->opts.backgrounds[i] + "!");
            }

            this->bg.push_back(new cv::BackgroundSubtractorMOG2(30, 16));
            this->bg[i]->operator()(image, mask);
        }
    }

    void operator () (Image *image)
    {
        BoundingBoxVector bb;
        SlidingWindow window(WINDOW_WIDTH, WINDOW_HEIGHT,
                             WINDOW_STEP_X, WINDOW_STEP_Y);
        cv::Mat mask;
        cv::Mat foreground;
        ostream &out = *(this->out);
        bool ret;

        out << "Processing " << image->getName() << "... ";

        ret = image->open();
        if (!ret) {
            cerr << "unable to open image " << endl;
            return;
        }

        /* remove background */
        if (this->bg.size()) {
            foreground = image->getForegroundMask(this->bg);
        }

        /* threshold image by skin color in HSV mode */
        mask = image->threshold(ProcessImage::threshold,
                                cv::COLOR_BGR2HSV,
                                foreground);

        /* run sliding window over the thresholded image */
        bb = window.run(mask, THRESHOLD);

        /* highlight detected images in the original image */
        out << "found " << bb.size() << " faces" << endl;
        image->highlightObjects(bb, Image::Red);

        if (this->opts.output_dir.length() != 0) {
            image->save(this->opts.output_dir + "/%n");
        } else {
            image->showAndWait("Preview");
        }

        image->close();
    }

    /**
     * Those coefficient were selected to find skin on output from one
     * particular foyer camera with given light conditions.
     */
    static bool threshold(const cv::Vec3b &value)
    {
        /* filter out shadow */
        if (   in_range(value[HUE], _H(18), _H(18))
            && in_range(value[SAT], _S(10), _S(12))) {
            return false;
        } else if (   in_range(value[HUE], 4, _H(0), _H(20), _H(350), _H(360))
                   && in_range(value[SAT], _S(10), _S(30))
                   && in_range(value[VAL], _V(30), _V(50))
                   && value[VAL] > value[SAT]) {
            /* little light */
            return true;
        } else if (   in_range(value[HUE], _H(310), _H(340))
                   && in_range(value[SAT], _S(15), _S(30))
                   && in_range(value[VAL], _V(20), _V(35))
                   && value[VAL] > value[SAT]) {
            /* very little light */
            return true;
        } else if (   in_range(value[HUE], _H(10), _H(25))
                   && in_range(value[SAT], _S(20), _S(40))
                   && in_range(value[VAL], _V(35), _V(45))
                   && value[VAL] > value[SAT]) {
            /* little light */
            return true;
        } else if (   in_range(value[HUE], 4, _H(0), _H(16), _H(340), _H(360))
                   && in_range(value[SAT], _S(25), _S(40))
                   && in_range(value[VAL], _V(50), _V(70))
                   && value[VAL] > value[SAT]) {
            /* medium light */
            return true;
        } else if (   in_range(value[HUE], _H(10), _H(15))
                   && in_range(value[SAT], _S(35), _S(45))
                   && in_range(value[VAL], _V(60), _V(90))
                   && value[VAL] > value[SAT]) {
            /* high light */
            return true;
        } else if (   in_range(value[HUE], _H(0), _H(20))
                   && in_range(value[SAT], _S(10), _S(40))
                   && in_range(value[VAL], _V(85), _V(100))
                   && value[VAL] > value[SAT]) {
            /* very high light */
            return true;
        } else if (   in_range(value[HUE], _H(310), _H(345))
                   && in_range(value[SAT], _S(20), _S(40))
                   && in_range(value[VAL], _V(35), _V(45))
                   && value[VAL] > value[SAT]) {
            /* violet */
            return true;
        } else if (   in_range(value[HUE], _H(285), _H(290))
                   && in_range(value[SAT], _S(13), _S(20))
                   && in_range(value[VAL], _V(25), _V(35))
                   && value[VAL] > value[SAT]) {
            /* violet */
            return true;
        } else if (   in_range(value[HUE], _H(335), _H(337))
                   && in_range(value[SAT], _S(10), _S(15))
                   && in_range(value[VAL], _V(30), _V(35))
                   && value[VAL] > value[SAT]) {
            /* gray */
            return true;
        } else if (   in_range(value[HUE], _H(18), _H(25))
                   && in_range(value[SAT], _S(30), _S(45))
                   && in_range(value[VAL], _V(40), _V(66))
                   && value[VAL] > value[SAT]) {
            /* brown */
            return true;
        } else if (   in_range(value[HUE], _H(290), _H(320))
                   && in_range(value[SAT], _S(0), _S(10))
                   && in_range(value[VAL], _V(90), _V(100))
                   && value[VAL] > value[SAT]) {
            /* over-exposed but some hue left */
            return true;
        }

        return false;
    }
};

int main(int argc, const char **argv)
{
    Options opts;
    ImageSequence images;
    bool ret;

    /* parse command line */
    ret = opts.parse(argc, argv, cerr);
    if (ret != true) {
        Options::printUsage(cerr, argv[0]);
        return 1;
    }

    /* print current configuration */
    opts.print(cout);
    cout << endl;

    /* create sequence of images */
    images = ImageSequence(opts.input_dir, opts.extension, opts.prefix,
                           opts.suffix, opts.num_digits, opts.from,
                           opts.to);

    /* process images */
    try {
        ProcessImage processor(cout, opts);
        images.run(processor);
    } catch (cv::Exception &e) {
        cerr << "OpenCV error:" << endl << e.what() << endl;
    } catch (exception &e) {
        cerr << "Error:" << endl << e.what() << endl;
    }

    return 0;
}
