use tauri::State;
use std::sync::Mutex;

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("CarController.hpp");
        type CarController;
        type CarStatus;
        type FullInfo;

        fn new_car_controller(id: i32) -> UniquePtr<CarController>;
        fn getAllInfo(self: &CarController) -> FullInfo;
        // 可根据需要添加更多方法
    }
    
    #[derive(Debug)]
    struct FullInfo {
        id: i32,
        position: f64,
        speed: f64,
        acceleration: f64,
        status: i32, // CarStatus 可用 i32 表示
        has_material: bool,
    }
}

struct CarControllerState {
    controller: cxx::UniquePtr<ffi::CarController>,
}

#[tauri::command]
pub fn get_car_info(state: State<Mutex<CarControllerState>>) -> ffi::FullInfo {
    let state = state.lock().unwrap();
    state.controller.getAllInfo()
}

pub fn init_car_controller(app: &tauri::AppHandle) {
    let controller = ffi::new_car_controller(1); // 这里用1为例
    app.manage(Mutex::new(CarControllerState { controller }));
}
