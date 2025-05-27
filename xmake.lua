set_project("autoCar2.0")
set_version("1.0.0")

set_languages("cxx17")

local CARGO_SOURCE_DIR = path.join(os.projectdir(), "src-tauri")
local CARGO_TARGET_DIR = path.join(CARGO_SOURCE_DIR, "target")
local CXXBRIDGE_DIR = path.join(CARGO_TARGET_DIR, "cxxbridge")
local CPP_SOURCE_DIR = path.join(os.projectdir(), "src-cpp")

os.mkdir(path.join(CXXBRIDGE_DIR, "rust"))
os.mkdir(path.join(CXXBRIDGE_DIR, "src"))

rule("gen_cxx_h")
    on_build(function (target)
        os.exec("cxxbridge --header > %s", path.join(CXXBRIDGE_DIR, "rust/cxx.h"))
    end)

rule("bridge_lib_rs")
    on_build(function (target)
        os.exec("cxxbridge %s/src/lib.rs > %s", CARGO_SOURCE_DIR, path.join(CXXBRIDGE_DIR, "src/lib.rs.cc"))
        os.exec("cxxbridge %s/src/lib.rs --header > %s", CARGO_SOURCE_DIR, path.join(CXXBRIDGE_DIR, "src/lib.rs.h"))
    end)

target("autocar_cpp")
    set_kind("static")
    add_files("src-cpp/*.cpp")
    add_includedirs("src-tauri/includes")
    add_headerfiles("src-cpp/*.h", "src-cpp/*.hpp", "src-tauri/includes/*.h", "src-tauri/includes/*.hpp")
    add_defines("AUTOCAR_CPP_EXPORTS", "AUTOCAR_CPP_STATIC", "AUTOCAR_CPP_STATIC_EXPORTS")
    add_files(path.join(CXXBRIDGE_DIR, "src/lib.rs.cc"))
    add_headerfiles(path.join(CXXBRIDGE_DIR, "rust/cxx.h"), path.join(CXXBRIDGE_DIR, "src/lib.rs.h"))
    add_rules("gen_cxx_h", "bridge_lib_rs")
    set_targetdir("lib")
    if is_plat("windows") then
        add_links("userenv", "ws2_32", "bcrypt")
    end

-- 测试代码
-- target("autoCar")
--     set_kind("binary")
--     add_files("src-cpp/*.cpp")
--     add_includedirs("src-tauri/includes")
--     set_targetdir("bin")