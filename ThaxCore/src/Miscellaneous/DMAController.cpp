#include "Precompiled.hpp"
#include "DMAController.hpp"

#include "MemoryBus/Bus.hpp"

namespace dmg
{
    void DMAController::Initialize(const std::shared_ptr<Bus>& bus)
    {
        m_bus = bus;
    }

    bool DMAController::IsActive() const
    {
        return m_dma_active;
    }

    void DMAController::Write(const uint8_t value)
    {
        m_data = value;
        StartDMATransfer();
    }

    uint8_t DMAController::Read() const
    {
        return m_data;
    }

    void DMAController::StartDMATransfer()
    {
        m_dma_active = true;
        m_dma_progress = 0;
    }

    void DMAController::Update()
    {
        const uint16_t source = Read() * 0x100;
        constexpr uint16_t destination = 0xFE00;
        constexpr uint8_t size = 0xA0;

        if (m_bus.expired()) return;
        const auto bus = m_bus.lock();

        const uint8_t data = bus->Read(source + m_dma_progress);
        bus->Write(destination + m_dma_progress, data);
        m_dma_progress++;
        if(m_dma_progress == size) m_dma_active = false;
    }
}
