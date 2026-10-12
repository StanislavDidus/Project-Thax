#pragma once

#include <functional>
#include <array>

#include "DMG/DMG.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"

#include "imfilebrowser.h"

#include "Graphics/Graphics.hpp"

namespace dmg
{
    struct ROMEntry
    {
        std::string name;
        std::filesystem::path path;
    };

    struct ImGuiContext
    {
        ImGui::FileBrowser rom_file_dialog;
        std::vector<ROMEntry> rom_entries;
    };

    inline ImGuiContext InitializeImGui(const GraphicsContext& graphics_context)
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

        // Initialize File Browser
        ImGuiContext context;
        context.rom_file_dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_SelectDirectory};
        context.rom_file_dialog.SetTitle("File Explorer");

        DMG_INFO("ImGui UI was initialized.");

        return context;
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
            ImGui::SeparatorText("CPU Registers");

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

    inline std::vector<ROMEntry> SearchROMs(const std::filesystem::path& path)
    {
        if (!std::filesystem::is_directory(path))
        {
            DMG_ERROR("Trying to load ROMs from not a folder.");
            return {};
        }

        using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

        std::vector<ROMEntry> rom_entries {};
        for (const auto& dir_entry : recursive_directory_iterator(path))
        {
            if (!dir_entry.is_directory())
            {
                ROMEntry rom_entry{};
                rom_entry.name = dir_entry.path().stem();
                rom_entry.path = dir_entry.path();
                rom_entries.emplace_back(rom_entry);
            }
        }

        return rom_entries;
    }

    inline void DrawMain(ImGuiContext& imgui_context)
    {
        static bool show_settings = false;
        if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    ImGui::MenuItem("Settings", nullptr, &show_settings);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            static int selected = 0;
            {
                ImGui::BeginChild("Library", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0), ImGuiChildFlags_Borders);
                for (int i = 0; i < imgui_context.rom_entries.size(); i++)
                {
                    ROMEntry rom_entry = imgui_context.rom_entries[i];
                    char label[128];
                    sprintf(label, "%s", rom_entry.name.c_str());
                    if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                        selected = i;
                }
                ImGui::EndChild();
            }
            ImGui::SameLine();
        }
        ImGui::End();

        if (show_settings)
        {
            ImGui::FileBrowser& browser = imgui_context.rom_file_dialog;
            static std::array<char, 256> rom_folder_path_buffer {};

            if (ImGui::Begin("Settings", &show_settings))
            {
                ImGui::SeparatorText("Choose your ROM folder path");

                ImGui::InputTextWithHint("##anything", "path to your rom directory", rom_folder_path_buffer.data(), rom_folder_path_buffer.size());

                ImGui::SameLine();

                if (ImGui::Button("Open File Explorer"))
                {
                    browser.Open();
                }
                if (ImGui::Button("Search for ROMs"))
                {
                    imgui_context.rom_entries = SearchROMs(std::string{rom_folder_path_buffer.data()});
                }
            }
            ImGui::End();

            browser.Display();

            if (browser.HasSelected())
            {
                std::filesystem::path folder = browser.GetSelected();

                const std::string s = folder.string();
                std::array<char, 256>& buf = rom_folder_path_buffer;
                const size_t n = std::min(s.size(), buf.size() - 1);
                std::copy_n(folder.string().begin(), n, buf.begin());
                buf[n] = '\0';

                browser.ClearSelected();
            }
        }
    }

}
