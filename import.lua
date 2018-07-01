project "oxide"
    kind "StaticLib"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    filter{"system:windows"}
        links{"OpenGL32", "dwmapi"}
    filter{"system:linux"}
        links{"X11", "GL", "Xext", "Xfixes"}
    filter{}

    includedirs { "src", "include" }
    files { "src/**.h*", "src/**.c*" }