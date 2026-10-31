#pragma once
#include <cstdint>

namespace dmg
{
    class Bus;
    class PPU;

    class DMAController
    {
    public:
        DMAController() = default;
        ~DMAController() = default;

        [[nodiscard]] bool IsActive() const;
        void Initialize(const std::shared_ptr<Bus>& bus);
        void Update();

        void Write(uint8_t value);
        [[nodiscard]] uint8_t Read() const;
    private:
        void StartDMATransfer();

        uint8_t m_data = 0;
        bool m_dma_active = false;
        uint8_t m_dma_progress = 0;

        std::weak_ptr<Bus> m_bus {};
    };
}
