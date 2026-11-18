#include "DMG/Utils/Log.hpp"

namespace dmg
{
    Logger::Logger()
    {
        m_prefix = "[DMG] ";
    }

    void Logger::PushLog(Log::Type type, const std::string& text)
    {
        m_messages.emplace_back(type, m_prefix + text);
    }

    const std::vector<Log>& Logger::GetLogs() const
    {
        return m_messages;
    }
}
