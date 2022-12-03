#pragma once

#include "AdventAPI/IAdventDay.h"
#include "MimeResolver.h"

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

#include <fstream>

namespace AdventLib
{
    class AdventDayBase : public ::AdventAPI::IAdventDay
    {
        public:
            AdventDayBase(int day = -1) : m_day(day) {};

            virtual std::string Render(nlohmann::json& data, const std::filesystem::path& privateDir, spdlog::logger& logger) { return ""; }

            std::string Render(const std::filesystem::path& baseDir, const std::filesystem::path& commonTemplates, const nlohmann::json& jsonBase, spdlog::logger& logger) override
            {
                nlohmann::json data = jsonBase;
                std::string viewName = Render(data, baseDir / "private", logger);
                if (!viewName.empty())
                {
                    auto filename = baseDir / "private" / viewName;
                    filename.replace_extension(".jinja");

                    inja::Environment env;
                    env.include_template("base", env.parse_template((commonTemplates / "base.jinja").generic_string()));
                    return env.render_file(filename.generic_string(), data);
                }

                return "";
            }

            size_t Content(const std::filesystem::path& baseDir, const std::string& requestedFile, char** dataOut, std::string& mimeType, spdlog::logger& logger) override
            {
                // Check if a day is requested
                std::filesystem::path tPath = requestedFile;
                auto itPFirst = tPath.begin();
                if (itPFirst != tPath.end())
                {
                    unsigned int day = 0;
                    std::stringstream ss;
                    ss << itPFirst->generic_string();
                    ss >> day;

                    if (day != 0)
                    {
                        return 0;
                    }
                }

                // Resolve the mime type
                std::string fileExtension = std::filesystem::path(requestedFile).extension().generic_string();
                if (!fileExtension.empty())
                {
                    fileExtension = fileExtension.substr(1);

                    // Resolve mime type 
                    if (MimeResolver::GetMimeType(fileExtension, mimeType))
                    {
                        std::stringstream ss;
                        ss << m_day;

                        // Find file in days directory
                        size_t size;
                        size = ReadFile(baseDir / "public" / ss.str() / requestedFile, dataOut);

                        // If not found try public dir
                        if(size == 0)
                            size = ReadFile(baseDir / "public" / requestedFile, dataOut);

                        // Find warn
                        if(size == 0)
                            logger.warn("Can't open file {} for day {}", requestedFile, m_day);

                        return size;
                    }
                    else
                    {
                        logger.debug("File extension {} could not be resolved to a mime type ({} for day {})",
                            fileExtension, requestedFile, m_day);
                    }
                }

              
                return 0;
            }

        private:
            size_t ReadFile(const std::filesystem::path& fileRequested, char** dataOut)
            {
                // Check file existing
                auto fileStatus = std::filesystem::status(fileRequested);
                if (
                    fileStatus.type() == std::filesystem::file_type::regular ||
                    fileStatus.type() == std::filesystem::file_type::symlink
                    )
                {
                    std::ifstream fin;
                    fin.open(fileRequested, std::ios::binary | std::ios::in);
                    if (fin.is_open())
                    {
                        // Get size
                        fin.seekg(0, std::ios::end);
                        auto fsize = fin.tellg();
                        fin.seekg(0, std::ios::beg);

                        if (fsize)
                        {
                            *dataOut = new char[fsize];
                            if (dataOut)
                            {
                                fin.read(*dataOut, fsize);
                                return fsize;
                            }
                        }
                    }
                }

                return 0;
            }

        private:
            int m_day = -1;
    };
}
