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

    constexpr std::chrono::duration<double> FRAME_DURATION {1.0 / 59.7};
    std::chrono::time_point<std::chrono::high_resolution_clock> frame_start = std::chrono::high_resolution_clock::now();

    // Initialize Tile Data Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

        auto tile_data_buffer = dmg.getTileDataBuffer();
        auto tile_data_buffer_rgb = dmg::ConvertBufferToRGB(tile_data_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 144, 168, 0, GL_RGB,
            GL_UNSIGNED_BYTE, tile_data_buffer_rgb.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        dmg::StartImGuiFrame();

        ImGui::DockSpaceOverViewport();

        dmg::RegisterInfo registers = dmg.GetRegisters();

        dmg::DrawAssetWindow(texture);
        dmg::DrawInfoWindow(registers, dmg);
        dmg::DrawControlWindow(dmg);
        dmg::DrawMemoryWindow(dmg, registers);
        dmg::DrawMain(dmg, imgui_context, current_config);
        ImGui::ShowDemoWindow();

        dmg::EndImGuiFrame();

        glfwSwapBuffers(graphics_context.window);

        // End frame -- Update Timers
        std::chrono::time_point<std::chrono::high_resolution_clock> frame_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = frame_end - frame_start;
        double fps = 1.0 / elapsed.count();

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
