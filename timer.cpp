/******************************************************************************/
/* timer.cpp                                                                  */
/* Seung Jae (Ryan) Lee                                                       */
/******************************************************************************/

// timer.cpp

#include <chrono>
#include <cstdint>

class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    bool hasStarted;
    bool isRunning;

public:
    Timer() : hasStarted(false), isRunning(false) {}

    static int64_t getCurrentTimeMilliseconds() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }

    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        hasStarted = true;
        isRunning = true;
    }

    void stop() {
        if (isRunning) {
            endTime = std::chrono::high_resolution_clock::now();
            isRunning = false;
        }
    }

    std::chrono::high_resolution_clock::duration duration_lossless() const {
        if (!hasStarted) {
            return std::chrono::high_resolution_clock::duration::zero();
        }
        
        auto currentTime = isRunning ? 
            std::chrono::high_resolution_clock::now() : endTime;
        return currentTime - startTime;
    }

    uint64_t duration_nano() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            duration_lossless()).count();
    }

    uint64_t duration_milli() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            duration_lossless()).count();
    }
    
    uint64_t elapsed() const {
        if (!hasStarted) { return 0; }
        auto currentTime = isRunning ? 
            std::chrono::high_resolution_clock::now() : endTime;
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - startTime).count();
    }
};
