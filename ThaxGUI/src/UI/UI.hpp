#pragma once

#include <functional>

#include "DMG/DMG.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"

#include "Graphics/Graphics.hpp"

namespace dmg
{
    inline void InitializeImGui(const GraphicsContext& graphics_context)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(graphics_context.window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();

        io.FontGlobalScale = 2.0f;

        DMG_INFO("ImGui UI was initialized.");
    }

    inline void StartImGuiFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    inline void EndImGuiFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    inline void DestroyImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        DMG_INFO("ImGui UI was destroyed.");
    }

    template<typename T>
    void PrintRegisterValue(const std::string& text, T value)
    {
        ImGui::Text(text.c_str());
        ImGui::SameLine();
        ImGui::Text("%X", value);
    }

    inline void DrawRegisterWindow(const RegisterInfo& register_info, const DMG& dmg)
    {
        if (ImGui::Begin("Registers"))
        {
            if (ImGui::BeginTable("##anything", 2))
            {
                ImGui::TableNextColumn();
                PrintRegisterValue("PC: ", register_info.PC);
                PrintRegisterValue("A: ", register_info.A);
                PrintRegisterValue("B: ", register_info.B);
                PrintRegisterValue("D: ", register_info.D);
                PrintRegisterValue("H: ", register_info.H);
                ImGui::TableNextColumn();
                PrintRegisterValue("SP: ", register_info.SP);
                PrintRegisterValue("F: ", register_info.F);
                PrintRegisterValue("C: ", register_info.C);
                PrintRegisterValue("E: ", register_info.E);
                PrintRegisterValue("L: ", register_info.L);
            }
            ImGui::EndTable();

            ImGui::SeparatorText("Memory Mapped Registers");
            if (ImGui::BeginTable("##anything", 2))
            {
                ImGui::TableNextColumn();
                PrintRegisterValue("DIV: ", dmg.ReadMemory(0xFF04));
                PrintRegisterValue("TIMA: ", dmg.ReadMemory(0xFF05));
                ImGui::TableNextColumn();
                PrintRegisterValue("TMA: ", dmg.ReadMemory(0xFF06));
                PrintRegisterValue("TAC: ", dmg.ReadMemory(0xFF07));
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    inline void DoIfEnabled(bool condition, const std::function<void()>& func)
    {
        if (condition) ImGui::BeginDisabled();

        func();

        if (condition) ImGui::EndDisabled();
    }

    inline void DrawControlWindow(DMG& dmg)
    {
        if (ImGui::Begin("Control"))
        {
            static std::array<char, 256> path_buffer;
            ImGui::InputText("ROM Path", path_buffer.data(), sizeof(path_buffer));

            if (ImGui::Button("Load"))
            {
                dmg.LoadROM(path_buffer.data());
            }

            if (ImGui::Button("Start"))
            {
                dmg.SetState(DMG::PlayState::PLAY);
            }

            DoIfEnabled(dmg.GetState() != DMG::PlayState::PLAY, [&dmg]
            {
                ImGui::SameLine();

                if (ImGui::Button("Pause"))
                {
                    dmg.SetState(DMG::PlayState::PAUSE);
                }
            });

            if (ImGui::Button("Start & Pause"))
            {
                dmg.SetState(DMG::PlayState::PAUSE);
            }

            DoIfEnabled(dmg.GetState() == DMG::PlayState::IDLE, [&dmg]
            {
                if (ImGui::Button("Stop"))
                {
                    dmg.SetState(DMG::PlayState::IDLE);
                }
            });

            DoIfEnabled(dmg.GetState() != DMG::PlayState::PAUSE,[&dmg]
            {
                ImGui::SameLine();

                if (ImGui::Button("Step Instruction"))
                {
                    dmg.StepInstruction();
                }

                if (ImGui::Button("Step Frame"))
                {
                    dmg.StepFrame();
                }
            });
        }
        ImGui::End();
    }

    inline void DrawMemoryWindow(const DMG& dmg, const RegisterInfo& register_info)
    {
        static MemoryEditor mem_edit;
        static std::array<uint8_t, 65'536> memory {0};

        mem_edit.HighlightMin = register_info.PC;
        mem_edit.HighlightMax = register_info.PC + 1;
        mem_edit.HighlightColor = IM_COL32(0, 255, 0, 255);

        for (int i = 0; i < 65'536; ++i)
        {
            memory[i] = dmg.ReadMemory(i);
        }
        mem_edit.DrawWindow("Memory", memory.data(), memory.size());
    }
}