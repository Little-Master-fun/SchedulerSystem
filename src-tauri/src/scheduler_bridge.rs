// Rust 端桥接 C++ Scheduler 并暴露 Tauri 命令

use tauri::{State, Manager};
use std::sync::Mutex;
use cxx::UniquePtr;

#[cxx::bridge]
mod ffi {
    extern "C++" {
        include!("Scheduler.hpp");
        type Scheduler;
        fn new_scheduler() -> UniquePtr<Scheduler>;
        fn get_assigned_tasks(self: &Scheduler) -> Vec<TaskDetail>;
        fn get_completed_tasks(self: &Scheduler) -> Vec<TaskDetail>;
        fn get_time(self: &Scheduler) -> f64;
    }

    #[derive(Debug, Clone, serde::Serialize)]
    pub struct TaskDetail {
        pub task_id: i32,
        pub material_id: String,
        pub type_: String,
        pub from_device: i32,
        pub to_device: i32,
        pub create_time: i64,
        pub start_time: i64,
        pub car_id: i32,
        pub pick_up_time: i64,
        pub drop_off_time: i64,
        pub taken_time: i64,
        pub status: i32,
        pub progress: f64,
    }
}

pub struct SchedulerState {
    pub scheduler: UniquePtr<ffi::Scheduler>,
}

#[tauri::command]
pub fn get_assigned_tasks(state: State<Mutex<SchedulerState>>) -> Vec<ffi::TaskDetail> {
    let state = state.lock().unwrap();
    state.scheduler.get_assigned_tasks()
}

#[tauri::command]
pub fn get_completed_tasks(state: State<Mutex<SchedulerState>>) -> Vec<ffi::TaskDetail> {
    let state = state.lock().unwrap();
    state.scheduler.get_completed_tasks()
}

pub fn init_scheduler(app: &tauri::AppHandle) {
    let scheduler = ffi::new_scheduler();
    app.manage(Mutex::new(SchedulerState { scheduler }));
}
