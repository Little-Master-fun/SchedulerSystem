// track_utils.hpp
#ifndef TRACK_UTILS_HPP
#define TRACK_UTILS_HPP

#include <vector>
#include <string>
#include <cmath>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


inline long long getCurrentTimeMs()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(
               system_clock::now().time_since_epoch())
        .count();
}


struct Vector3
{
    double x;
    double y;
    double z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    double distanceTo(const Vector3 &other) const
    {
        return std::sqrt((x - other.x) * (x - other.x) +
                         (y - other.y) * (y - other.y) +
                         (z - other.z) * (z - other.z));
    }

    Vector3 lerp(const Vector3 &other, double t) const
    {
        return Vector3(x + (other.x - x) * t,
                       y + (other.y - y) * t,
                       z + (other.z - z) * t);
    }
};

inline std::vector<Vector3> createTrack(double R, double L = 40.0, int arcSegments = 50)
{
    std::vector<Vector3> points;
    for (int i = 0; i <= arcSegments; ++i)
    {
        double angle = M_PI / 2 + (double(i) / arcSegments) * M_PI;
        points.emplace_back(-L / 2 + R * std::cos(angle), 0, R * std::sin(angle));
    }
    for (int i = 0; i <= arcSegments; ++i)
    {
        double angle = (3 * M_PI) / 2 + (double(i) / arcSegments) * M_PI;
        points.emplace_back(L / 2 + R * std::cos(angle), 0, R * std::sin(angle));
    }
    return points;
}

inline std::vector<Vector3> initTrackInner()
{
    return createTrack(2.5);
}

inline std::vector<Vector3> initTrackOuter()
{
    return createTrack(3.7);
}

inline std::vector<Vector3> initTrackCar()
{
    std::vector<Vector3> points = createTrack(3.1);
    points.emplace_back(-40.0 / 2.0, 0, 3.1);
    return points;
}

inline Vector3 getPositionOnTrack(double dist)
{
    std::vector<Vector3> trackPoints = initTrackCar();
    std::vector<double> trackLengths = {0};
    double totalLength = 0;
    for (size_t i = 1; i < trackPoints.size(); ++i)
    {
        totalLength += trackPoints[i].distanceTo(trackPoints[i - 1]);
        trackLengths.push_back(totalLength);
    }
    dist = std::fmod(dist, totalLength);
    for (size_t i = 1; i < trackLengths.size(); ++i)
    {
        if (dist <= trackLengths[i])
        {
            double t = (dist - trackLengths[i - 1]) / (trackLengths[i] - trackLengths[i - 1]);
            return trackPoints[i - 1].lerp(trackPoints[i], t);
        }
    }
    return trackPoints[0];
}

struct Task
{
    int taskId;
    std::string materialId;
    std::string type; // "入库" or "出库"
    int fromDevice;
    int toDevice;
    long long createTime;
};

inline std::vector<Task> task2;

inline void initializeTasks()
{
    int taskId = 1;
    for (int fromDevice = 16; fromDevice <= 18; ++fromDevice)
    {
        for (int toDevice = 1; toDevice <= 11; toDevice += 2)
        {
            for (int i = 0; i < 3; ++i)
            {
                Task task;
                task.taskId = taskId;
                task.materialId = "TP" + std::to_string(1000 + taskId).substr(1); // pad to 3 digits
                task.type = "入库";
                task.fromDevice = fromDevice;
                task.toDevice = toDevice;
                task.createTime = task.createTime = getCurrentTimeMs();
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
                task.materialId = "TP" + std::to_string(1000 + taskId).substr(1);
                task.type = "出库";
                task.fromDevice = fromDevice;
                task.toDevice = toDevice;
                task.createTime = task.createTime = getCurrentTimeMs();
                task2.push_back(task);
                ++taskId;
            }
        }
    }
}

#endif // TRACK_UTILS_HPP
