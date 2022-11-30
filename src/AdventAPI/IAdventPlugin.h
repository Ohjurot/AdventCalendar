#pragma once

#include "IAdventDay.h"

#include <cstdint>

#define ADVENTAPI_DISCOVERY_MAIN AdventCalendarMain
#define ADVENTAPI_DISCOVERY_MAIN_STR "?AdventCalendarMain@@YAXPEAPEAVIAdventPlugin@AdventAPI@@@Z"

#define ADVENTAPI_WINDOWS __declspec(dllexport)
#define ADVENTAPI_LINUX

namespace AdventAPI
{
    class IAdventPlugin;

    struct AdventDayInfo
    {
        int dayOfDecember = 0;
        int weight = 0;
        IAdventPlugin* owner = nullptr;
        IAdventDay* implementation = nullptr;
    };

    class IAdventPlugin
    {
        public:
            virtual ~IAdventPlugin() = default;

            virtual size_t RegisterAdventDays(AdventAPI::AdventDayInfo* infos, size_t infosMax) = 0;
            virtual void Destroy() = 0;
    };
}

typedef void(*AdventAPI_Discovery)(AdventAPI::IAdventPlugin** ppPlugin);
