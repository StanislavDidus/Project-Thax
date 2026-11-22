#include "DMG/Utils/Log.hpp"

#include "Precompiled.hpp"

namespace dmg
{
    Logger::Logger()
    {
        m_prefix = "[DMG]";

        //PushLog(Log::Type::ERR, "ERROR");
        //PushLog(Log::Type::WARN, "This is a warning");
    }

    void Logger::ClearLogs()
    {
        m_messages.clear();
    }

    void Logger::PushLog(Log::Type type, const std::string& text)
    {
        auto now = std::chrono::system_clock::now();
        auto today = floor<std::chrono::days>(now);
        std::chrono::hh_mm_ss time_of_day{floor<std::chrono::seconds>(now - today)};

        std::stringstream time;
        time <<  "[" << time_of_day.hours().count() << ":" << time_of_day.minutes().count() << ":" << time_of_day.seconds().count() << "]";

        std::stringstream type_text;
        type_text << "[";
        switch (type)
        {
        case Log::Type::INFO:
            type_text << "INFO";
            break;
        case Log::Type::WARN:
            type_text << "WARN";
            break;
        case Log::Type::ERR:
            type_text << "ERR";
            break;
        }
        type_text << "]";

        m_messages.emplace_back(type, m_prefix + " " + type_text.str() + " " + time.str() + " " + text);
    }

    const std::vector<Log>& Logger::GetLogs() const
    {
        return m_messages;
    }
}
