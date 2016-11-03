/*
[CyoJSON] detail/utf8.hpp

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
#   error Do not #include this file!
#endif

#include <cassert>
#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace cyojson
{
    namespace detail
    {
        inline void OutputUTF8(std::string& out, std::uint16_t value)
        {
            if (value >= 0x800)
            {
                // Output 3 bytes...
                // (aaaabbbb bbcccccc => 1110aaaa 10bbbbbb 10cccccc)
                std::uint16_t a = (value & 0xF0) >> 12;
                out += (char)(0xE0 | a);
                std::uint16_t b = (value & 0xFC) >> 6;
                out += (char)(0x80 | b);
                std::uint16_t c = (value & 0x3F);
                out += (char)(0x80 | c);
            }
            else if (value >= 0x80)
            {
                // Output 2 bytes...
                // (00000aaa aabbbbbb => 110aaaaa 10bbbbbb)
                std::uint16_t a = (value & 0x7C) >> 6;
                out += (char)(0xC0 | a);
                std::uint16_t b = (value & 0x3F);
                out += (char)(0x80 | b);
            }
            else
            {
                // Output 1 byte...
                // (ASCII char, 0-127)
                out += (char)value;
            }
        }

        inline void OutputUTF8(std::string& out, std::uint16_t highSurrogate, std::uint16_t lowSurrogate)
        {
            assert(0xD800 <= highSurrogate && highSurrogate <= 0xDBFF);
            assert(0xDC00 <= lowSurrogate && lowSurrogate <= 0xDFFF);

            std::uint32_t value = 0;
            value |= (highSurrogate & 0x07FF);
            value <<= 10;
            value |= (lowSurrogate & 0x03FF);
            value += 0x10000;

            assert(0x10000 <= value && value <= 0x10FFFF);

            // Output 4 bytes...
            // (00000000 000aaabb bbbbcccc ccdddddd => 11110aaa 10bbbbbb 10cccccc 10dddddd)
            auto a = (value & 0x001C0000) >> 18;
            out += (char)(0xF0 | a);
            auto b = (value & 0x0003F000) >> 12;
            out += (char)(0x80 | b);
            auto c = (value & 0x00000FC0) >> 6;
            out += (char)(0x80 | c);
            auto d = (value & 0x0000003F);
            out += (char)(0x80 | d);
        }
    }
}
