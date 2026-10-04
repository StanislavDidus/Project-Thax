#ifdef _WIN32
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#endif

#include "DMG/DMG.hpp"
#include "Graphics/Graphics.hpp"
#include "DMG/Utils/Log.hpp"
#include "UI/UI.hpp"
#include "imgui_memory_editor.h"
//#include "../../GameBoyCore/src/Test/Test.hpp"

int main()
{
#ifdef _WIN32
    timeBeginPeriod(1);
#endif

    dmg::Log::Init();
    dmg::GraphicsContext graphics_context = dmg::InitializeGraphicsContext();
    dmg::InitializeImGui(graphics_context);

    dmg::DMG dmg = dmg::DMG{dmg::DMG::InitState::STANDARD};
    dmg.LoadROM("roms/test_suits/01-special.gb");

    std::array<uint8_t, 65'536> dmg_memory {0};

    constexpr std::chrono::duration<double> FRAME_DURATION {1.0 / 59.7};
    std::chrono::time_point<std::chrono::high_resolution_clock> frame_start = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(graphics_context.window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(graphics_context.window, &width, &height);
        glViewport(0, 0, width, height);

        dmg.Update();

        glClear(GL_COLOR_BUFFER_BIT);

        dmg::StartImGuiFrame();

        ImGui::DockSpaceOverViewport();

        dmg::RegisterInfo registers = dmg.GetRegisters();

        dmg::DrawRegisterWindow(registers);
        dmg::DrawControlWindow(dmg);
        dmg::DrawMemoryWindow(dmg, registers);
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

    DMG_INFO("Program finished.");

#ifdef _WIN32
    timeEndPeriod(1);
#endif
    return 0;
}
