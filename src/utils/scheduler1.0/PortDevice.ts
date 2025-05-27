import { invoke } from '@tauri-apps/api/tauri'
import store from '@/store'

export type PortType = 'inlet' | 'outlet' | 'in-interface' | 'out-interface'
export type PortStatus = 'idle' | 'waiting' | 'loading' | 'unloading' | 'full' | 'empty'

export class PortDevice {
  public id: number
  public type: PortType
  public status: PortStatus = 'idle'
  public hasCargo = false
  public timer = 0
  public position: number
  private taskQueue: number[] = []
  public currentMaterialId: number | null = null
  public scheduler: any = null

  constructor(id: number, type: PortType, position: number) {
    this.id = id
    this.type = type
    this.position = position
  }

  public setScheduler(scheduler: any) {
    this.scheduler = scheduler
  }

  // 添加任务（物料ID）
  public async addTask(materialId: number) {
    return await invoke('port_add_task', { id: this.id, materialId })
  }

  // 更新设备状态
  public async update(deltaTime: number) {
    return await invoke('port_update', { id: this.id, deltaTime })
  }

  // 小车取走物料时调用
  public async onMaterialTaken() {
    return await invoke('port_on_material_taken', { id: this.id })
  }

  // 小车放上物料时调用
  public async onMaterialPlaced(materialId: number) {
    return await invoke('port_on_material_placed', { id: this.id, materialId })
  }

  public async startOperation(status: PortStatus, duration: number) {
    return await invoke('port_start_operation', { id: this.id, status, duration })
  }

  public async isBusy(): Promise<boolean> {
    return await invoke('port_is_busy', { id: this.id })
  }
  public async getMaterialId(): Promise<number | null> {
    return await invoke('port_get_material_id', { id: this.id })
  }
  public async isAvailable(): Promise<boolean> {
    return await invoke('port_is_available', { id: this.id })
  }
}

// 设备位置信息
const devicePositions: Record<number, number> = {
  1: 13.54,
  2: 15.94,
  3: 19.54,
  4: 21.93,
  5: 25.54,
  6: 27.93,
  7: 31.53,
  8: 33.93,
  9: 37.54,
  10: 39.93,
  11: 43.54,
  12: 45.93,
  13: 67.47,
  14: 70.47,
  15: 73.47,
  16: 85.47,
  17: 88.47,
  18: 91.47,
}

// 类型映射表
const portTypes: Record<number, PortType> = {
  1: 'in-interface',
  2: 'out-interface',
  3: 'in-interface',
  4: 'out-interface',
  5: 'in-interface',
  6: 'out-interface',
  7: 'in-interface',
  8: 'out-interface',
  9: 'in-interface',
  10: 'out-interface',
  11: 'in-interface',
  12: 'out-interface',
  13: 'outlet',
  14: 'outlet',
  15: 'outlet',
  16: 'inlet',
  17: 'inlet',
  18: 'inlet',
}

// 创建设备列表
export function getAllDevices(): PortDevice[] {
  return Object.keys(devicePositions).map((idStr) => {
    const id = parseInt(idStr)
    return new PortDevice(id, portTypes[id], devicePositions[id])
  })
}

// 创建设备 Map
export function getDeviceMap(): Map<number, PortDevice> {
  const map = new Map<number, PortDevice>()
  getAllDevices().forEach((dev) => map.set(dev.id, dev))
  return map
}

// 通过设备id查找设备状态
export function getDeviceStatusById(id: number): PortStatus | null {
  const map = getDeviceMap()
  const device = map.get(id)
  return device ? device.status : null
}
