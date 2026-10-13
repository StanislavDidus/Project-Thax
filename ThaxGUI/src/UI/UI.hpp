#pragma once

#include <functional>
#include <array>

#include "DMG/DMG.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"
#include "IconsFontAwesome4.h"

#include "imfilebrowser.h"
#include "Data/Data.hpp"

#include "Graphics/Graphics.hpp"

#include "imgui_stdlib.h"

namespace dmg
{
    struct ImGuiContext
    {
        ImGui::FileBrowser rom_file_dialog;
        std::unique_ptr<ROMEntry> root_entry = nullptr;
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

        io.Fonts->AddFontFromFileTTF("assets/fonts/Inconsolata_Expanded-Medium.ttf", 13.0f);
        static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MIN_FA, 0};
        ImFontConfig config;
        config.MergeMode = true;
        config.PixelSnapH = true;
        io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

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
            if (ImGui::Button(ICON_FA_PLAY " Play"))
            {
                dmg.SetState(DMG::PlayState::PLAY);
            }

            DoIfEnabled(dmg.GetState() != DMG::PlayState::PLAY, [&dmg]
            {
                ImGui::SameLine();

                if (ImGui::Button(ICON_FA_PAUSE " Pause"))
                {
                    dmg.SetState(DMG::PlayState::PAUSE);
                }
            });

            if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Play & Pause"))
            {
                dmg.SetState(DMG::PlayState::PAUSE);
            }

            DoIfEnabled(dmg.GetState() == DMG::PlayState::IDLE, [&dmg]
            {
                ImGui::SameLine();

                if (ImGui::Button(ICON_FA_STOP " Stop"))
                {
                    dmg.SetState(DMG::PlayState::IDLE);
                }
            });

            DoIfEnabled(dmg.GetState() != DMG::PlayState::PAUSE,[&dmg]
            {
                static int instruction_skip = 1;
                char label[128];
                sprintf(label, "Step Instructions");
                if (ImGui::Button(label))
                {
                    for (int i = 0; i < instruction_skip; ++i)
                        dmg.StepInstruction();
                }

                ImGui::SameLine();
                ImGui::PushItemWidth(150.0f);
                if (ImGui::InputInt("##anything1", &instruction_skip))
                {
                    if (instruction_skip < 0) instruction_skip = 0;
                }

                static int frame_skip = 1;
                char label_1[128];
                sprintf(label_1, "Step Frames");
                if (ImGui::Button(label_1))
                {
                    dmg.StepFrame();
                }

                ImGui::SameLine();
                ImGui::PushItemWidth(150.0f);
                if (ImGui::InputInt("##anything2", &frame_skip))
                {
                    if (frame_skip < 0) frame_skip = 0;
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

    inline void PrintEntry(DMG& dmg, ROMEntry* entry, ROMEntry** selected_rom_entry)
    {
        static int selected = 0;
        static int i = 0;
        if (!entry) return;

        char label[128];

        if (entry->is_directory && !entry->closed_directory)
            sprintf(label, ICON_FA_FOLDER_OPEN " %s", entry->name.c_str());
        else if (entry->is_directory && entry->closed_directory)
            sprintf(label, ICON_FA_FOLDER " %s", entry->name.c_str());
        else
            sprintf(label, ICON_FA_GAMEPAD " %s", entry->name.c_str());

        if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav | ImGuiSelectableFlags_AllowDoubleClick))
        {
            selected = i;
            *selected_rom_entry = entry;

            if (ImGui::IsMouseDoubleClicked(0))
            {
                if (entry->is_directory)
                {
                    entry->closed_directory = !entry->closed_directory;
                }
                else if (!entry->is_directory)
                {
                    dmg.LoadROM(entry->path);
                }
            }
        }
        ++i;

        if (entry->is_directory && !entry->closed_directory)
        {
            ImGui::Indent();
            for (const auto& child : entry->children)
            {
                PrintEntry(dmg, child.get(), selected_rom_entry);
            }
            ImGui::Unindent();
        }
    }

    inline void DrawMain(DMG& dmg, ImGuiContext& imgui_context, Config& config)
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

            static ROMEntry* selected_rom_entry = nullptr;
            {
                ImGui::SetNextWindowSize({ImGui::GetWindowSize().x * 0.5f, 0.0f}, ImGuiCond_Once);
                ImGui::BeginChild("Library", {0,0}, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

                // Get root entry
                if (imgui_context.root_entry != nullptr)
                {
                    for ( const auto& child : imgui_context.root_entry->children )
                    {
                        PrintEntry(dmg, child.get(), &selected_rom_entry);
                    }
                }
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginGroup();
                ImGui::BeginChild("File Description", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                if (selected_rom_entry)
                {
                    std::string file_type;
                    if (selected_rom_entry->is_directory) file_type = "Folder";
                    else if (!selected_rom_entry->is_directory) file_type = "File";
                    ImGui::Text("%s", file_type.c_str());
                    ImGui::Separator();
                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::CalcTextSize(selected_rom_entry->name.c_str()).x * 0.5f);
                    ImGui::Text(selected_rom_entry->name.c_str());
                    //ImGui::Image();
                    //ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    //ImVec2 p = ImGui::GetCursorPos();
                    //draw_list->AddRect(p, ImVec2{p.x + 100.0f, p.y + 100.0f}, IM_COL32(255,0,0,255));
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }
            ImGui::SameLine();
        }
        ImGui::End();

        if (show_settings)
        {
            ImGui::FileBrowser& browser = imgui_context.rom_file_dialog;
            static std::string rom_folder_path_buffer {config.rom_folder};

            if (ImGui::Begin("Settings", &show_settings))
            {
                ImGui::SeparatorText("Choose your ROM folder path");

                ImGui::InputTextWithHint("##anything", "path to your rom directory", &rom_folder_path_buffer, ImGuiInputTextFlags_ElideLeft);

                ImGui::SameLine();

                if (ImGui::Button("Open File Explorer"))
                {
                    browser.Open();
                }
                if (ImGui::Button("Search for ROMs"))
                {
                    //SearchROMs(std::string{rom_folder_path_buffer.data()});
                    //imgui_context.root_entry = std::move(rom_entries[rom_folder_path_buffer.data()]);
                    imgui_context.root_entry = std::move(SearchROMs(std::string{rom_folder_path_buffer.data()}));
                }
            }
            ImGui::End();

            browser.Display();

            if (browser.HasSelected())
            {
                std::filesystem::path folder = browser.GetSelected();

                const std::string s = folder.string();
                rom_folder_path_buffer = folder.string();
                /*
                std::array<char, 256>& buf = rom_folder_path_buffer;
                const size_t n = std::min(s.size(), buf.size() - 1);
                std::copy_n(folder.string().begin(), n, buf.begin());
                buf[n] = '\0';
                */

                config.rom_folder = folder;

                browser.ClearSelected();
            }
        }
    }

}
