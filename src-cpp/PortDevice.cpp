#include "PortDevice.hpp"
#include "Scheduler.hpp" // 如果 Scheduler 的定义放在别处，请确保包含正确路径
#include <iostream>

// 构造函数，初始化端口设备
PortDevice::PortDevice(int id, PortType type, double position)
    : id(id), type(type), position(position) {}

// 设置调度器指针
void PortDevice::setScheduler(Scheduler *sched)
{
    this->scheduler = sched;
}

// 添加任务到队列
void PortDevice::addTask(int materialId)
{
    taskQueue.push_back(materialId);
    // 如果当前没有任务且可用，则立即开始任务
    if (!currentMaterialId.has_value() && isAvailable())
    {
        currentMaterialId = taskQueue.front();
        taskQueue.erase(taskQueue.begin());
        hasCargo = true;
        status = PortStatus::Full;
    }
    // 这段逻辑其实不会被执行，因为和上面条件一样
    else if (!currentMaterialId.has_value() && isAvailable())
    {
        startNextTask();
    }
}

// 启动下一个任务
void PortDevice::startNextTask()
{
    if (taskQueue.empty() || !isAvailable())
        return;

    currentMaterialId = taskQueue.front();
    taskQueue.erase(taskQueue.begin());

    switch (type)
    {
    case PortType::Inlet:
        startOperation(PortStatus::Loading, 30); // 入口加载
        break;
    case PortType::OutInterface:
        startOperation(PortStatus::Loading, 50); // 出接口加载
        break;
    case PortType::InInterface:
        if (status == PortStatus::Idle)
            startOperation(PortStatus::Unloading, 25); // 入接口卸载
        break;
    case PortType::Outlet:
        if (status == PortStatus::Idle)
            startOperation(PortStatus::Unloading, 30); // 出口卸载
        break;
    }
}

// 更新设备状态（计时器递减）
void PortDevice::update(double deltaTime)
{
    if (timer > 0)
    {
        timer -= deltaTime;
        if (timer <= 0)
        {
            timer = 0;
            finishOperation();
        }
    }
}

// 完成当前操作
void PortDevice::finishOperation()
{
    // 加载完成，设置为满
    if ((type == PortType::OutInterface || type == PortType::Inlet) &&
        status == PortStatus::Loading)
    {
        hasCargo = true;
        status = PortStatus::Full;
    }

    // 卸载完成，设置为空闲，并通知调度器
    if ((type == PortType::Outlet || type == PortType::InInterface) &&
        status == PortStatus::Unloading)
    {
        hasCargo = false;
        status = PortStatus::Idle;
        if (scheduler && currentMaterialId.has_value())
        {
            scheduler->completeTask(currentMaterialId.value(), true); // 通知调度器任务完成
        }
        currentMaterialId.reset();
        startNextTask();
    }
}

// 物料被取走
void PortDevice::onMaterialTaken()
{
    if (hasCargo && (type == PortType::OutInterface || type == PortType::Inlet))
    {
        hasCargo = false;
        status = PortStatus::Idle;
        currentMaterialId.reset();
        startNextTask();
    }
}

// 物料被放置
void PortDevice::onMaterialPlaced(int materialId)
{
    if (!hasCargo && (type == PortType::Outlet || type == PortType::InInterface))
    {
        hasCargo = true;
        status = PortStatus::Full;
        currentMaterialId = materialId;

        if (type == PortType::InInterface)
        {
            startOperation(PortStatus::Unloading, 25); // 入接口卸载
        }
        else if (type == PortType::Outlet)
        {
            startOperation(PortStatus::Unloading, 30); // 出口卸载
        }
    }
}

// 启动操作，设置状态和计时器
void PortDevice::startOperation(PortStatus newStatus, double duration)
{
    if (timer > 0)
        return;
    status = newStatus;
    timer = duration;
}

// 判断设备是否忙碌
bool PortDevice::isBusy() const
{
    return timer > 0;
}

// 判断设备是否可用
bool PortDevice::isAvailable() const
{
    return !isBusy() && (status == PortStatus::Idle || status == PortStatus::Empty);
}

// 获取当前物料ID
std::optional<int> PortDevice::getMaterialId() const
{
    return currentMaterialId;
}

// 设备位置信息表
const std::map<int, double> devicePositions = {
    {1, 13.54},
    {2, 15.94},
    {3, 19.54},
    {4, 21.93},
    {5, 25.54},
    {6, 27.93},
    {7, 31.53},
    {8, 33.93},
    {9, 37.54},
    {10, 39.93},
    {11, 43.54},
    {12, 45.93},
    {13, 67.47},
    {14, 70.47},
    {15, 73.47},
    {16, 85.47},
    {17, 88.47},
    {18, 91.47},
};

// 端口类型映射表
const std::map<int, PortType> portTypes = {
    {1, PortType::InInterface},
    {2, PortType::OutInterface},
    {3, PortType::InInterface},
    {4, PortType::OutInterface},
    {5, PortType::InInterface},
    {6, PortType::OutInterface},
    {7, PortType::InInterface},
    {8, PortType::OutInterface},
    {9, PortType::InInterface},
    {10, PortType::OutInterface},
    {11, PortType::InInterface},
    {12, PortType::OutInterface},
    {13, PortType::Outlet},
    {14, PortType::Outlet},
    {15, PortType::Outlet},
    {16, PortType::Inlet},
    {17, PortType::Inlet},
    {18, PortType::Inlet},
};

// 获取所有设备对象
std::vector<std::shared_ptr<PortDevice>> getAllDevices()
{
    std::vector<std::shared_ptr<PortDevice>> devices;
    for (const auto &entry : devicePositions)
    {
        int id = entry.first;
        double pos = entry.second;
        auto typeIt = portTypes.find(id);
        if (typeIt != portTypes.end())
        {
            devices.push_back(std::make_shared<PortDevice>(id, typeIt->second, pos));
        }
    }
    return devices;
}

// 获取设备ID到设备对象的映射表
std::map<int, std::shared_ptr<PortDevice>> getDeviceMap()
{
    std::map<int, std::shared_ptr<PortDevice>> map;
    for (const auto &device : getAllDevices())
    {
        map[device->id] = device;
    }
    return map;
}

// 根据ID获取设备状态
std::optional<PortStatus> getDeviceStatusById(int id)
{
    auto map = getDeviceMap();
    auto it = map.find(id);
    if (it != map.end())
    {
        return it->second->status;
    }
    return std::nullopt;
}
