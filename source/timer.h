#pragma once

#include <chrono>
#include <string>
#include "logger.h"

namespace ntn
{
    class Timer
    {
    public:
        Timer() :m_startTime(std::chrono::high_resolution_clock::now()),m_deltaTime(0.0f) {};

        Timer(const std::string& name) : m_startTime(std::chrono::high_resolution_clock::now()),
                                               m_deltaTime(0.0f),
                                               m_nameFunction(name) {};

        ~Timer()
        {
            std::string msg = m_nameFunction + "....excecuted in  ";
            float time_excuted = getTotalTime() * 1000;
            msg += std::to_string(time_excuted);
            msg += " ms";
            Log::info(msg);
        }
        // Update the time and calculate delta time
        void update()
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            m_deltaTime = std::chrono::duration<float>(currentTime - m_lastTime).count();
            m_lastTime = currentTime;
        }

        // Get the time elapsed since the last update
        float getDeltaTime() const
        {
            return m_deltaTime;
        }

        // Get the total time elapsed since the Time object was created
        float getTotalTime() const
        {
            return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - m_startTime).count();
        }

    private:
        std::string m_nameFunction;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
        float m_deltaTime = 0.0f;
    };
}