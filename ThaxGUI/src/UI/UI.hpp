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

#include "UIAssets.hpp"

#include "imgui_stdlib.h"

namespace dmg
{
    struct ImGuiContext
    {
        ImGui::FileBrowser rom_file_dialog;
        std::unique_ptr<ROMEntry> root_entry = nullptr;
    };


    void SetupImGuiStyle()
    {
	    // Purple Comfy style by RegularLunar from ImThemes
	    ImGuiStyle& style = ImGui::GetStyle();

	    style.Alpha = 1.0f;
	    style.DisabledAlpha = 0.1f;
	    style.WindowPadding = ImVec2(8.0f, 8.0f);
	    style.WindowRounding = 10.0f;
	    style.WindowBorderSize = 0.0f;
	    style.WindowMinSize = ImVec2(30.0f, 30.0f);
	    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	    style.WindowMenuButtonPosition = ImGuiDir_Right;
	    style.ChildRounding = 5.0f;
	    style.ChildBorderSize = 1.0f;
	    style.PopupRounding = 10.0f;
	    style.PopupBorderSize = 0.0f;
	    style.FramePadding = ImVec2(5.0f, 3.5f);
	    style.FrameRounding = 5.0f;
	    style.FrameBorderSize = 0.0f;
	    style.ItemSpacing = ImVec2(5.0f, 4.0f);
	    style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	    style.CellPadding = ImVec2(4.0f, 2.0f);
	    style.IndentSpacing = 5.0f;
	    style.ColumnsMinSpacing = 5.0f;
	    style.ScrollbarSize = 15.0f;
	    style.ScrollbarRounding = 9.0f;
	    style.GrabMinSize = 15.0f;
	    style.GrabRounding = 5.0f;
	    style.TabRounding = 5.0f;
	    style.TabBorderSize = 0.0f;
	    //style.TabMinWidthForCloseButton = 0.0f;
	    style.ColorButtonPosition = ImGuiDir_Right;
	    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	    style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.360515f);
	    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	    style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	    style.Colors[ImGuiCol_Border] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38039216f, 0.42352942f, 0.57254905f, 0.54901963f);
	    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25882354f, 0.25882354f, 0.25882354f, 0.0f);
	    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 0.0f);
	    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23529412f, 0.23529412f, 0.23529412f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_Button] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_Header] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_Separator] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_Tab] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_TabActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509804f, 1.0f, 0.0f);
	    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13333334f, 0.25882354f, 0.42352942f, 0.0f);
	    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.2901961f);
	    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433478f);
	    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
	    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}

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

        ImFontConfig base_config;
        base_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF(inconsolata_medium_font_data, inconsolata_medium_font_size, 13.0f, &base_config);

        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.MergeMode = true;
        config.PixelSnapH = true;
        static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MIN_FA, 0};
        io.Fonts->AddFontFromMemoryTTF(fontawesome_webfont_data, fontawesome_webfont_size, 13.0f, &config, icon_ranges);

        // Initialize File Browser
        ImGuiContext context;
        context.rom_file_dialog = ImGui::FileBrowser{ImGuiFileBrowserFlags_SelectDirectory};
        context.rom_file_dialog.SetTitle("File Explorer");

        SetupImGuiStyle();

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

    inline void PrintFRegisterValue(const uint8_t value)
    {
        ImGui::Text("F: ");
        ImGui::SameLine();
        value >> 7 & 1 ? ImGui::Text("Z") : ImGui::Text("-");
        ImGui::SameLine();
        value >> 6 & 1 ? ImGui::Text("N") : ImGui::Text("-");
        ImGui::SameLine();
        value >> 5 & 1 ? ImGui::Text("H") : ImGui::Text("-");
        ImGui::SameLine();
        value >> 4 & 1 ? ImGui::Text("C") : ImGui::Text("-");
    }

    inline void DrawAssetWindow(unsigned int gl_texture)
    {
        if (ImGui::Begin("Asset Viewport"))
        {
            ImVec2 window_size = ImGui::GetWindowSize();
            ImGui::Image(static_cast<ImTextureID>(gl_texture), window_size);
        }
        ImGui::End();
    }

    inline void DrawViewport(unsigned int viewport_texture)
    {
        if (ImGui::Begin("Viewport"))
        {
            ImVec2 window_size = ImGui::GetWindowSize();
            ImGui::Image(static_cast<ImTextureID>(viewport_texture), window_size);
        }
        ImGui::End();
    }

    inline void DrawInfoWindow(const RegisterInfo& register_info, const DMG& dmg)
    {
        if (ImGui::Begin("Info"))
        {
            if (ImGui::CollapsingHeader("State"))
            {
                ImGui::SeparatorText("CPU State");
            }
            if (ImGui::CollapsingHeader("Registers"))
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
                    PrintFRegisterValue(register_info.F);
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

                ImGui::SeparatorText("Others");
                if (ImGui::BeginTable("##anything", 2))
                {
                    //ImGui::TableNextColumn();
                    //ImGui::TableNextColumn();
                }
                ImGui::EndTable();
            }

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
            if (ImGui::BeginTable("##anything", 2))
            {
                ImGui::TableNextColumn();

                if (ImGui::Button(ICON_FA_PLAY " Play"))
                {
                    dmg.SetState(DMG::PlayState::PLAY);
                }

                if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Play & Pause"))
                {
                    dmg.SetState(DMG::PlayState::PAUSE);
                }

                ImGui::TableNextColumn();

                DoIfEnabled(dmg.GetState() != DMG::PlayState::PLAY, [&dmg]
                {
                    if (ImGui::Button(ICON_FA_PAUSE " Pause"))
                    {
                        dmg.SetState(DMG::PlayState::PAUSE);
                    }
                });

                DoIfEnabled(dmg.GetState() == DMG::PlayState::IDLE, [&dmg]
                {
                    if (ImGui::Button(ICON_FA_STOP " Stop"))
                    {
                        dmg.SetState(DMG::PlayState::IDLE);
                    }
                });
            }
            ImGui::EndTable();


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
        mem_edit.OptShowAscii = false;
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

        ImGui::PushID(i);
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
        ImGui::PopID();
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
            static std::string rom_folder_path_buffer {config.rom_folder.string()};

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
