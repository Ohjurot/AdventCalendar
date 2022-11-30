#pragma once

#include <fmt/format.h>

#include <stdexcept>

#define ADVENT_ASSERT(expr, msg) \
    if(!(expr)) { throw Advent::Exception(msg); }
#define ADVENT_ASSERT_FMT(expr, msg, ...) \
    if(!(expr)) { throw Advent::Exception(fmt::format(msg, __VA_ARGS__)); }

namespace Advent
{
    class Exception : public std::exception
    {
        public:
            Exception(const std::string& str) : 
                m_message(str)
            {}

            const char* what() const noexcept override
            {
                return m_message.c_str();
            }

        private:
            std::string m_message;
    };
}
