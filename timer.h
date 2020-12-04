#pragma once

// #define USE_FMT

#ifdef USE_FMT
#include <fmt/compile.h>
#include <fmt/os.h>
#endif

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
        : m_name(name), m_start(std::chrono::high_resolution_clock::now()){};

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


#ifdef USE_FMT
    auto out = fmt::output_file(fileName);
    out.print("{{ \"traceEvents\": [");

    for (uint32_t i = 0; i < events.size(); i++) {
        const auto &e = events.at(i);
        out.print(FMT_COMPILE("{{\"name\":\"{}\",\"sf\": "
                  "1,\"ph\":\"X\",\"pid\":1,\"tid\":1,\"ts\":{},\"dur\":{}"),
                  e.name, e.start - startOffset, e.duration);

        if (i == events.size() - 1) {
            out.print("}}");
        } else {
            out.print("}},");
        }
    }
    out.print("]}}");

#else
    std::ofstream file(fileName, std::ios::out);

    file << "{ \"traceEvents\": [";

    for (uint32_t i = 0; i < events.size(); i++) {
        const auto &e = events.at(i);
        file << "{\"name\":\"" << e.name
             << "\","
                "\"sf\": 1,"
                "\"ph\":\"X\","
                "\"pid\":1,\"tid\":1,"
                "\"ts\":";

        file << e.start - startOffset;
        file << ",\"dur\":" << e.duration;

        if (i == events.size() - 1) {
            file << "}";
        } else {
            file << "},";
        }
    }
    file << "]}";
#endif
}

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define FunctionTimer Timer CONCAT(timerAtLine, __LINE__)(__func__)