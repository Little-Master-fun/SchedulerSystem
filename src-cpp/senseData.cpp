#include "senseData.hpp"

// 这里initializeTasks()函数体比较复杂，不适合inline，单独写在cpp里
void initializeTasks()
{
    task2.clear(); // 先清空已有任务
    int taskId = 1;

    for (int fromDevice = 16; fromDevice <= 18; ++fromDevice)
    {
        for (int toDevice = 1; toDevice <= 11; toDevice += 2)
        {
            for (int i = 0; i < 3; ++i)
            {
                Task task;
                task.taskId = taskId;
                char buf[8];
                snprintf(buf, sizeof(buf), "TP%03d", taskId);
                task.materialId = std::string(buf);

                task.type = "入库";
                task.fromDevice = fromDevice;
                task.toDevice = toDevice;
                task.createTime = getCurrentTimeMs();
                task2.push_back(task);
                ++taskId;
            }
        }
    }

    for (int fromDevice = 2; fromDevice <= 12; fromDevice += 2)
    {
        for (int toDevice = 13; toDevice <= 15; ++toDevice)
        {
            for (int i = 0; i < 3; ++i)
            {
                Task task;
                task.taskId = taskId;
                char buf[8];
                snprintf(buf, sizeof(buf), "TP%03d", taskId);
                task.materialId = std::string(buf);

                task.type = "出库";
                task.fromDevice = fromDevice;
                task.toDevice = toDevice;
                task.createTime = getCurrentTimeMs();
                task2.push_back(task);
                ++taskId;
            }
        }
    }
}
