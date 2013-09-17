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

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>
#include <ostream>
#include <vector>

class Options {
public:
    std::string input_dir;
    std::string output_dir;
    std::string extension;
    std::string prefix;
    std::string suffix;
    std::vector<std::string> backgrounds;
    unsigned int num_digits;
    unsigned int from;
    unsigned int to;

    Options();

    bool parse(int argc, const char **argv, std::ostream &out);

    void print(std::ostream &out) const;

    static void printUsage(std::ostream &out,
                           const std::string &program_name);

private:
    bool get_arg_as_uint(int argc, const char **argv, int *cur,
                         unsigned int *_out) const;

    bool get_arg_as_string(int argc, const char **argv, int *cur,
                           std::string *_out) const;
};

#endif /* OPTIONS_H_ */
