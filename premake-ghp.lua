require "premake_modules/ghp"

project "oxide"
    kind "StaticLib"

    filter{"system:windows"}
        links{"OpenGL32", "dwmapi"}
    filter{}

    includedirs {"src", "include"}
    files { "src/**.hh", "src/**.cc", "src/**.h", "src/**.c" }

    defines { '_CRT_SECURE_NO_WARNINGS' }

    libdirs{"src/lib/RelWithDebInfo"}
    links{"glfw3", "glew32s"}

ghp.export_includedirs "include"
ghp.export_project("oxide" "oxide")
