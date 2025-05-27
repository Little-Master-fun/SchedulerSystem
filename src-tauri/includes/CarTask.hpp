#ifndef CAR_TASK_HPP
#define CAR_TASK_HPP

#include <string>

enum class TaskType
{
    入库,
    出库,
    巡航
};

struct CarTask
{
    int taskId;
    int materialId;
    TaskType type;
    int fromDevice;
    int toDevice;
    long long createTime = 0;   // 时间戳
    int priority = -1;          // 可选字段，-1 表示未设定
    double desiredSpeed = -1.0; // 可选字段，-1.0 表示未设定

    CarTask() = default;

    CarTask(int taskId,
            int materialId,
            TaskType type,
            int fromDevice,
            int toDevice,
            long long createTime,
            int priority = -1,
            double desiredSpeed = -1.0)
        : taskId(taskId),
          materialId(materialId),
          type(type),
          fromDevice(fromDevice),
          toDevice(toDevice),
          createTime(createTime),
          priority(priority),
          desiredSpeed(desiredSpeed) {}
};

#endif // CAR_TASK_HPP
