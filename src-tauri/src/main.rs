// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod car_controller_bridge;
mod port_device_bridge;
mod scheduler_bridge;
use std::sync::Mutex;
use tauri::Manager;

fn main() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![
            car_controller_bridge::get_car_info,
            port_device_bridge::get_port_status,
            port_device_bridge::is_port_busy,
            scheduler_bridge::get_assigned_tasks,
            scheduler_bridge::get_completed_tasks,
        ])
        .setup(|app| {
            car_controller_bridge::init_car_controller(app);
            port_device_bridge::init_port_device(app);
            scheduler_bridge::init_scheduler(app);
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
