#pragma once

#include <string>
#include <map>

namespace AdventLib
{
    class MimeResolver
    {
        public:
            static bool GetMimeType(const std::string& extension, std::string& mimeOut);

        private:
            static std::map<std::string, std::string> m_mimeTypes;

        private:
            MimeResolver() = default;
            MimeResolver(const MimeResolver&) = delete;
            MimeResolver& operator=(const MimeResolver&) = delete;
    };
}
