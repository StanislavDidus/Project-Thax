#ifdef _WIN32
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#endif

#include "DMG/DMG.hpp"
#include "Input/Input.hpp"
#include "Graphics/Graphics.hpp"
#include "DMG/Utils/Log.hpp"
#include "UI/UI.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
#ifdef _WIN32
    timeBeginPeriod(1);
#endif

    dmg::Log::Init();
    dmg::GraphicsContext graphics_context = dmg::InitializeGraphicsContext();
    dmg::ImGuiContext imgui_context = dmg::InitializeImGui(graphics_context);

    dmg::DMG dmg = dmg::DMG{dmg::DMG::InitState::STANDARD};

    auto config = dmg::LoadConfig();
    dmg::Config current_config {};
    if (config.has_value())
    {
        current_config = config.value();
        imgui_context.root_entry = std::move(dmg::SearchROMs(current_config.rom_folder));
    }

    dmg::InitializeImGuiTheme(current_config);

    constexpr std::chrono::duration<double> FRAME_DURATION {1.0 / 59.7};
    std::chrono::time_point<std::chrono::high_resolution_clock> frame_start = std::chrono::high_resolution_clock::now();

    unsigned int tile_data_texture = dmg::GenerateGLTexture(dmg::globals::TILE_DATA_VIEW_WIDTH, dmg::globals::TILE_DATA_VIEW_HEIGHT);
    unsigned int bg_tilemap_texture = dmg::GenerateGLTexture(dmg::globals::TILEMAP_DATA_WIDTH, dmg::globals::TILEMAP_DATA_HEIGHT);
    unsigned int window_tilemap_texture = dmg::GenerateGLTexture(dmg::globals::TILEMAP_DATA_WIDTH, dmg::globals::TILEMAP_DATA_HEIGHT);
    unsigned int lcd_texture = dmg::GenerateGLTexture(dmg::globals::LCD_WIDTH, dmg::globals::LCD_HEIGHT);

    // Setup icon
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(icon_data), icon_size, &width, &height, &channels, 0);
        if (pixels)
        {
            GLFWimage images[1];
            images[0].width = width;
            images[0].height = height;
            images[0].pixels = pixels;
            glfwSetWindowIcon(graphics_context.window, 1, images);
            stbi_image_free(pixels);
        }
    }

    while (!glfwWindowShouldClose(graphics_context.window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(graphics_context.window, &width, &height);
        glViewport(0, 0, width, height);

        ///// UPDATE /////

        dmg::HandleInput(graphics_context, dmg);

        dmg.Update();

        ///// RENDER /////

        glClear(GL_COLOR_BUFFER_BIT);

        dmg::GenerateTextureFromBuffer(tile_data_texture, dmg::globals::TILE_DATA_VIEW_WIDTH, dmg::globals::TILE_DATA_VIEW_HEIGHT, std::move(dmg.GetTileDataBuffer()));
        dmg::GenerateTextureFromBuffer(bg_tilemap_texture, dmg::globals::TILEMAP_DATA_WIDTH, dmg::globals::TILEMAP_DATA_HEIGHT, std::move(dmg.GetBGTilemapBuffer()));
        dmg::GenerateTextureFromBuffer(window_tilemap_texture, dmg::globals::TILEMAP_DATA_WIDTH, dmg::globals::TILEMAP_DATA_HEIGHT, std::move(dmg.GetWindowTilemapBuffer()));

        glBindTexture(GL_TEXTURE_2D, lcd_texture);
        auto lcd_buffer_rgb = dmg::ConvertBufferToRGB(dmg.GetLCDBuffer());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            dmg::globals::LCD_WIDTH, dmg::globals::LCD_HEIGHT,
            GL_RGB, GL_UNSIGNED_BYTE, lcd_buffer_rgb.data());


        dmg::StartImGuiFrame();

        //ImGui::DockSpaceOverViewport();
        dmg::DMGInfo dmg_info = dmg.GetDMGInfo();

        /*
        dmg::DrawAssetWindow(tile_data_texture);
        dmg::DrawViewport(lcd_texture);
        dmg::DrawInfoWindow(dmg_info, dmg);
        dmg::DrawControlWindow(dmg, imgui_context, current_config);
        dmg::DrawMemoryWindow(dmg, dmg_info);
        dmg::DrawMain(dmg, imgui_context, current_config);
        */
        dmg::DrawMain2(imgui_context, dmg, current_config);
        dmg::DrawLibrary(imgui_context, current_config, dmg, lcd_texture);
        dmg::DrawVRAMViewer(imgui_context, tile_data_texture, bg_tilemap_texture, window_tilemap_texture);
        dmg::DrawMemoryWindow(dmg, dmg_info, imgui_context);
        dmg::DrawInfoWindow(dmg_info, dmg, imgui_context);
        dmg::DrawControlWindow(dmg, imgui_context, current_config);
        //ImGui::ShowDemoWindow();

        dmg::EndImGuiFrame();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(graphics_context.window);

        // End frame -- Update Timers
        std::chrono::time_point<std::chrono::high_resolution_clock> frame_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = frame_end - frame_start;
        double fps = 1.0 / elapsed.count();

        // Change window title
        //std::stringstream window_title;
        //window_title << "Game Boy Emulator (" << fps << ")";
        //glfwSetWindowTitle(graphics_context.window, window_title.str().c_str());

        if (elapsed < FRAME_DURATION)
        {
            auto remaining = FRAME_DURATION - elapsed;
            if (remaining > std::chrono::milliseconds(2))
            std::this_thread::sleep_for(remaining - std::chrono::milliseconds(1));

            // spin for the last bit for precision
            while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - frame_start) < FRAME_DURATION);
        }

        frame_start = std::chrono::high_resolution_clock::now();
    }

    dmg::DestroyImGui();
    dmg::DestroyGraphicsContext(graphics_context);

    dmg::SaveConfig(current_config);

    DMG_INFO("Program finished.");

#ifdef _WIN32
    timeEndPeriod(1);
#endif
    return 0;
}
