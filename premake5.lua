require("premake_modules/export-compile-commands")
require("premake_modules/cmake")

workspace "oxide"
    configurations { "Debug", "Release" }
    platforms { "x32", "x64" }

    location "premake"
    
    filter {"system:windows"}
        characterset "MBCS"
    filter {}

    -- Set up platforms 
    filter {"platforms:x32"}
        architecture "x32"
    filter {"platforms:x64"}
        architecture "x64"
    filter {}

    -- Setup c++ spec per platform
    -- Linux uses a buildoption to allow for more
    -- up to date standards (2a)
    filter {"system:windows"}
        toolset "msc-v141"
        cppdialect "C++17"
    filter {"system:linux"}
        toolset "clang" -- prefer clang over gcc
        buildoptions "-std=c++17"
    filter {}

    -- Setup configurations
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        optimize "Off"

        filter {"system:windows"}
            symbols "Full"
        filter {"system:linux"}
            symbols "On"
            buildoptions "-g3" -- need this for gdb
        filter {}
        runtime "Debug"

    filter {"configurations:Release"}
        defines { "NDEBUG" }
        optimize "Full"
        symbols "Off"
        flags {"LinkTimeOptimization"}
        
    filter {}
    
    project "oxide"
        kind "StaticLib"
        language "C++"
        targetdir "bin/%{cfg.buildcfg}"
        
        -- Windows and linux use different precompiled header stuff
        filter {"system:linux"}
            pchheader "src/precompiled.hh"
        filter {"system:windows"}
            pchheader "precompiled.hh"
        filter {}
        
        pchsource "src/precompiled.cc"

        filter{"files:src/include/*.c"}
            flags{"NoPCH"}
        filter{}

        filter{"configurations:Debug"}
            libdirs{"src/lib/Debug/"}
        filter{"configurations:Release"}
            libdirs{"src/lib/RelWithDebInfo"}
        filter{}

        filter{"system:windows"}
            links{"OpenGL32", "dwmapi"}
        filter{}

        links{"glfw3", "glew32s.lib"}
        
        includedirs { "src" }
        files { "src/**.hh", "src/**.cc", "src/**.h", "src/**.c" }

        -- For moving the compile commands into the root directory of the project
        -- so that autocomplete tools can see them (cquery...)
        
        -- This is messy but was the only way to get it to work consistently
        -- across multiple platforms (circleci, windows 10, vsts...)
        filter "system:linux"
            prebuildcommands {
                "{MKDIR} %{wks.location}/compile_commands/",
                "{TOUCH} %{wks.location}/compile_commands/%{cfg.shortname}.json",
                "{COPY} %{wks.location}/compile_commands/%{cfg.shortname}.json ../compile_commands.json"
            }
        filter "system:windows"
            prebuildcommands {
                "cmd.exe /c \"" .. "{MKDIR} %{wks.location}/compile_commands/",
                "cmd.exe /c \""  .. "{TOUCH} %{wks.location}/compile_commands/%{cfg.shortname}.json",
                "cmd.exe /c \""  .. "{COPY} %{wks.location}/compile_commands/%{cfg.shortname}.json ../compile_commands.json*"
            }
