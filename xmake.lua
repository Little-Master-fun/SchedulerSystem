set_project("autoCar2.0")
set_version("1.0.0")

set_languages("cxx17")

target("autocar_cpp")
    set_kind("static")
    add_files("src-cpp/*.cpp")
    add_includedirs("src-tauri/includes")
    set_targetdir("lib")
    add_headerfiles("src-cpp/*.h")
    add_headerfiles("src-cpp/*.hpp")
    add_headerfiles("src-tauri/includes/*.h")
    add_headerfiles("src-tauri/includes/*.hpp")
    add_defines("AUTOCAR_CPP_EXPORTS")
    add_defines("AUTOCAR_CPP_STATIC")
    add_defines("AUTOCAR_CPP_STATIC_EXPORTS")


-- target("autoCar")
--     set_kind("binary")
--     add_files("src-cpp/*.cpp")
--     add_includedirs("src-tauri/includes")
--     set_targetdir("bin")