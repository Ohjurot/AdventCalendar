#pragma once

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <string>
#include <memory>
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
                const std::filesystem::path& commonTemplates,
                const nlohmann::json& jsonBase,

                // Logger
                spdlog::logger& logger
            ) = 0;

            virtual size_t Content(
                // Input (Plugins directory, Request)
                const std::filesystem::path& baseDir,
                const std::string& requestedFile,

                // Output
                char** dataOut,
                std::string& mimeType,

                // Logger
                spdlog::logger& logger
            ) = 0;
    };
}
