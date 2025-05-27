// Scheduler.hpp (C++ header version based on revised TypeScript source)
#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include "CarController.hpp"
#include "PortDevice.hpp"
#include "CarTask.hpp"

enum class TaskStatus
{
    Waiting,
    InProgress,
    Done
};

struct TaskDetail
{
    int taskId;
    std::string materialId;
    std::string type;
    int fromDevice;
    int toDevice;
    long long createTime;
    std::optional<long long> startTime;
    std::optional<int> carId;
    std::optional<long long> pickUpTime;
    std::optional<long long> dropOffTime;
    std::optional<long long> takenTime;
    TaskStatus status;
    double progress;
};

struct Assignment
{
    std::shared_ptr<CarController> car;
    std::shared_ptr<PortDevice> port;
    double from;
    double arriveTime;
    double finishTime;
    std::pair<double, double> path;
};

class Scheduler
{
public:
    Scheduler(std::vector<std::shared_ptr<CarController>> cars,
              std::map<int, std::shared_ptr<PortDevice>> deviceMap,
              double trackLength);

    void update(double deltaTime);
    void setAccelerationTime(double time);
    double getTime() const;
    double getAccelerationTime() const;
    void addTask(const CarTask &task);
    void assignTasksToDevices();
    void assignTasks();
    void updateTaskProgress(int taskId, double progress);
    void pickUpCargo(int taskId);
    void dropOffCargo(int taskId);
    void takeCargo(int taskId);
    std::optional<TaskDetail> getTaskProgress(int taskId) const;
    void completeTask(int taskIdOrMaterialId, bool byMaterialId = false);
    std::vector<TaskDetail> getAssignedTasks() const;
    std::vector<TaskDetail> getCompletedTasks() const;
    std::vector<TaskDetail> getCompletedTaskDetails() const;
    std::vector<std::map<std::string, std::string>> getCarsSpeedTimeline() const;
    std::vector<std::map<std::string, std::string>> getTaskDetails() const;
    bool checkIfAllTasksDone();
    double getVirtualClock() const;

private:
    std::vector<std::shared_ptr<CarController>> cars;
    std::vector<CarTask> taskQueue;
    std::map<int, TaskDetail> assignedTasks;
    std::map<int, TaskDetail> completedTasks;
    std::map<int, std::shared_ptr<PortDevice>> deviceMap;
    std::map<int, int> assignedTask;

    double trackLength;
    const double carLength = 2.0;
    double virtualClock = 0;
    double accelerationTime = 1.0;
    bool isTasksOver = false;

    bool isPathConflict(const Assignment &a, const Assignment &b) const;
    bool isInCircularPath(double from, double to, double pos) const;
    bool hasConflict(const std::vector<Assignment> &assignments) const;
    std::vector<std::vector<Assignment>> generateAllAssignmentCombinations(
        const std::vector<std::shared_ptr<CarController>> &cars,
        const std::vector<std::shared_ptr<PortDevice>> &ports,
        double trackLength,
        double pickTime = 5.0) const;
    void preventCollision();
    double calculateTimeToTarget(const CarController &car, double targetPos) const;
    double deviceToPosition(int deviceId) const;
};

#endif // SCHEDULER_HPP
