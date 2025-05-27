// PortDevice.hpp
#ifndef PORT_DEVICE_HPP
#define PORT_DEVICE_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

enum class PortType
{
    Inlet,
    Outlet,
    InInterface,
    OutInterface
};

enum class PortStatus
{
    Idle,
    Waiting,
    Loading,
    Unloading,
    Full,
    Empty
};

class Scheduler; // 前向声明

class PortDevice
{
public:
    int id;
    PortType type;
    PortStatus status = PortStatus::Idle;
    bool hasCargo = false;
    double timer = 0;
    double position;
    std::vector<int> taskQueue;
    std::optional<int> currentMaterialId;
    Scheduler *scheduler = nullptr;

    PortDevice(int id, PortType type, double position);

    void setScheduler(Scheduler *sched);
    void addTask(int materialId);
    void update(double deltaTime);
    void onMaterialTaken();
    void onMaterialPlaced(int materialId);
    void startOperation(PortStatus status, double duration);

    bool isBusy() const;
    bool isAvailable() const;
    std::optional<int> getMaterialId() const;

private:
    void startNextTask();
    void finishOperation();
};

// 外部设备信息（用于初始化）
extern const std::map<int, double> devicePositions;
extern const std::map<int, PortType> portTypes;

std::vector<std::shared_ptr<PortDevice>> getAllDevices();
std::map<int, std::shared_ptr<PortDevice>> getDeviceMap();
std::optional<PortStatus> getDeviceStatusById(int id);

#endif // PORT_DEVICE_HPP
