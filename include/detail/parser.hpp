/*
[CyoJSON] detail/parser.hpp

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

#include "utf8.hpp"

#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace cyojson
{
    namespace detail
    {
        class ParserImpl final
        {
        public:
            ParserImpl(const char* next, Parser::Callbacks& callbacks)
                : next_(next),
                callbacks_(callbacks)
            {
            }

            ParserImpl(const ParserImpl&) = delete;

            void operator=(const ParserImpl&) = delete;

            bool Parse(int& line, int& column)
            {
                if (!ParseObject())
                {
                    line = line_;
                    column = column_;
                    return false;
                }

                SkipWhitespace();

                line = line_;
                column = column_;
                return (*next_ == '\0');
            }

        private:
            const int c_firstLine = 1;
            const int c_firstColumnOfLine = 1;
            const char* const c_true = "true";
            const char* const c_false = "false";
            const char* const c_null = "null";

            int line_ = c_firstLine;
            int column_ = c_firstColumnOfLine;
            const char* next_;
            std::string path_;
            Parser::Callbacks& callbacks_;

            void NextChar()
            {
                ++next_;
                ++column_;
            }

            void NextChars(int count)
            {
                next_ += count;
                column_ += count;
            }

            void NewLine()
            {
                column_ = c_firstColumnOfLine;
                ++line_;
            }

            void SkipWhitespace()
            {
                while (*next_ != '\0')
                {
                    switch (*next_)
                    {
                    case '\r':
                        if (*(next_ + 1) == '\n')
                            next_ += 2;
                        else
                            ++next_;
                        NewLine();
                        continue;

                    case '\n':
                        ++next_;
                        NewLine();
                        continue;
                    }

                    if (std::isspace(*next_))
                        NextChar();
                    else
                        break;
                }
            }

            bool IsNext(char ch, bool skipWhitespace = true)
            {
                assert(ch != '\0');
                assert(!std::isspace(ch));

                if (skipWhitespace)
                    SkipWhitespace();

                if (*next_ != ch)
                    return false;

                NextChar();
                return true;
            }

            bool IsNext(char ch1, char ch2, bool skipWhitespace = true)
            {
                assert(ch1 != '\0' && ch2 != '\0');
                assert(!std::isspace(ch1) && !std::isspace(ch2));

                if (skipWhitespace)
                    SkipWhitespace();

                if (*next_ != ch1 && *next_ != ch2)
                    return false;

                NextChar();
                return true;
            }

            bool IsNext(const char* str, bool skipWhitespace = true)
            {
                assert(str && *str);

                if (skipWhitespace)
                    SkipWhitespace();

                const char* next = next_;
                const char* nextStr = str;
                while (*nextStr && *nextStr == *next)
                {
                    ++next;
                    ++nextStr;
                }
                if (*nextStr)
                    return false;

                NextChars((int)(nextStr - str));
                return true;
            }

            bool ParseString(std::string& out)
            {
                assert(out.empty());

                if (!IsNext('\"'))
                    return false;

                for (;;)
                {
                    char ch = *next_;
                    if (ch == '\"')
                        break;

                    NextChar();

                    if (ch != '\\')
                    {
                        out += ch;
                        continue;
                    }

                    if (*next_ == 'u')
                    {
                        if (!ParseUnicode(out))
                            return false;
                        else
                            continue;
                    }

                    const char* const escapes = "\"\"\\\\//b\bf\fn\nr\rt\t";
                    const char* escape = std::strchr(escapes, *next_);
                    if (escape == nullptr)
                        return false;
                    out += *(escape + 1);
                    NextChar();
                }

                if (!IsNext('\"'))
                    return false;

                return true;
            }

            bool ParseNumber(std::string& out)
            {
                assert(out.empty());

                SkipWhitespace();

                const char* numberStart = next_;

                IsNext('-', false);

                if (!IsNext('0', false))
                {
                    while (std::isdigit(*next_))
                        NextChar();
                }

                if (IsNext('.', false))
                {
                    while (std::isdigit(*next_))
                        NextChar();
                }

                if (IsNext('e', 'E', false))
                {
                    IsNext('+', '-', false);

                    while (std::isdigit(*next_))
                        NextChar();
                }

                const char* numberEnd = next_;
                if (numberStart == numberEnd)
                    return false;

                out = std::string(numberStart, numberEnd - numberStart);
                return true;
            }

            bool ParseValue()
            {
                SkipWhitespace();

                switch (*next_)
                {
                case '{':
                    return ParseObject();

                case '[':
                    return ParseArray();

                case '\"':
                {
                    std::string str;
                    if (!ParseString(str))
                        return false;
                    callbacks_.Value(path_.c_str(), str.c_str());
                    return true;
                }

                case 't':
                    if (!IsNext(c_true))
                        return false;
                    callbacks_.Value(path_.c_str(), c_true);
                    return true;

                case 'f':
                    if (!IsNext(c_false))
                        return false;
                    callbacks_.Value(path_.c_str(), c_false);
                    return true;

                case 'n':
                    if (!IsNext(c_null))
                        return false;
                    callbacks_.Value(path_.c_str(), c_null);
                    return true;

                default:
                {
                    std::string num;
                    if (!ParseNumber(num))
                        return false;
                    callbacks_.Value(path_.c_str(), num.c_str());
                    return true;
                }
                }
            }

            bool ParseArray()
            {
                if (!IsNext('['))
                    return false;

                callbacks_.ArrayStart(path_.c_str());

                for (;;)
                {
                    if (!ParseValue())
                        break;

                    if (!IsNext(','))
                        break;
                }

                if (!IsNext(']'))
                    return false;

                callbacks_.ArrayEnd(path_.c_str());

                return true;
            }

            bool ParseObject()
            {
                if (!IsNext('{'))
                    return false;

                callbacks_.ObjectStart(path_.c_str());

                for (;;)
                {
                    std::string name;
                    if (!ParseString(name))
                        break;
                    if (!path_.empty())
                        path_ += ':';
                    path_ += name;

                    if (!IsNext(':'))
                        break;

                    if (!ParseValue())
                        break;

                    auto pos = path_.find_last_of(':');
                    if (pos != path_.npos)
                        path_.erase(pos);
                    else
                        path_.clear();

                    if (!IsNext(','))
                        break;
                }

                if (!IsNext('}'))
                    return false;

                callbacks_.ObjectEnd(path_.c_str());

                return true;
            }

            bool ParseUnicode(std::string& out)
            {
                NextChar(); //u

                // Parse first (or only) char...

                std::uint16_t value;
                if (!ParseUnicodeChar(value))
                    return false;

                if (value < 0xD800 || value >= 0xE000)
                {
                    OutputUTF8(out, value);
                    return true;
                }

                std::uint16_t highSurrogate = value;
                if (highSurrogate >= 0xDC00)
                    return false; //invalid high surrogate

                assert(0xD800 <= highSurrogate && highSurrogate <= 0xDBFF);

                // Parse second char...

                if (!IsNext('\\'))
                    return false;
                if (!IsNext('u'))
                    return false;

                std::uint16_t lowSurrogate;
                if (!ParseUnicodeChar(lowSurrogate))
                    return false;

                assert(0xDC00 <= lowSurrogate && lowSurrogate <= 0xDFFF);

                OutputUTF8(out, highSurrogate, lowSurrogate);
                return true;
            }

            bool ParseUnicodeChar(std::uint16_t& value)
            {
                value = 0;
                for (int i = 0; i < 4; ++i)
                {
                    std::uint16_t curr;
                    if ('a' <= *next_ && *next_ <= 'z')
                        curr = ((*next_ - 'a') + 10);
                    else if ('A' <= *next_ && *next_ <= 'Z')
                        curr = ((*next_ - 'A') + 10);
                    else if ('0' <= *next_ && *next_ <= '9')
                        curr = (*next_ - '0');
                    else
                        return false;
                    value <<= 4;
                    value |= curr;
                    NextChar();
                }
                return true;
            }
        };
    }

    inline bool Parser::Parse(const char* json, Callbacks& callbacks)
    {
        if (!json)
            return false;

        detail::ParserImpl impl(json, callbacks);
        return impl.Parse(line_, column_);
    }
}
