#pragma once

#include <spdlog/spdlog.h>

#include <string>
#include <filesystem>

namespace AdventAPI
{
    class IAdventDay
    {
        public:
            virtual ~IAdventDay() = default;

            virtual std::string Render(
                // Input
                const std::filesystem::path& baseDir,

                // Logger
                spdlog::logger& logger
            ) = 0;

            virtual bool Content(
                // Input (Plugins directory, Request)
                const std::filesystem::path& baseDir,
                const std::string& requestedFile,

                // Output
                std::string& dataOut,
                std::string& mimeType,

                // Logger
                spdlog::logger& logger
            ) = 0;
    };
}
