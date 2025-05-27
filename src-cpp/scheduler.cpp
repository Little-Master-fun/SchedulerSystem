// Scheduler.cpp
#include "Scheduler.hpp"
#include <algorithm>
#include <iostream>

// 构造函数，初始化成员变量
Scheduler::Scheduler(std::vector<std::shared_ptr<CarController>> cars,
                     std::map<int, std::shared_ptr<PortDevice>> deviceMap,
                     double trackLength)
    : cars(std::move(cars)),
      deviceMap(std::move(deviceMap)),
      trackLength(trackLength),
      virtualClock(0),
      accelerationTime(1.0),
      isTasksOver(false)
{
    // 构造函数，成员初始化列表
}

// 更新调度器状态，推进虚拟时钟
void Scheduler::update(double deltaTime)
{
    if (isTasksOver)
        return;
    virtualClock += deltaTime;

    // 更新所有任务进度
    for (auto &[taskId, taskDetail] : assignedTasks)
    {
        if (taskDetail.status == TaskStatus::InProgress)
        {
            // 简单示例，任务进度随时间增加，这里按比例计算
            taskDetail.progress += deltaTime / 100.0;
            if (taskDetail.progress >= 1.0)
            {
                taskDetail.progress = 1.0;
                taskDetail.status = TaskStatus::Done;
                // 从assigned移到completed
                completedTasks[taskId] = taskDetail;
            }
        }
    }

    // 检查是否所有任务完成
    isTasksOver = checkIfAllTasksDone();
}

// 设置加速时间
void Scheduler::setAccelerationTime(double time)
{
    accelerationTime = time;
}

// 获取当前虚拟时钟
double Scheduler::getTime() const
{
    return virtualClock;
}

// 获取加速时间
double Scheduler::getAccelerationTime() const
{
    return accelerationTime;
}

// 添加任务到任务队列
void Scheduler::addTask(const CarTask &task)
{
    // 加入任务队列
    taskQueue.push_back(task);
}

// 分配任务给设备和车辆
void Scheduler::assignTasksToDevices()
{
    // 简单示例：按顺序将任务分配给设备和车辆（示例逻辑，实际可更复杂）
    for (const auto &task : taskQueue)
    {
        if (assignedTasks.find(task.taskId) != assignedTasks.end())
            continue; // 已分配跳过

        // 找空闲车辆
        for (const auto &car : cars)
        {
            bool busy = false;
            for (const auto &[id, taskDetail] : assignedTasks)
            {
                if (taskDetail.carId && taskDetail.carId.value() == car->getId())
                {
                    busy = true;
                    break;
                }
            }
            if (busy)
                continue;

            // 找目标设备
            auto itFrom = deviceMap.find(task.fromDevice);
            auto itTo = deviceMap.find(task.toDevice);
            if (itFrom == deviceMap.end() || itTo == deviceMap.end())
                continue;

            // 创建任务详细信息
            TaskDetail detail{};
            detail.taskId = task.taskId;
            detail.materialId = task.materialId;
            detail.fromDevice = task.fromDevice;
            detail.toDevice = task.toDevice;
            detail.createTime = virtualClock;
            detail.startTime = virtualClock;
            detail.carId = car->getId();
            detail.status = TaskStatus::InProgress;
            detail.progress = 0;

            assignedTasks[task.taskId] = detail;

            break; // 一辆车一个任务，跳出车循环
        }
    }
}

// 分配任务（可扩展更复杂的分配逻辑）
void Scheduler::assignTasks()
{
    // 这里可以调用assignTasksToDevices 或者实现更复杂的分配逻辑
    assignTasksToDevices();
}

// 更新指定任务的进度
void Scheduler::updateTaskProgress(int taskId, double progress)
{
    auto it = assignedTasks.find(taskId);
    if (it != assignedTasks.end())
    {
        it->second.progress = progress;
        if (progress >= 1.0)
        {
            it->second.progress = 1.0;
            it->second.status = TaskStatus::Done;
            completedTasks[taskId] = it->second;
            assignedTasks.erase(it);
        }
    }
}

// 记录任务提货时间
void Scheduler::pickUpCargo(int taskId)
{
    auto it = assignedTasks.find(taskId);
    if (it != assignedTasks.end())
    {
        it->second.pickUpTime = virtualClock;
    }
}

// 记录任务卸货时间
void Scheduler::dropOffCargo(int taskId)
{
    auto it = assignedTasks.find(taskId);
    if (it != assignedTasks.end())
    {
        it->second.dropOffTime = virtualClock;
    }
}

// 记录任务取货时间
void Scheduler::takeCargo(int taskId)
{
    auto it = assignedTasks.find(taskId);
    if (it != assignedTasks.end())
    {
        it->second.takenTime = virtualClock;
    }
}

// 获取任务进度（可选返回）
std::optional<TaskDetail> Scheduler::getTaskProgress(int taskId) const
{
    auto it = assignedTasks.find(taskId);
    if (it != assignedTasks.end())
    {
        return it->second;
    }
    auto itDone = completedTasks.find(taskId);
    if (itDone != completedTasks.end())
    {
        return itDone->second;
    }
    return std::nullopt;
}

// 完成任务（可按任务ID或物料ID）
void Scheduler::completeTask(int taskIdOrMaterialId, bool byMaterialId)
{
    if (byMaterialId)
    {
        // 按materialId查找并标记完成
        for (auto it = assignedTasks.begin(); it != assignedTasks.end();)
        {
            if (it->second.materialId == std::to_string(taskIdOrMaterialId)) // 这里是简单示例，类型需要调整
            {
                it->second.status = TaskStatus::Done;
                completedTasks[it->first] = it->second;
                it = assignedTasks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    else
    {
        auto it = assignedTasks.find(taskIdOrMaterialId);
        if (it != assignedTasks.end())
        {
            it->second.status = TaskStatus::Done;
            completedTasks[it->first] = it->second;
            assignedTasks.erase(it);
        }
    }
}

// 获取所有已分配任务
std::vector<TaskDetail> Scheduler::getAssignedTasks() const
{
    std::vector<TaskDetail> tasks;
    for (const auto &[id, detail] : assignedTasks)
    {
        tasks.push_back(detail);
    }
    return tasks;
}

// 获取所有已完成任务
std::vector<TaskDetail> Scheduler::getCompletedTasks() const
{
    std::vector<TaskDetail> tasks;
    for (const auto &[id, detail] : completedTasks)
    {
        tasks.push_back(detail);
    }
    return tasks;
}

// 获取所有已完成任务详情
std::vector<TaskDetail> Scheduler::getCompletedTaskDetails() const
{
    // 这里简单返回已完成任务，和getCompletedTasks相同
    return getCompletedTasks();
}

// 获取所有车辆速度时间线（示例返回空）
std::vector<std::map<std::string, std::string>> Scheduler::getCarsSpeedTimeline() const
{
    // 这是示例，返回空vector，实际需要从car对象获取速度时间线
    return {};
}

// 获取所有任务详情（字符串映射）
std::vector<std::map<std::string, std::string>> Scheduler::getTaskDetails() const
{
    // 示例，返回所有任务的简单字符串映射
    std::vector<std::map<std::string, std::string>> details;
    for (const auto &[id, task] : assignedTasks)
    {
        std::map<std::string, std::string> d;
        d["taskId"] = std::to_string(task.taskId);
        d["materialId"] = task.materialId;
        d["status"] = (task.status == TaskStatus::Done) ? "Done" : "InProgress";
        details.push_back(d);
    }
    for (const auto &[id, task] : completedTasks)
    {
        std::map<std::string, std::string> d;
        d["taskId"] = std::to_string(task.taskId);
        d["materialId"] = task.materialId;
        d["status"] = "Done";
        details.push_back(d);
    }
    return details;
}

// 检查所有任务是否完成
bool Scheduler::checkIfAllTasksDone()
{
    return assignedTasks.empty() && taskQueue.empty();
}

// 获取虚拟时钟
double Scheduler::getVirtualClock() const
{
    return virtualClock;
}

// 判断两条路径是否冲突
bool Scheduler::isPathConflict(const Assignment &a, const Assignment &b) const
{
    // 简单检测两条路径是否有冲突，示例逻辑
    if (a.finishTime < b.arriveTime || b.finishTime < a.arriveTime)
        return false; // 时间不重叠

    // 路径区间重叠
    if (a.path.second < b.path.first || b.path.second < a.path.first)
        return false; // 空间不重叠

    return true;
}

// 判断pos是否在环形轨道[from, to]区间内
bool Scheduler::isInCircularPath(double from, double to, double pos) const
{
    // 判断 pos 是否在环形轨道的区间[from, to]
    if (from <= to)
        return pos >= from && pos <= to;
    else
        return pos >= from || pos <= to;
}

// 判断所有Assignment是否有冲突
bool Scheduler::hasConflict(const std::vector<Assignment> &assignments) const
{
    // 判断所有Assignment是否有冲突
    for (size_t i = 0; i < assignments.size(); ++i)
    {
        for (size_t j = i + 1; j < assignments.size(); ++j)
        {
            if (isPathConflict(assignments[i], assignments[j]))
                return true;
        }
    }
    return false;
}

// 生成所有分配组合（示例返回空）
std::vector<std::vector<Assignment>> Scheduler::generateAllAssignmentCombinations(
    const std::vector<std::shared_ptr<CarController>> &cars,
    const std::vector<std::shared_ptr<PortDevice>> &ports,
    double trackLength,
    double pickTime) const
{
    // 这个函数比较复杂，示例简单返回空
    return {};
}

// 预防车辆碰撞（示例为空）
void Scheduler::preventCollision()
{
    // 预防车辆碰撞的逻辑，示例为空
}

// 计算车辆到目标位置所需时间
double Scheduler::calculateTimeToTarget(const CarController &car, double targetPos) const
{
    // 简单示例，根据车辆当前位置和目标位置计算时间，考虑加速度时间
    double distance = targetPos - car.getPosition();
    if (distance < 0)
        distance += trackLength; // 环形轨道

    double speed = car.getSpeed();
    if (speed == 0)
        speed = 1; // 避免除以0

    return accelerationTime + distance / speed;
}

// 根据设备ID获取轨道上的位置
double Scheduler::deviceToPosition(int deviceId) const
{
    // 简单示例，根据设备id返回轨道上的位置
    return (deviceId * (trackLength / deviceMap.size()));
}
