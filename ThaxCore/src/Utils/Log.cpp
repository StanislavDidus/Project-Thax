#include "DMG/Utils/Log.hpp"

#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace dmg
{
    std::shared_ptr<spdlog::logger> Log::m_logger = nullptr;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        m_logger = spdlog::stdout_color_mt("DMG");
        m_logger->set_level(spdlog::level::trace);

        DMG_INFO("Logger was initialized.");
    }
}
