#include "AdventAPI/IAdventPlugin.h"
#include "AdventAPI/IAdventDay.h"

#include "AdventLib/AdventDayBase.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <mutex>

#ifdef EXAMPLEPLUGIN_WINDOWS
#define EXAMPLEPLUGIN_API ADVENTAPI_WINDOWS
#else
#define EXAMPLEPLUGIN_API ADVENTAPI_LINUX
#endif

class ImageDay : public AdventLib::AdventDayBase
{
    public:
        ImageDay(int day) :
            AdventLib::AdventDayBase(day),
            m_day(day)
        {
        }

        std::string Render(nlohmann::json& data, const std::filesystem::path& privateDir, spdlog::logger& logger) override
        {
            std::lock_guard janitor(m_lock);

            if (m_caption.empty())
            {
                std::stringstream ss;
                ss << m_day;

                std::ifstream ifs(privateDir / "captions.json", std::ios::in);
                nlohmann::json jf = nlohmann::json::parse(ifs);
                if (jf.find(ss.str()) != jf.end())
                {
                    m_caption = jf[ss.str()];
                }
            }

            std::stringstream ss;
            ss << "content/" << m_day << "/image.jpg";

            data["caption"] = m_caption;
            data["image"] = ss.str();
            return "default";
        }

    private:
        std::mutex m_lock;

        int m_day;
        std::string m_caption;
};

class ExamplePlugin : public AdventAPI::IAdventPlugin
{
    public:
        ExamplePlugin(int days = 24)
        {
            for (int i = 0; i < days; i++)
            {
                m_days.push_back(new ImageDay(i + 1));
            }
        }
        
        ~ExamplePlugin()
        {
            for (auto* day : m_days)
                delete day;
        }

        size_t RegisterAdventDays(AdventAPI::AdventDayInfo* infos, size_t infosMax) override
        {
            if (infosMax < m_days.size())
                return 0;

            int d = 1;
            int i = 0;
            for(auto* day : m_days)
                infos[i++] = { d++, 0, this, day };
            return m_days.size();
        }

        void Destroy() override
        {
            delete this;
        }
    
    private:
        std::vector<ImageDay*> m_days;
};

void EXAMPLEPLUGIN_API ADVENTAPI_DISCOVERY_MAIN(AdventAPI::IAdventPlugin** ppPlugin)
{
    *ppPlugin = new ExamplePlugin();
}

