#pragma once

#include <AdventAPI/IAdventPlugin.h>
#include <AdventAPI/IAdventDay.h>

#ifdef ADVENTCALENDAR_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <mutex>

namespace Advent
{
    class AdventDayManger
    {
        private:
            struct AdventPlugin 
            {
                #ifdef ADVENTCALENDAR_WINDOWS
                HMODULE pluginModule = nullptr;
                #else
                void* pluginModule = nullptr;
                #endif

                AdventAPI::IAdventPlugin* instance = nullptr;
                std::filesystem::path dllPath;
                size_t refCount = 0;

                void Unload();
                bool Load();
            };

        public:
            AdventDayManger() = default;
            AdventDayManger(const AdventDayManger&) = delete;
            ~AdventDayManger();

            AdventDayManger& operator=(const AdventDayManger&) = delete;

            void Init(const std::filesystem::path& pluginDir);
            bool Invoke(int dayOfDecember, const std::filesystem::path& commonTemplates, const nlohmann::json& baseJson, std::string& out);
            size_t Content(int dayOfDecember, const std::string& file, char** outData, std::string& outMime);

        private:
            AdventAPI::AdventDayInfo* GetDay(int dayOfDecember);

            void LoadPlugins();
            void CleanupPlugins();

        private:
            std::mutex m_mtx;

            std::filesystem::path m_pluginDir;

            // Currently operating days
            AdventAPI::AdventDayInfo m_days[31] = {};

            // Plug ins
            std::unordered_map<AdventAPI::IAdventPlugin*, AdventPlugin> m_plugins;
    };
}
