#pragma once

#include <httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

#include <AdventCalendar/Application.h>
#include <AdventCalendar/Exceptions.h>
#include <AdventCalendar/Advent/AdventDayManger.h>

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <csignal>

#ifdef ADVENTCALENDAR_DEBUG
#define ADVENT_VERBOSE_LEVEL spdlog::level::trace
#else
#define ADVENT_VERBOSE_LEVEL spdlog::level::debug
#endif

namespace Advent
{
    class AdventApp : public Application
    {
        public:
            AdventApp() = default;
            AdventApp(const AdventApp&) = delete;
            ~AdventApp() override;

            AdventApp& operator=(const AdventApp&) = delete;

            void SetupArguments(argparse::ArgumentParser& args) override;
            int ApplicationMain(const argparse::ArgumentParser& args) override;
            void SetupSpdlog() override;

        private:
            void RequestAuthentication(const httplib::Request& request, httplib::Response& response);
            void RequestIndex(const httplib::Request& request, httplib::Response& response);
            void RequestDay(const httplib::Request& request, httplib::Response& response);
            void RequestDayContent(const httplib::Request& request, httplib::Response& response);

            int AssertDay(std::string daystr);
            int GetDecembersDay();
            bool IsAuthenticated(const httplib::Request& request);
            nlohmann::json GenerateDefaultJson(const httplib::Request& request);

            void RequestStop();
        
        private:
            std::filesystem::path m_serverDir;

            int m_overrideDay = -1;
            AdventDayManger m_days;
            httplib::SSLServer* m_server = nullptr;

            std::vector<std::string> m_accessTokens;

        // Signal handling
        private:    
            static void SignalHandler(int signal);
            static AdventApp* s_instance;
    };
}
