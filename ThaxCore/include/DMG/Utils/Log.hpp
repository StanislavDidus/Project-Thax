#pragma once

#include <format>
#include <functional>
#include <utility>

namespace dmg
{
    class Log
    {
    public:
        enum class Type : uint8_t
        {
            INFO,
            WARN,
            ERR,
        };

        Log(const Type type, std::string  text) : m_type(type), m_text(std::move(text)) {}

        [[nodiscard]] Type GetType() const { return m_type; }
        [[nodiscard]] const std::string& GetText() const { return m_text; }
    private:
        Type m_type = Type::INFO;
        std::string m_text {};
    };

    class Logger
    {
    public:
        Logger();
        ~Logger() = default;

        static Logger& GetLogger()
        {
            static Logger s;
            return s;
        }

        void ClearLogs();

        void PushLog(Log::Type type, const std::string& text);
        [[nodiscard]] const std::vector<Log>& GetLogs() const;
    private:
        std::vector<Log> m_messages;
        std::string m_prefix {};
    };
}

#define DMG_INFO(...)  ::dmg::Logger::GetLogger().PushLog(::dmg::Log::Type::INFO, std::format(__VA_ARGS__))
#define DMG_WARN(...)  ::dmg::Logger::GetLogger().PushLog(::dmg::Log::Type::WARN, std::format(__VA_ARGS__))
#define DMG_ERROR(...) ::dmg::Logger::GetLogger().PushLog(::dmg::Log::Type::ERR,  std::format(__VA_ARGS__))

/*#define DMG_TRACE(...)      ::dmg::Log::GetLogger()->trace(__VA_ARGS__)
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
#endif*/