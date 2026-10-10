#pragma once

#include "spdlog/spdlog.h"

namespace dmg
{
    class Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return m_logger; }
    private:
        static std::shared_ptr<spdlog::logger> m_logger;
    };
}

#define DMG_TRACE(...)      ::dmg::Log::GetLogger()->trace(__VA_ARGS__)
#define DMG_INFO(...)       ::dmg::Log::GetLogger()->info(__VA_ARGS__)
#define DMG_WARN(...)       ::dmg::Log::GetLogger()->warn(__VA_ARGS__)
#define DMG_ERROR(...)      ::dmg::Log::GetLogger()->error(__VA_ARGS__)
#define DMG_FATAL(...)      ::dmg::Log::GetLogger()->fatal(__VA_ARGS__)

#ifdef NDEBUG
#undef DMG_TRACE
#define DMG_TRACE
#undef DMG_INFO
#define DMG_INFO
#undef DMG_WARN
#define DMG_WARN
#undef DMG_ERROR
#define DMG_ERROR
#undef DMG_FATAL
#define DMG_FATAL
#endif