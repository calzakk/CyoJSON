# CyoJSON

CyoJSON is an easy-to-use C++ library for parsing JSON-formatted text.

## Features

* Simple interface.
* Stream-based parsing.
* Invokes callback functions during parsing - object starts/ends, array starts/ends, values.
* Implemented using header files only;
* Uses modern C++.

## Usage

CyoJSON is implemented using header files only, so doesn't need to be separately built prior to use. One header provides the interface, and several headers (in the 'details' subdirectory) provide implementation details that are only indirectly #included.

    #include "cyojson.hpp"
    using namespace cyojson;

### Parser::Callbacks class

Define and instantiate a class that derives from Parser::Callbacks:

    class Callbacks final : public Parser::Callbacks
    {
    public:
        void Value(const char* path, const char* value) override
        {
            std::cout << path << " = \"" << value << "\"" << std::endl;
        }
    };

    Callbacks callbacks;

### Parser class

Now instantiate the Parser class:

    Parser parser;

Pass the JSON text (char*, encoded using UTF-8) and the callbacks:

    bool success = parser.Parse(json, callbacks);

Check for errors:

    if (!success)
    {
        std::cout << "Error on line " << parser.getLine()
            << " at column " << parser.getColumn() << std::endl;
    }

## Tests

Unit tests coming soon!

## Platforms

CyoArguments requires a C++11 compiler, and is known to build on the following platforms:

* Linux using GCC 5.4;
* Windows using Visual Studio 2013.

## License

### The MIT License (MIT)

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
