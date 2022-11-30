#include "Application.h"

int Advent::Application::Invoke(int argc, const char** argv) noexcept
{
    int returnCode = -1;

    // Setup arguments
    argparse::ArgumentParser args("AdventCalendar", "1.0");
    if (
        InvokeSave([&]() { this->SetupSpdlog(); }) &&
        InvokeSave([&]() { this->SetupArguments(args); }) &&
        InvokeSave([&]() { args.parse_args(argc, argv); }) &&
        InvokeSave([&]() { returnCode = this->ApplicationMain(args); })
        )
    {
        return returnCode;
    }

    return -1;
}

bool Advent::Application::InvokeSave(const std::function<void(void)>& f)
{
    try
    {
        f();
        return true;
    }
    catch (std::exception& ex)
    {
        spdlog::critical("Exception occured: {}", ex.what());
    }
    catch (...)
    {
        spdlog::critical("Unknown exception occurred!");
    }

    return false;
}
