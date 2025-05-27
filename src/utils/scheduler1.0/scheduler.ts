import { invoke } from '@tauri-apps/api/tauri'
import { CarController } from './CarController'
import type { CarTask } from '@/types'
import { PortDevice } from './PortDevice'
import store from '@/store'

type Assignment = {
  car: CarController
  port: PortDevice
  from: number
  arriveTime: number
  finishTime: number
  path: [number, number]
}

export type TaskStatus = 'waiting' | 'in-progress' | 'done'

export interface TaskDetail {
  taskId: number
  materialId: string
  type: string
  fromDevice: number
  toDevice: number
  createTime: number
  startTime: number | null
  carId: number | null
  pickUpTime: number | null
  dropOffTime: number | null
  takenTime: number | null
  status: TaskStatus
  progress: number
}

export class Scheduler {
  private cars: CarController[]
  private taskQueue: CarTask[] = []
  public assignedTasks: Map<number, TaskDetail | number> = new Map()
  private completedTasks: Map<number, TaskDetail> = new Map()
  private trackLength: number
  private readonly carLength: number = 2
  private deviceMap: Map<number, PortDevice> = new Map()
  private virtualClock: number = 0
  private assignedTask: Map<number, number> = new Map()
  public isTasksOver: boolean = false
  private accelerationTime: number = 1

  constructor(cars: CarController[], deviceMap: Map<number, PortDevice>, trackLength: number) {
    this.cars = cars
    this.trackLength = trackLength
    this.deviceMap = deviceMap
  }

  public async setAccelerationTime(time: number) {
    return await invoke('scheduler_set_acceleration_time', { time })
  }
  public async getTime(): Promise<number> {
    return await invoke('scheduler_get_time')
  }
  public async getAccelerationTime(): Promise<number> {
    return await invoke('scheduler_get_acceleration_time')
  }
  public async addTask(task: CarTask) {
    return await invoke('scheduler_add_task', { task })
  }
  // 主调度入口
  public async update(deltaTime: number) {
    return await invoke('scheduler_update', { deltaTime })
  }
  public async getVirtualClock(): Promise<number> {
    return await invoke('scheduler_get_virtual_clock')
  }

  // 分配任务到设备
  public async assignTasksToDevices() {
    return await invoke('scheduler_assign_tasks_to_devices')
  }

  // 分配任务（采用全局最优方法）
  public async assignTasks() {
    return await invoke('scheduler_assign_tasks')
  }

  // 跟新任务进度
  public async updateTaskProgress(taskId: number, progress: number) {
    return await invoke('scheduler_update_task_progress', { taskId, progress })
  }
  // 小车取走物料
  public async pickUpCargo(taskId: number) {
    return await invoke('scheduler_pick_up_cargo', { taskId })
  }
  // 小车放下物料
  public async dropOffCargo(taskId: number) {
    return await invoke('scheduler_drop_off_cargo', { taskId })
  }
  // 小车取走物料
  public async takeCargo(taskId: number) {
    return await invoke('scheduler_take_cargo', { taskId })
  }
  // 获取任务进度
  public async getTaskProgress(taskId: number): Promise<TaskDetail | undefined> {
    return await invoke('scheduler_get_task_progress', { taskId })
  }

  // 完成任务
  public async completeTask(taskIdOrMaterialId: number | string, byMaterialId = false) {
    return await invoke('scheduler_complete_task', { taskIdOrMaterialId, byMaterialId })
  }

  // 获取任务
  public async getAssignedTasks(): Promise<TaskDetail[]> {
    return await invoke('get_assigned_tasks')
  }
  public async getCompletedTasks(): Promise<TaskDetail[]> {
    return await invoke('get_completed_tasks')
  }
  public async getCompletedTaskDetails(): Promise<TaskDetail[]> {
    return await invoke('scheduler_get_completed_task_details')
  }
  public async getTaskDetails() {
    return await invoke('scheduler_get_task_details')
  }
  public async checkIfAllTasksDone(): Promise<boolean> {
    return await invoke('scheduler_check_if_all_tasks_done')
  }
  public async getCarsSpeedTimeline() {
    return await invoke('scheduler_get_cars_speed_timeline')
  }
}
