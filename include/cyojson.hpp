/*
[CyoJSON] cyojson.hpp

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

#ifndef __CYOJSON_HPP
#define __CYOJSON_HPP

////////////////////////////////////////////////////////////////////////////////

#ifndef UNREFERENCED_PARAMETER
#   define UNREFERENCED_PARAMETER(p) (void)(p)
#endif

namespace cyojson
{
    class Parser
    {
    public:
        class Callbacks
        {
        public:
            virtual void ObjectStart(const char* path);
            virtual void ObjectEnd(const char* path);
            virtual void ArrayStart(const char* path);
            virtual void ArrayEnd(const char* path);
            virtual void Value(const char* path, const char* value);
        };

        bool Parse(const char* json, Callbacks& callbacks);

        int getLine() const { return line_; }
        int getColumn() const { return column_; }

    private:
        int line_ = 0;
        int column_ = 0;
    };

    inline void Parser::Callbacks::ObjectStart(const char* path) { UNREFERENCED_PARAMETER(path); }
    inline void Parser::Callbacks::ObjectEnd(const char* path) { UNREFERENCED_PARAMETER(path); }
    inline void Parser::Callbacks::ArrayStart(const char* path) { UNREFERENCED_PARAMETER(path); }
    inline void Parser::Callbacks::ArrayEnd(const char* path) { UNREFERENCED_PARAMETER(path); }
    inline void Parser::Callbacks::Value(const char* path, const char* value) { UNREFERENCED_PARAMETER(path); UNREFERENCED_PARAMETER(value); }
}

#include "detail/cyojson.hpp"

////////////////////////////////////////////////////////////////////////////////

#endif //__CYOJSON_HPP
