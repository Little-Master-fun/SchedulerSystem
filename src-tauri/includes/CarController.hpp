// CarController.hpp
#ifndef CAR_CONTROLLER_HPP
#define CAR_CONTROLLER_HPP

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <cmath>
#include "PortDevice.hpp"
#include "CarTask.hpp"

enum class CarStatus
{
    Idle,
    Moving,
    Loading,
    Loaded,
    Unloading,
    Waiting,
    Cruising
};

class Scheduler; // 前向声明

class CarController
{
public:
    int id;
    double position = 0.0;
    double speed = 0.0;
    double acceleration = 0.0;
    double targetSpeed = 0.0;
    CarStatus status = CarStatus::Idle;
    std::shared_ptr<CarTask> task = nullptr;
    bool isCollision = false;
    PortDevice *portFrom = nullptr;
    PortDevice *portTo = nullptr;
    std::map<int, PortDevice *> &deviceMap;
    bool hasMaterial = false;
    CarController(int carId, std::map<int, PortDevice *> &deviceMapRef) 
        : id(carId), deviceMap(deviceMapRef) {}
    int getId() const { return id; }
    std::function<void(int, double, double, int)> addCarSpeedTable = nullptr;

    CarController(int id, std::map<int, PortDevice *> &deviceMap, double initialPosition = 0.0);
    void update(double dt);
    void setScheduler(Scheduler *scheduler);
    void assignTask(std::shared_ptr<CarTask> task, double fromPos, double toPos);
    void tryResume();
    void setTarget(double pos);
    void setTargetSpeed(double v);
    double getPosition() const;
    double getSpeed() const;
    CarStatus getStatus() const;
    double getDistanceTo(const CarController &car, double trackLength) const;
    double getMaxStraightSpeed() const;

    struct DisplayInfo
    {
        double positionInMeters;
        CarStatus status;
    };
    DisplayInfo getDisplayProps() const;

    struct FullInfo
    {
        int id;
        double position;
        double speed;
        double acceleration;
        CarStatus status;
        std::shared_ptr<CarTask> task;
        PortDevice *portFrom;
        PortDevice *portTo;
        bool hasMaterial;
    };
    FullInfo getAllInfo() const;

private:
    Scheduler *scheduler = nullptr;
    std::vector<double> stopPoints;
    int currentStopIndex = 0;
    std::vector<std::pair<double, double>> curveRanges = {{0, 9.739}, {49.739, 59.478}};
    const double maxCurveSpeed = 0.67;
    const double maxStraightSpeed = 2.67;
    const double trackLength = 99.478;
    double loadingTimer = 0;
    double unloadingTimer = 0;
    const double loadingDuration = 7.5;
    const double unloadingDuration = 7.5;

    bool inCurve(double pos) const;
    void updateMotion(double dt);
    bool reachedTarget() const;
};

#endif // CAR_CONTROLLER_HPP
