#pragma once

#include <AdventAPI/IAdventPlugin.h>
#include <AdventAPI/IAdventDay.h>

#ifdef ADVENTCALENDAR_WINDOWS
#include <Windows.h>
#endif

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
                HMODULE pluginModule;
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
            bool Invoke(int dayOfDecember, std::string& out);
            bool Content(int dayOfDecember, const std::string& file, bool& result, std::string& outData, std::string& outMime);

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
