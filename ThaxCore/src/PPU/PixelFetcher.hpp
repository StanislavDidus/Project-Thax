#pragma once
#include <array>
#include <cstdint>

#include "Object.hpp"
#include "Tile.hpp"
#include "DMG/ExternalGlobals.hpp"

namespace dmg
{
    class PPU;
    class FIFO;

    class PixelFetcher
    {
    public:
        PixelFetcher(PPU& ppu, FIFO& fifo);
        ~PixelFetcher() = default;

        void FetchObject(const Object& object);

        void Step();
        void StartScanline();
        void EndScanline();
        void StartWindow();
        void Reset();

        [[nodiscard]] PixelFetcherState GetState() const;

    private:
        enum class FetchType{ TILE, OBJECT};
        struct FetchData
        {
            FetchType m_type = FetchType::TILE;
            ObjectPalette m_object_palette = ObjectPalette::OBP0;
            uint16_t m_address = 0;
            uint8_t m_tile_row = 0;
        };

        void FetchTile();

        PixelFetcherState m_state = PixelFetcherState::GET_TILE_DATA;
        uint32_t m_progress = 0;

        uint32_t m_x = 0;

        FetchData m_current_fetch {};
        FetchData m_last_fetch {};

        uint16_t m_tile_ref = 0;
        uint8_t m_tile_row = 0;
        uint8_t m_tile_low = 0;
        uint8_t m_tile_high = 0;
        //std::optional<uint16_t> m_tile_address = std::nullopt;

        // Window
        bool m_inside_window = false;
        uint16_t m_x_window = 0;
        uint16_t m_y_window = 0;

        PPU& m_ppu;
        FIFO& m_fifo;
    };
}
