#include "AdventDayManger.h"

void Advent::AdventDayManger::AdventPlugin::Unload()
{
    if (pluginModule)
    {
        #ifdef ADVENTCALENDAR_WINDOWS
        FreeLibrary(pluginModule);
        #else
        dlclose(pluginModule);
        #endif
    }

    spdlog::info("Unloaded plugin {}", dllPath.generic_string());
}

bool Advent::AdventDayManger::AdventPlugin::Load()
{
    #ifdef ADVENTCALENDAR_WINDOWS
    pluginModule = LoadLibraryW(dllPath.wstring().c_str());
    if (pluginModule)
    {
        AdventAPI_Discovery discoverFuncion = 
            (AdventAPI_Discovery)GetProcAddress(pluginModule, ADVENTAPI_DISCOVERY_MAIN_STR);
        if (discoverFuncion)
        {
            discoverFuncion(&instance);
            if (instance == nullptr)
            {
                Unload();
            }
            return instance != nullptr;
        }
    }
    #else
    pluginModule = dlopen(dllPath.string().c_str(), RTLD_LAZY);
    if(pluginModule)
    {
        AdventAPI_Discovery discoverFuncion = 
            (AdventAPI_Discovery)dlsym(pluginModule, "_Z18AdventCalendarMainPPN9AdventAPI13IAdventPluginE");
        if (discoverFuncion)
        {
            discoverFuncion(&instance);
            if (instance == nullptr)
            {
                Unload();
            }
            return instance != nullptr;
        }
    } 
    #endif

    return false;
}

Advent::AdventDayManger::~AdventDayManger()
{
    for (auto& plugin : m_plugins)
    {
        if (plugin.second.instance)
        {
            plugin.second.instance->Destroy();
            plugin.second.Unload();
        }
    }
}

void Advent::AdventDayManger::Init(const std::filesystem::path& pluginDir)
{
    std::lock_guard janitor(m_mtx);

    m_pluginDir = pluginDir;
    LoadPlugins();
}

bool Advent::AdventDayManger::Invoke(int dayOfDecember, std::string& out)
{
    std::lock_guard janitor(m_mtx);
    
    auto* day = GetDay(dayOfDecember);
    if (day)
    {
        auto dir = m_plugins[day->owner].dllPath;
        dir.replace_extension(".d");
        out = day->implementation->Render(dir, *spdlog::default_logger());
        return true;
    }
    
    return false;
}

bool Advent::AdventDayManger::Content(int dayOfDecember, const std::string& file, bool& result, std::string& outData, std::string& outMime)
{
    std::lock_guard janitor(m_mtx);

    auto* day = GetDay(dayOfDecember);
    if (day)
    {
        auto baseDir = m_plugins[day->owner].dllPath;
        baseDir.replace_extension(".d");
        result = day->implementation->Content(baseDir, file, outData, outMime, *spdlog::default_logger());
        return true;
    }

    return false;
}

AdventAPI::AdventDayInfo* Advent::AdventDayManger::GetDay(int dayOfDecember)
{
    if (dayOfDecember >= 1 && dayOfDecember <= 31)
    {
        auto& day = m_days[dayOfDecember - 1];
        if (day.implementation)
        {
            return &day;
        }
        else
        {
            LoadPlugins();
            if (day.implementation)
            {
                return &day;
            }
        }
    }

    return nullptr;
}

void Advent::AdventDayManger::LoadPlugins()
{
    for (auto& file : std::filesystem::directory_iterator(m_pluginDir))
    {
        if (!file.is_directory() && file.exists())
        {
            if (
                file.path().extension() == ".so" ||
                file.path().extension() == ".dll"
                )
            {
                // Check if already loaded
                bool loaded = false;
                for (auto& plugin : m_plugins)
                {
                    if (plugin.second.dllPath == file.path())
                    {
                        loaded = true;
                        break;
                    }
                }
                if (loaded)
                    continue;

                // Load
                AdventPlugin plugin;
                plugin.dllPath = file.path();
                if (plugin.Load())
                {
                    spdlog::info("Loaded plugin {}", plugin.dllPath.generic_string());
                    AdventAPI::AdventDayInfo pendingDays[31];
                    size_t numPendingDays = plugin.instance->RegisterAdventDays(pendingDays, 31);
                    size_t loadedDays = 0;
                    for (size_t i = 0; i < numPendingDays; i++)
                    {
                        auto& requestedDay = m_days[pendingDays[i].dayOfDecember - 1];
                        if (requestedDay.implementation == nullptr || requestedDay.weight < pendingDays[i].weight)
                        {
                            if (requestedDay.implementation)
                            {
                                m_plugins[requestedDay.owner].refCount--;
                            }
                            plugin.refCount++;
                            requestedDay = pendingDays[i];
                            
                            loadedDays++;
                        }
                    }

                    // Store
                    if (loadedDays)
                    {
                        m_plugins[plugin.instance] = std::move(plugin);
                    }
                    else
                    {
                        plugin.Unload();
                    }

                    // Remove non ref plugins
                    CleanupPlugins();
                }
                else
                {
                    spdlog::warn("Failed loading plugin {}", plugin.dllPath.generic_string());
                }
            }
        } 
    }
}

void Advent::AdventDayManger::CleanupPlugins()
{
    std::vector<AdventAPI::IAdventPlugin*> toRemove;
    for (auto& plugin : m_plugins)
    {
        if (plugin.second.refCount == 0)
        {
            toRemove.push_back(plugin.first);
        }
    }

    for (auto* plugin : toRemove)
    {
        auto removeIt = m_plugins.find(plugin);
        if (removeIt != m_plugins.end())
        {
            if (removeIt->second.instance)
                removeIt->second.instance->Destroy();
            m_plugins.erase(removeIt);
        }
    }
}
