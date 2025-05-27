// Rust 端桥接 C++ PortDevice 并暴露 Tauri 命令

use tauri::State;
use std::sync::Mutex;

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("PortDevice.hpp");
        type PortDevice;
        type PortStatus;
        type PortType;

        fn new_port_device(id: i32, port_type: i32, position: f64) -> UniquePtr<PortDevice>;
        fn getMaterialId(self: &PortDevice) -> Option<i32>;
        fn isBusy(self: &PortDevice) -> bool;
        fn isAvailable(self: &PortDevice) -> bool;
        fn getStatus(self: &PortDevice) -> i32; // PortStatus as i32

    }
}

struct PortDeviceState {
    device: cxx::UniquePtr<ffi::PortDevice>,
}

#[tauri::command]
pub fn get_port_status(state: State<Mutex<PortDeviceState>>) -> i32 {
    let state = state.lock().unwrap();
    state.device.getStatus()
}

#[tauri::command]
pub fn is_port_busy(state: State<Mutex<PortDeviceState>>) -> bool {
    let state = state.lock().unwrap();
    state.device.isBusy()
}

pub fn init_port_device(app: &tauri::AppHandle) {
    let device = ffi::new_port_device(1, 0, 0.0); // 示例参数
    app.manage(Mutex::new(PortDeviceState { device }));
}
