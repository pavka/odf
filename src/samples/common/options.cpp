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

#include <iostream>
#include <sstream>

#include "common/options.h"

using namespace std;

Options::Options()
    : input_dir(""),
      output_dir(""),
      extension(""),
      prefix(""),
      suffix(""),
      num_digits(0),
      from(0),
      to(0)
{
    /* no op */
}

bool Options::get_arg_as_uint(int argc, const char **argv, int *cur,
                              unsigned int *_out) const
{
    stringstream ss;
    unsigned int out;

    (*cur)++;
    if (argc <= *cur) {
        return false;
    }

    ss << argv[*cur];
    ss >> out;

    *_out = out;

    return true;
}

bool Options::get_arg_as_string(int argc, const char **argv, int *cur,
                                std::string *_out) const
{
    (*cur)++;
    if (argc <= *cur) {
        return false;
    }

    *_out = argv[*cur];

    return true;
}

bool Options::parse(int argc, const char **argv, std::ostream &out)
{
    stringstream ss;
    string current;
    bool to_set = false;
    bool from_set = false;
    bool bret;

    if (argc < 2) {
        return false;
    }

    for (int i = 1; i < argc; i++) {
        current = string(argv[i]);
        ss.clear();

        if (current == "-p") {
            bret = this->get_arg_as_string(argc, argv, &i, &this->prefix);
        } else if (current == "-s") {
            bret = this->get_arg_as_string(argc, argv, &i, &this->suffix);
        } else if (current == "-d") {
            bret = this->get_arg_as_uint(argc, argv, &i, &this->num_digits);
        } else if (current == "-f") {
            bret = this->get_arg_as_uint(argc, argv, &i, &this->from);
            from_set = true;
        } else if (current == "-t") {
            bret = this->get_arg_as_uint(argc, argv, &i, &this->to);
            to_set = true;
        } else if (current == "-e") {
            bret = this->get_arg_as_string(argc, argv, &i, &this->extension);
        } else if (current == "-o") {
            bret = this->get_arg_as_string(argc, argv, &i, &this->output_dir);
        } else if (current == "-b") {
            bret = this->get_arg_as_string(argc, argv, &i, &current);
            if (bret) {
                this->backgrounds.push_back(current);
            }
        } else {
            if (this->input_dir.length() != 0) {
                out << "Input directory is already set!" << endl;
                return false;
            }

            this->input_dir = argv[i];
            bret = true;
        }

        if (!bret) {
            out << "Unable to parse option " << argv[i - 1] << "!" << endl;
            return false;
        }
    }

    if (this->input_dir.length() == 0) {
        out << "Input directory is missing!" << endl;
        return false;
    }

    if (!from_set) {
        out << "You have to specify start of the range with -f!" << endl;
        return false;
    }

    if (!to_set) {
        out << "You have to specify end of the range with -t!" << endl;
        return false;
    }

    return true;
}

void Options::print(std::ostream &out) const
{
    std::vector<std::string>::const_iterator it;
    bool first = true;

    out << "Input directory: "      << this->input_dir << endl;
    out << "Output directory: "     << this->output_dir << endl;
    out << "Allowed extension: "    << this->extension << endl;
    out << "Filename prefix: "      << this->prefix << endl;
    out << "Filename suffix: "      << this->suffix << endl;
    out << "Number of digits: "     << this->num_digits << endl;
    out << "File range start: "     << this->from << endl;
    out << "File range to: "        << this->to << endl;
    out << "Background to remove: ";

    for (it = this->backgrounds.begin(); it != this->backgrounds.end(); it++) {
        if (first) {
            out << *it << endl;
            first = false;
        } else {
            out << "                      " << *it << endl;
        }
    }
}

void Options::printUsage(std::ostream &out,
                         const std::string &program_name)
{
    out << "Usage: " << endl;
    out << program_name << " [-p prefix] [-s suffix] [-d num_digits]"
                           " [-e extension] [-o output_dir]"
                           " [-b background [-b ...]]"
                           " -f from -t to input_dir" << endl;
}
