#pragma once

#include <chrono>
#include <deque>
#include <fstream>

struct TimerEvent {
    int64_t start;
    int64_t duration;
    const char *name;
};

inline std::deque<TimerEvent> events;

class Timer {
  public:
    Timer(const char *name)
        : m_name(name), m_start(std::chrono::high_resolution_clock::now()) {};

    ~Timer() {
        using namespace std::chrono;
        const auto end = high_resolution_clock::now();

        auto dur = end - m_start;

        auto startUs =
            time_point_cast<microseconds>(m_start).time_since_epoch().count();

        auto durUs = duration_cast<microseconds>(dur).count();
        events.push_back({startUs, durUs, m_name});
    };

  private:
    const char *m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

inline void generateRaport(const std::string &fileName = "events.json") {
    int64_t startOffset = std::numeric_limits<int64_t>::max();

    for (const auto &e : events) {
        if (startOffset > e.start) {
            startOffset = e.start;
        }
    }

    std::ofstream file(fileName, std::ios::out);

    file << "{ \"traceEvents\": [";

    for (uint32_t i = 0; i < events.size(); i++) {
        const auto &e = events.at(i);
        file << "{";
        file << "\"name\":\"" << e.name << "\",";
        file << "\"sf\": 1,";
        file << "\"ph\":\"X\",";
        file << "\"pid\":1,\"tid\":1,";
        file << "\"ts\":" << e.start - startOffset << ",";
        file << "\"dur\":" << e.duration;

        if (i == events.size() - 1) {
            file << "}";
        } else {
            file << "},";
        }
    }

    file << "]}";
    file << "";

    file.close();
}

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define FunctionTimer Timer CONCAT(timerAtLine, __LINE__)(__func__)