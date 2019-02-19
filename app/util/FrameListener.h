#ifndef _FRAMELISTENER_H_
#define _FRAMELISTENER_H_

#include <array>
#include <chrono>
#include <ctime>
#include <numeric>
#include <stdio.h>

namespace util {

static const int NUM_FPS_SAMPLES = 64;

class FrameListener {
public:
    static FrameListener *Get() {
        static FrameListener instance;
        return &instance;
    }

    double MaxFPS() const {
        return m_maxFPS;
    }

    double MinFPS() const {
        return m_minFPS;
    }

    double FPS() const {
        return m_fps;
    }

    unsigned int FrameCounter() const {
        return m_frameCounter;
    }

    void update() {
        // update counter
        m_frameCounter ++;

        // update time
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - m_prevTime;
        m_prevTime = end;

        // update fps
        m_fpsSample[m_frameCounter % NUM_FPS_SAMPLES] = 1.0 / elapsed_seconds.count();
        double fps_sum = std::accumulate(m_fpsSample.begin(), m_fpsSample.end(), 0.0);
        m_fps = fps_sum / NUM_FPS_SAMPLES;
        m_maxFPS = m_fps > m_maxFPS ? m_fps : m_minFPS;
        if (m_frameCounter > 100) {
            m_minFPS = m_minFPS > m_fps ? m_fps : m_minFPS;
        }
    }


private:
    FrameListener() {}

    unsigned int m_frameCounter = 0;
    std::chrono::system_clock::time_point m_prevTime;

    double m_fps = 0.0, m_maxFPS = 0.0, m_minFPS = 1000.0;
    std::array<double, NUM_FPS_SAMPLES> m_fpsSample{0.0};
};

} // namespace util

#endif // _FRAMELISTENER_H_
