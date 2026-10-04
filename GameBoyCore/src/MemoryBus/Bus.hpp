#pragma once

#include <cstdint>
#include <vector>

namespace dmg
{
    class Bus
    {
    public:
        Bus() = default;
        virtual ~Bus() = default;

        Bus(const Bus& other) = delete;
        Bus(Bus&& other) noexcept = delete;
        Bus& operator=(const Bus& other) = delete;
        Bus& operator=(Bus&& other) noexcept = delete;

        virtual void Clear() = 0;
        [[nodiscard]] virtual uint8_t Read(uint16_t address) const = 0;
        virtual void Write(uint16_t address, uint8_t value) = 0;
        virtual const std::vector<uint16_t>& GetDirtyAddresses() const { return dirty_addresses; }

    protected:
        void AddDirtyAddress(const uint16_t address) { dirty_addresses.emplace_back(address); }
    private:
        std::vector<uint16_t> dirty_addresses {};
    };
}
