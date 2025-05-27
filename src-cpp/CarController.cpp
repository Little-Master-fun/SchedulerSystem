#include "CarController.hpp"
#include "Scheduler.hpp"
#include <algorithm>

CarController::CarController(int id, std::map<int, PortDevice *> &deviceMap, double initialPosition)
    : id(id), deviceMap(deviceMap), position(initialPosition)
{
}

void CarController::setScheduler(Scheduler *scheduler)
{
    this->scheduler = scheduler;
}

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

void CarController::tryResume()
{
    if (status == CarStatus::Waiting)
    {
        setTargetSpeed(maxStraightSpeed);
        status = CarStatus::Moving;
    }
}

void CarController::setTarget(double pos)
{
    // simplistic logic; more complex logic may involve path planning
    if (pos < position)
        targetSpeed = -maxStraightSpeed;
    else
        targetSpeed = maxStraightSpeed;
}

void CarController::setTargetSpeed(double v)
{
    targetSpeed = v;
}

void CarController::update(double dt)
{
    if (status == CarStatus::Idle || isCollision)
        return;

    updateMotion(dt);

    if (status == CarStatus::Moving && reachedTarget())
    {
        if (currentStopIndex == 0)
        {
            status = CarStatus::Loading;
            loadingTimer = 0.0;
        }
        else if (currentStopIndex == 1)
        {
            status = CarStatus::Unloading;
            unloadingTimer = 0.0;
        }
        setTargetSpeed(0);
    }

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

    if (addCarSpeedTable)
    {
        addCarSpeedTable(id, position, speed, static_cast<int>(status));
    }
}

void CarController::updateMotion(double dt)
{
    double maxSpeed = inCurve(position) ? maxCurveSpeed : maxStraightSpeed;
    if (std::abs(targetSpeed) > maxSpeed)
        targetSpeed = (targetSpeed > 0 ? 1 : -1) * maxSpeed;

    acceleration = (targetSpeed - speed) * 2.0;
    speed += acceleration * dt;
    position += speed * dt;

    if (position >= trackLength)
        position -= trackLength;
    else if (position < 0)
        position += trackLength;
}

bool CarController::inCurve(double pos) const
{
    for (const auto &range : curveRanges)
    {
        if (pos >= range.first && pos <= range.second)
            return true;
    }
    return false;
}

bool CarController::reachedTarget() const
{
    return std::abs(position - stopPoints[currentStopIndex]) < 0.5;
}

double CarController::getPosition() const
{
    return position;
}

double CarController::getSpeed() const
{
    return speed;
}

CarStatus CarController::getStatus() const
{
    return status;
}

double CarController::getDistanceTo(const CarController &car, double trackLen) const
{
    double dist = car.position - position;
    if (dist < 0)
        dist += trackLen;
    return dist;
}

double CarController::getMaxStraightSpeed() const
{
    return maxStraightSpeed;
}

CarController::DisplayInfo CarController::getDisplayProps() const
{
    return DisplayInfo{position, status};
}

CarController::FullInfo CarController::getAllInfo() const
{
    return FullInfo{
        id, position, speed, acceleration, status, task, portFrom, portTo, hasMaterial};
}
