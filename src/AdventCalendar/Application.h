#pragma once

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include <functional>

#define ADVENT_MAIN(cls) \
int main(int argc, const char** argv) \
{ \
    cls instance;\
    return instance(argc, argv);\
}

namespace Advent
{
    class Application
    {
        public:
            Application() = default;
            Application(const Application&) = delete;
            virtual ~Application() = default;

            Application& operator=(const Application&) = delete;

            virtual void SetupArguments(argparse::ArgumentParser& args) = 0;
            virtual int ApplicationMain(const argparse::ArgumentParser& args) = 0;
            virtual void SetupSpdlog() {};

            int Invoke(int argc, const char** argv) noexcept;

            // Operators
            inline int operator()(int argc, const char** argv) noexcept
            {
                return Invoke(argc, argv);
            }

        private:
            bool InvokeSave(const std::function<void(void)>& f);
    };
}
