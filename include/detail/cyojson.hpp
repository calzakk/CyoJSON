/*
[CyoJSON] detail/cyojson.hpp

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
#include <cctype>
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

            bool Parse()
            {
                if (!ParseObject())
                    return false;

                SkipWhitespace();

                return (*next_ == '\0');
            }

        private:
            const char* const c_true = "true";
            const char* const c_false = "false";
            const char* const c_null = "null";

            void SkipWhitespace()
            {
                while (std::isspace(*next_))
                    ++next_;
            }

            bool IsNext(char ch, bool skipWhitespace = true)
            {
                assert(ch != '\0');

                if (skipWhitespace)
                    SkipWhitespace();

                if (*next_ != ch)
                    return false;

                ++next_;
                return true;
            }

            bool IsNext(char ch1, char ch2, bool skipWhitespace = true)
            {
                assert(ch1 != '\0' && ch2 != '\0');

                if (skipWhitespace)
                    SkipWhitespace();

                if (*next_ != ch1 && *next_ != ch2)
                    return false;

                ++next_;
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

                next_ += (nextStr - str);

                return true;
            }

            bool ParseString(std::string& out)
            {
                assert(out.empty());

                if (!IsNext('\"'))
                    return false;

                const char* stringStart = next_;
#if 1 //TEMP
                while (*next_ != '\"') //TEMP
                    ++next_; //TEMP
#else //TEMP
                //TODO: handle unicode characters and control characters
#endif //TEMP

                const char* stringEnd = next_;
                out = std::string(stringStart, stringEnd - stringStart);

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
                        ++next_;
                }

                if (IsNext('.', false))
                {
                    while (std::isdigit(*next_))
                        ++next_;
                }

                if (IsNext('e', 'E', false))
                {
                    IsNext('+', '-', false);

                    while (std::isdigit(*next_))
                        ++next_;
                }

                const char* numberEnd = next_;
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
                    callbacks_.Value(str.c_str());
                    return true;
                }

                case 't':
                    if (!IsNext(c_true))
                        return false;
                    callbacks_.Value(c_true);
                    return true;

                case 'f':
                    if (!IsNext(c_false))
                        return false;
                    callbacks_.Value(c_false);
                    return true;

                case 'n':
                    if (!IsNext(c_null))
                        return false;
                    callbacks_.Value(c_null);
                    return true;

                default:
                {
                    std::string num;
                    if (!ParseNumber(num))
                        return false;
                    callbacks_.Value(num.c_str());
                    return true;
                }
                }
            }

            bool ParseArray()
            {
                if (!IsNext('['))
                    return false;

                if (!IsNext(']'))
                {
                    for (;;)
                    {
                        if (!ParseValue())
                            return false;

                        if (!IsNext(','))
                            break;
                    }
                }

                if (!IsNext(']'))
                    return false;

                return true;
            }

            bool ParseObject()
            {
                if (!IsNext('{'))
                    return false;

                if (!IsNext('}'))
                {
                    for (;;)
                    {
                        std::string name;
                        if (!ParseString(name))
                            return false;
                        callbacks_.Start(name.c_str());

                        if (!IsNext(':'))
                            return false;

                        if (!ParseValue())
                            return false;

                        callbacks_.End();

                        if (!IsNext(','))
                            break;
                    }
                }

                if (!IsNext('}'))
                    return false;

                return true;
            }

        private:
            const char* next_;
            Parser::Callbacks& callbacks_;
        };
    }

    bool Parser::Parse(const char* json, Callbacks& callbacks)
    {
        if (!json)
            return false;

        return detail::ParserImpl(json, callbacks).Parse();
    }
}
