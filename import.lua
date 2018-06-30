project "oxide"
    kind "StaticLib"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    libdirs{"src/lib/RelWithDebInfo"}

    filter{"system:windows"}
        links{"OpenGL32", "dwmapi"}
    filter{}

    links{"glfw3", "glew32s.lib"}

    includedirs { "src", "include" }
    files { "src/**.hh", "src/**.cc", "src/**.h", "src/**.c" }