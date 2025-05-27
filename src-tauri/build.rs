fn main() {
  tauri_build::build();

  cxx_build::bridge("src/car_controller_bridge.rs")
        .file("../src-cpp/CarController.cpp")
        .file("../src-cpp/PortDevice.cpp")
        .file("../src-cpp/scheduler.cpp")
        .include("../src-tauri/includes")
        .flag_if_supported("-std=c++17")
        .compile("app_cxxbridge");

    println!("cargo:rerun-if-changed=src/car_controller_bridge.rs");
    println!("cargo:rerun-if-changed=src/port_device_bridge.rs");
    println!("cargo:rerun-if-changed=src/scheduler_bridge.rs");
    println!("cargo:rerun-if-changed=../src-cpp/CarController.cpp");
    println!("cargo:rerun-if-changed=../src-cpp/PortDevice.cpp");
    println!("cargo:rerun-if-changed=../src-cpp/scheduler.cpp");
    println!("cargo:rerun-if-changed=../src-tauri/includes/CarController.hpp");
    println!("cargo:rerun-if-changed=../src-tauri/includes/PortDevice.hpp");
    println!("cargo:rerun-if-changed=../src-tauri/includes/scheduler.hpp");
}
