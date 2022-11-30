#include "AdventAPI/IAdventPlugin.h"
#include "AdventAPI/IAdventDay.h"

#include <sstream>

#ifdef EXAMPLEPLUGIN_WINDOWS
#define EXAMPLEPLUGIN_API ADVENTAPI_WINDOWS
#else
#define EXAMPLEPLUGIN_API ADVENTAPI_LINUX
#endif

class HelloDay : public AdventAPI::IAdventDay
{
    public:
        HelloDay(int day) :
            m_day(day)
        {}

        std::string Render(spdlog::logger& logger) override
        {
            std::stringstream ss;
            ss << "What a day " << m_day << " of December!";
            return ss.str();
        }

        bool Content(const std::filesystem::path& baseDir, const std::string& requestedFile, std::string& dataOut, std::string& mimeType, spdlog::logger& logger) override
        {
            auto path = baseDir / requestedFile;
            logger.info("Requested file {}", path.generic_string());
            return false;
        }

    private:
        int m_day;
};

class ExamplePlugin : public AdventAPI::IAdventPlugin
{
    public:
        size_t RegisterAdventDays(AdventAPI::AdventDayInfo* infos, size_t infosMax) override
        {
            infos[0] = { 1, 0, this, &d1 };
            infos[1] = { 5, 0, this, &d5 };
            infos[2] = { 10, 0, this, &d10 };
            infos[3] = { 20, 0, this, &d20 };
            return 4;
        }

        void Destroy() override
        {
            delete this;
        }
    
    private:
        HelloDay d1{ 1 };
        HelloDay d5{ 5 };
        HelloDay d10{ 10 };
        HelloDay d20{ 20 };
};

void EXAMPLEPLUGIN_API ADVENTAPI_DISCOVERY_MAIN(AdventAPI::IAdventPlugin** ppPlugin)
{
    *ppPlugin = new ExamplePlugin();
}

