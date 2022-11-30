-- Conan no link setup (because visual studio will throw plenty of warnings at you)
function advent_conan_basic_setup()
    configurations{conan_build_type}
    architecture(conan_arch)
    includedirs{conan_includedirs}
    libdirs{conan_libdirs}
    defines{conan_defines}
    bindirs{conan_bindirs}
end

function advent_output_links()
    -- Conan links
    links{conan_libs}
    links{conan_system_libs}
    links{conan_frameworks}

    -- System lib links
    filter { "system:Linux" }
        links { "stdc++", "uuid" }
    filter {}
end

-- Create a new project
function advent_new_project(projectName, path)
    -- Common setup
    project(projectName)
    location("%{wks.location}/" .. path)
    targetdir("%{wks.location}/build/%{cfg.architecture}-%{cfg.shortname}/bin/")
    objdir("%{wks.location}/build/%{cfg.architecture}-%{cfg.shortname}/obj/%{cfg.buildtarget.name}/")
    libdirs {"%{cfg.buildtarget.directory}"}
    includedirs {"%{prj.location}", "%{wks.location}/src"}
    files { 
        "%{prj.location}/**.lua",
        "%{prj.location}/**.h", "%{prj.location}/**.hpp", "%{prj.location}/**.hh", "%{prj.location}/**.hxx",
        "%{prj.location}/**.c", "%{prj.location}/**.cpp", "%{prj.location}/**.cc", "%{prj.location}/**.cxx", 
        "%{prj.location}/**.md", "%{prj.location}/**.txt",
    }

    -- Cpp
    language "C++"
    cppdialect "C++20"

    -- Defines
    defines { string.upper(projectName) .. "_BUILD" }

    -- Windows / Linux
    filter { "system:Windows" }
        defines {  string.upper(projectName) .. "_WINDOWS", "NOMINMAX" }
    filter {}
    filter { "system:Linux" }
        defines {  string.upper(projectName) .. "_LINUX" }
    filter {}

    -- Debug / Release
    filter "configurations:Debug"
        defines {  "DEBUG", string.upper(projectName) .. "_DEBUG" }
        symbols "On"
    filter {}
    filter "configurations:Release"
        defines {  "NDEBUG", string.upper(projectName) .. "_RELEASE" }
        optimize "On"
    filter {}
end

-- Mark current project as executable
function advent_exe()
    kind "ConsoleApp"
    advent_output_links()
end

-- Mark current project as dll
function advent_dll()
    kind "SharedLib"
    advent_output_links()
end

-- Mark current project as lib
function advent_lib()
    kind "StaticLib"
end

-- Mark current project as lib
function advent_headers()
    kind "None"
end

