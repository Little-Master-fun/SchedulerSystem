import { invoke } from '@tauri-apps/api/tauri'
import type { CarTask } from '@/types'
import type { PortDevice } from './PortDevice.ts'
import store from '@/store'

type CarStatus = 'idle' | 'moving' | 'loading' | 'loaded' | 'unloading' | 'waiting' | 'cruising'

export class CarController {
  public id: number
  public deviceMap: Map<number, PortDevice>
  constructor(id: number, deviceMap: Map<number, PortDevice>, initialPosition = 0) {
    this.id = id
    this.deviceMap = deviceMap
  }

  public async update(dt: number) {
    // 让 Rust 后端处理 update，返回最新状态
    const info = await invoke('car_update', { id: this.id, dt })
    return info
  }

  public async assignTask(task: CarTask, fromPos: number, toPos: number) {
    return await invoke('car_assign_task', { id: this.id, task, fromPos, toPos })
  }

  public async setTarget(pos: number) {
    return await invoke('car_set_target', { id: this.id, pos })
  }

  public async setTargetSpeed(v: number) {
    return await invoke('car_set_target_speed', { id: this.id, v })
  }

  public async tryResume() {
    return await invoke('car_try_resume', { id: this.id })
  }

  public async getDisplayProps() {
    return await invoke('car_get_display_props', { id: this.id })
  }

  public async getPosition() {
    return await invoke('car_get_position', { id: this.id })
  }

  public async getSpeed() {
    return await invoke('car_get_speed', { id: this.id })
  }

  public async getStatus() {
    return await invoke('car_get_status', { id: this.id })
  }

  public async getAllInfo() {
    return await invoke('get_car_info', { id: this.id })
  }

  public async getDistanceTo(car: CarController, trackLength: number) {
    return await invoke('car_get_distance_to', { id: this.id, otherId: car.id, trackLength })
  }

  public async getMaxStraightSpeed() {
    return await invoke('car_get_max_straight_speed', { id: this.id })
  }
}
