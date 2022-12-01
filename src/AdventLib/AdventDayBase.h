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
            virtual std::string Render(nlohmann::json& data, spdlog::logger& logger) { return ""; }

            std::string Render(const std::filesystem::path& baseDir, const std::filesystem::path& commonTemplates, const nlohmann::json& jsonBase, spdlog::logger& logger) override
            {
                nlohmann::json data = jsonBase;
                std::string viewName = Render(data, logger);
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
                // Check file existing
                auto requestedFilePath = baseDir / "public" / requestedFile;
                auto fileStatus = std::filesystem::status(requestedFilePath);
                if (
                    fileStatus.type() == std::filesystem::file_type::regular || 
                    fileStatus.type() == std::filesystem::file_type::symlink
                    )
                {
                    // Get file extension
                    std::string fileExtension = requestedFilePath.extension().generic_string();
                    if (!fileExtension.empty())
                    {
                        fileExtension = fileExtension.substr(1);

                        // Resolve mime type 
                        if (MimeResolver::GetMimeType(fileExtension, mimeType))
                        {
                            std::ifstream fin;
                            fin.open(requestedFilePath, std::ios::binary | std::ios::in);
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
                            else
                            {
                                logger.warn("Can't open file {}", requestedFilePath.generic_string());
                            }
                        }
                        else
                        {
                            logger.debug("File extension {} could not be resolved to a mime type ({})",
                                fileExtension, requestedFilePath.generic_string());
                        }
                    }
                }

                return 0;
            }
    };
}
