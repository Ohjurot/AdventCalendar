include "conanbuildinfo.premake.lua"
include "scripts/premake_utils.lua"

workspace "AdventCalendar" 
    -- Static setup
    advent_conan_basic_setup()
    linkgroups "On" -- Disable order dependent linking

    -- Include all projects
    include "src/AdventCalendar/build.lua"     
    include "src/AdventAPI/build.lua"
    include "src/AdventLib/build.lua"     
    include "src/Plugins/ExamplePlugin/build.lua"     
