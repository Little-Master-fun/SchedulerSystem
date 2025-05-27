#include "CarController.hpp"
#include "Scheduler.hpp"
#include <algorithm>

// 构造函数，初始化小车控制器
CarController::CarController(int id, std::map<int, PortDevice *> &deviceMap, double initialPosition)
    : id(id), deviceMap(deviceMap), position(initialPosition)
{
}

// 设置调度器
void CarController::setScheduler(Scheduler *scheduler)
{
    this->scheduler = scheduler;
}

// 分配任务，设置起止点
void CarController::assignTask(std::shared_ptr<CarTask> task, double fromPos, double toPos)
{
    this->task = task;
    this->portFrom = deviceMap[task->fromDevice];
    this->portTo = deviceMap[task->toDevice];
    this->status = CarStatus::Moving;
    stopPoints = {fromPos, toPos};
    currentStopIndex = 0;
    hasMaterial = false;
    setTarget(stopPoints[0]);
}

// 尝试恢复运行
void CarController::tryResume()
{
    if (status == CarStatus::Waiting)
    {
        setTargetSpeed(maxStraightSpeed);
        status = CarStatus::Moving;
    }
}

// 设置目标位置
void CarController::setTarget(double pos)
{
    // 简单逻辑；更复杂的逻辑可能涉及路径规划
    if (pos < position)
        targetSpeed = -maxStraightSpeed;
    else
        targetSpeed = maxStraightSpeed;
}

// 设置目标速度
void CarController::setTargetSpeed(double v)
{
    targetSpeed = v;
}

// 更新小车状态
void CarController::update(double dt)
{
    if (status == CarStatus::Idle || isCollision)
        return;

    updateMotion(dt);

    // 到达目标点后的处理
    if (status == CarStatus::Moving && reachedTarget())
    {
        if (currentStopIndex == 0)
        {
            status = CarStatus::Loading; // 装载中
            loadingTimer = 0.0;
        }
        else if (currentStopIndex == 1)
        {
            status = CarStatus::Unloading; // 卸载中
            unloadingTimer = 0.0;
        }
        setTargetSpeed(0);
    }

    // 装载过程
    if (status == CarStatus::Loading)
    {
        loadingTimer += dt;
        if (loadingTimer >= loadingDuration)
        {
            status = CarStatus::Loaded;
            hasMaterial = true;
            currentStopIndex++;
            setTarget(stopPoints[currentStopIndex]);
            status = CarStatus::Moving;
        }
    }

    // 卸载过程
    if (status == CarStatus::Unloading)
    {
        unloadingTimer += dt;
        if (unloadingTimer >= unloadingDuration)
        {
            status = CarStatus::Idle;
            hasMaterial = false;
            task = nullptr;
        }
    }

    // 记录速度表
    if (addCarSpeedTable)
    {
        addCarSpeedTable(id, position, speed, static_cast<int>(status));
    }
}

// 更新小车运动状态
void CarController::updateMotion(double dt)
{
    double maxSpeed = inCurve(position) ? maxCurveSpeed : maxStraightSpeed;
    if (std::abs(targetSpeed) > maxSpeed)
        targetSpeed = (targetSpeed > 0 ? 1 : -1) * maxSpeed;

    acceleration = (targetSpeed - speed) * 2.0;
    speed += acceleration * dt;
    position += speed * dt;

    // 环形轨道处理
    if (position >= trackLength)
        position -= trackLength;
    else if (position < 0)
        position += trackLength;
}

// 判断当前位置是否在弯道
bool CarController::inCurve(double pos) const
{
    for (const auto &range : curveRanges)
    {
        if (pos >= range.first && pos <= range.second)
            return true;
    }
    return false;
}

// 判断是否到达目标点
bool CarController::reachedTarget() const
{
    return std::abs(position - stopPoints[currentStopIndex]) < 0.5;
}

// 获取当前位置
double CarController::getPosition() const
{
    return position;
}

// 获取当前速度
double CarController::getSpeed() const
{
    return speed;
}

// 获取当前状态
CarStatus CarController::getStatus() const
{
    return status;
}

// 获取与另一辆车的距离
double CarController::getDistanceTo(const CarController &car, double trackLen) const
{
    double dist = car.position - position;
    if (dist < 0)
        dist += trackLen;
    return dist;
}

// 获取最大直线速度
double CarController::getMaxStraightSpeed() const
{
    return maxStraightSpeed;
}

// 获取显示信息
CarController::DisplayInfo CarController::getDisplayProps() const
{
    return DisplayInfo{position, status};
}

// 获取全部信息
CarController::FullInfo CarController::getAllInfo() const
{
    return FullInfo{
        id, position, speed, acceleration, status, task, portFrom, portTo, hasMaterial};
}
