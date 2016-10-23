/*
[CyoJSON] example.cpp

The MIT License (MIT)

Copyright (c) 2016 Graham Bull

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "cyojson.hpp"

#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

using namespace cyojson;

///////////////////////////////////////////////////////////////////////////////

namespace
{
    class OutputToStream final : public Parser::Callbacks
    {
    public:
        OutputToStream(std::ostream& out)
            : out_(out)
        {
        }
        OutputToStream(const OutputToStream&) = delete;
        void operator=(const OutputToStream&) = delete;

        void Start(const char* name) override
        {
            if (!path_.empty())
                path_ += '/';
            path_ += name;
        }

        void Value(const char* value) override
        {
            out_ << path_ << " = \"" << value << "\"\n";
        }

        void End() override
        {
            auto pos = path_.find_last_of('/');
            if (pos != path_.npos)
                path_.erase(pos);
            if (path_ == "/")
                path_.clear();
        }

    private:
        std::ostream& out_;
        std::string path_;
    };
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
            throw std::runtime_error("Missing filename");

        std::ifstream file(argv[1]);
        if (!file.is_open())
            throw std::runtime_error("Unable to open file");

        char contents[8 * 1024] = { 0 };
        file.read(contents, sizeof(contents) - 1); //-1 for null termination

        OutputToStream callbacks(std::cout);

        bool success = Parser().Parse(contents, callbacks);
        std::cout << (success ? "Success" : "Error") << std::endl;

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR: " << ex.what() << std::endl;
        return 1;
    }
}
