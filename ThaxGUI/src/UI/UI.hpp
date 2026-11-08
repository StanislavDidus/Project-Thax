#pragma once

#include <functional>
#include <array>
#include <span>

#include "DMG/DMG.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"
#include "IconsFontAwesome4.h"

#include "imfilebrowser.h"
#include "imgui_internal.h"
#include "Data/Data.hpp"

#include "Graphics/Graphics.hpp"

#include "UIAssets.hpp"

#include "imgui_stdlib.h"

namespace dmg
{
    struct ImGuiContext
    {
        std::unique_ptr<ROMEntry> root_entry = nullptr;

    	bool show_vram_viewer = false;
    	bool show_memory_layout = false;
    	bool show_register_info = false;
    	bool show_control_menu = false;
    };

    class FileBrowser
    {
    public:
        FileBrowser(std::optional<ImGuiFileBrowserFlags_> flags, const std::string& title, const std::vector<std::string>& filters)
        {
            m_file_browser = ImGui::FileBrowser{flags.value()};
            m_file_browser.SetTitle(title);
            m_file_browser.SetTypeFilters(filters);
        }
        ImGui::FileBrowser m_file_browser;
    };

    inline void DrawTextInputField(const char* id, const std::string& title, const std::vector<std::string>& filters,
                                    const std::string& hint, std::filesystem::path& buffer, int flags = 0)
    {
        static std::unordered_map<ImGuiID, ImGui::FileBrowser> browsers;
        ImGui::PushID(id);
        const ImGuiID key = ImGui::GetID("browser");
        if (!browsers.contains(key))
        {
            browsers[key] = ImGui::FileBrowser{static_cast<unsigned int>(flags)};
            browsers[key].SetTitle(title);
            browsers[key].SetTypeFilters(filters);
        }

        ImGui::FileBrowser& browser = browsers[key];

        std::string b = buffer.string();
        if (ImGui::InputTextWithHint("##anything", hint.c_str(), &b, ImGuiInputTextFlags_ElideLeft))
            buffer = b;

        ImGui::SameLine();
        if (ImGui::Button("Open File Explorer"))
        {
            browser.Open();
        }

        browser.Display();

        if (browser.HasSelected())
        {
            std::filesystem::path path = browser.GetSelected();

            const std::string s = path.string();
            buffer = path.string();
            browser.ClearSelected();
        }

        ImGui::PopID();
    }

    inline void SetupImGuiStyleWhite()
	{
		// Comfortable Light Orange style by SouthCraftX from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 1.0f;
		style.WindowPadding = ImVec2(20.0f, 20.0f);
		style.WindowRounding = 11.5f;
		style.WindowBorderSize = 0.0f;
		style.WindowMinSize = ImVec2(20.0f, 20.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 20.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 17.4f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(20.0f, 3.4f);
		style.FrameRounding = 11.9f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.9f, 13.4f);
		style.ItemInnerSpacing = ImVec2(7.1f, 1.8f);
		style.CellPadding = ImVec2(12.1f, 9.2f);
		style.IndentSpacing = 0.0f;
		style.ColumnsMinSpacing = 8.7f;
		style.ScrollbarSize = 11.6f;
		style.ScrollbarRounding = 15.9f;
		style.GrabMinSize = 3.7f;
		style.GrabRounding = 20.0f;
		style.TabRounding = 9.8f;
		style.TabBorderSize = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.7254902f, 0.68235296f, 0.54901963f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.90588236f, 0.8980392f, 0.88235295f, 1.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.84313726f, 0.83137256f, 0.80784315f, 1.0f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.8862745f, 0.8745098f, 0.84705883f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84313726f, 0.83137256f, 0.80784315f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.84313726f, 0.83137256f, 0.80784315f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.9529412f, 0.94509804f, 0.92941177f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9529412f, 0.94509804f, 0.92941177f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.9019608f, 0.89411765f, 0.8784314f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.9529412f, 0.94509804f, 0.92941177f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.84313726f, 0.83137256f, 0.80784315f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.96862745f, 0.050980393f, 0.15686275f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.9647059f, 0.8f, 0.02745098f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.96862745f, 0.5882353f, 0.03529412f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.81960785f, 0.8117647f, 0.8039216f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.84705883f, 0.84705883f, 0.84705883f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.85882354f, 0.8352941f, 0.7921569f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.89411765f, 0.89411765f, 0.89411765f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.87058824f, 0.8509804f, 0.80784315f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.84313726f, 0.8156863f, 0.7490196f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.84313726f, 0.8156863f, 0.7490196f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.85490197f, 0.85490197f, 0.85490197f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.96862745f, 0.050980393f, 0.15686275f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.92156863f, 0.9137255f, 0.8980392f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.8745098f, 0.7254902f, 0.42745098f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.47843137f, 0.4f, 0.29803923f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9607843f, 0.019607844f, 0.11764706f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90588236f, 0.6627451f, 0.30980393f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.6392157f, 0.39607844f, 0.043137256f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.9529412f, 0.94509804f, 0.92941177f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.9529412f, 0.94509804f, 0.92941177f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.88235295f, 0.8666667f, 0.8509804f, 1.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.9019608f, 0.89411765f, 0.8784314f, 1.0f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0627451f, 0.0627451f, 0.0627451f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.5019608f, 0.4862745f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.73333335f, 0.70980394f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.5019608f, 0.4862745f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8039216f, 0.8235294f, 0.45490196f, 0.502f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8039216f, 0.8235294f, 0.45490196f, 0.502f);
	}

    inline void SetupImGuiStyleGrey()
    {
	    // Visual Studio style by MomoDeve from ImThemes
	    ImGuiStyle& style = ImGui::GetStyle();

	    style.Alpha = 1.0f;
	    style.DisabledAlpha = 0.6f;
	    style.WindowPadding = ImVec2(8.0f, 8.0f);
	    style.WindowRounding = 0.0f;
	    style.WindowBorderSize = 1.0f;
	    style.WindowMinSize = ImVec2(32.0f, 32.0f);
	    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	    style.WindowMenuButtonPosition = ImGuiDir_Left;
	    style.ChildRounding = 0.0f;
	    style.ChildBorderSize = 1.0f;
	    style.PopupRounding = 0.0f;
	    style.PopupBorderSize = 1.0f;
	    style.FramePadding = ImVec2(4.0f, 3.0f);
	    style.FrameRounding = 0.0f;
	    style.FrameBorderSize = 0.0f;
	    style.ItemSpacing = ImVec2(8.0f, 4.0f);
	    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	    style.CellPadding = ImVec2(4.0f, 2.0f);
	    style.IndentSpacing = 21.0f;
	    style.ColumnsMinSpacing = 6.0f;
	    style.ScrollbarSize = 14.0f;
	    style.ScrollbarRounding = 0.0f;
	    style.GrabMinSize = 10.0f;
	    style.GrabRounding = 0.0f;
	    style.TabRounding = 0.0f;
	    style.TabBorderSize = 0.0f;
	    style.ColorButtonPosition = ImGuiDir_Right;
	    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921569f, 0.5921569f, 0.5921569f, 1.0f);
	    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_Border] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
	    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
	    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_Separator] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
	    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
	    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
	    style.Colors[ImGuiCol_Tab] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
	    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
	    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
	    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
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
    	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(graphics_context.window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();

        io.FontGlobalScale = 2.0f;

        ImFontConfig base_config;
        base_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF(inconsolata_medium_font_data, inconsolata_medium_font_size, 13.0f, &base_config);

        ImFontConfig imgui_config;
        imgui_config.FontDataOwnedByAtlas = false;
        imgui_config.MergeMode = true;
        imgui_config.PixelSnapH = true;
        static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MIN_FA, 0};
        io.Fonts->AddFontFromMemoryTTF(fontawesome_webfont_data, fontawesome_webfont_size, 13.0f, &imgui_config, icon_ranges);

        // Initialize File Browser
        ImGuiContext context;

        DMG_INFO("ImGui UI was initialized.");

        return context;
    }

	inline void InitializeImGuiTheme(Config& config)
    {
    	if (config.theme == 0)
    		SetupImGuiStyleWhite();
    	else if (config.theme == 1)
    		SetupImGuiStyleGrey();
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

	inline void DrawImage(GLuint gl_texture, float width, float height)
    {
    	const ImVec2 avail_size = ImGui::GetContentRegionAvail();
    	const float scale_x = avail_size.x / width;
    	const float scale_y = avail_size.y / height;
    	const float final_scale = scale_x < scale_y ? scale_x : scale_y;
    	ImVec2 viewport_size
    	{
    		width * final_scale,
			height * final_scale,
		};

    	const ImVec2 cursor = ImGui::GetCursorPos();
    	const float offset_x = (avail_size.x - viewport_size.x) * 0.5f;
    	const float offset_y = (avail_size.y - viewport_size.y) * 0.5f;

    	ImGui::SetCursorPos(ImVec2(
			cursor.x + (offset_x > 0.0f ? offset_x : 0.0f),
			cursor.y + (offset_y > 0.0f ? offset_y : 0.0f)));

    	ImGui::Image(static_cast<ImTextureID>(gl_texture), viewport_size);
    }


    inline void DrawVRAMViewer(ImGuiContext& context, unsigned int tiles_texture, unsigned int tilemap1_texture, unsigned int tilemap2_texture)
    {
    	if (context.show_vram_viewer)
    	{
    		if (ImGui::Begin("VRAM Viewer", &context.show_vram_viewer))
    		{
    			if (ImGui::BeginTabBar("##TabBar"))
    			{
    				if (ImGui::BeginTabItem("Tiles"))
    				{
    					DrawImage(tiles_texture, globals::TILE_DATA_VIEW_WIDTH, globals::TILE_DATA_VIEW_HEIGHT);
    					ImGui::EndTabItem();
    				}
    				if (ImGui::BeginTabItem("BG Tilemap"))
    				{
    					DrawImage(tilemap1_texture, globals::TILEMAP_DATA_WIDTH, globals::TILEMAP_DATA_HEIGHT);
    					ImGui::EndTabItem();
    				}
    				if (ImGui::BeginTabItem("Window Tilemap"))
    				{
    					DrawImage(tilemap2_texture, globals::TILEMAP_DATA_WIDTH, globals::TILEMAP_DATA_HEIGHT);
    					ImGui::EndTabItem();
    				}
    				ImGui::EndTabBar();
    			}
    		}
    		ImGui::End();
    	}
    }

    inline void DrawViewport(unsigned int viewport_texture)
    {
        if (ImGui::Begin("Viewport"))
        {
        	DrawImage(viewport_texture, globals::LCD_WIDTH, globals::LCD_HEIGHT);
        }
        ImGui::End();
    }

    inline void DrawInfoWindow(const DMGInfo& info, const DMG& dmg, ImGuiContext& imgui_context)
    {
    	if (imgui_context.show_register_info)
    	{
    		if (ImGui::Begin("Info", &imgui_context.show_register_info))
    		{
    			ImGui::SeparatorText("States");

    			std::string cpu_state;
    			switch(info.CPU_State)
    			{
    			case 0: cpu_state = "NORMAL"; break;
    			case 1: cpu_state = "HALT"; break;
    			case 2: cpu_state = "STOP"; break;
			    default: ;
			    }
    			ImGui::Text("CPU State: %s", cpu_state.c_str());

    			std::string ppu_state;
    			switch(info.ppu_info.ppu_mode)
    			{
    			case PPUMode::HBLANK: ppu_state = "MODE0"; break;
    			case PPUMode::VBLANK: ppu_state = "MODE1"; break;
    			case PPUMode::OAM_SCAN: ppu_state = "MODE2"; break;
    			case PPUMode::DRAWING_PIXELS: ppu_state = "MODE3"; break;
    			case PPUMode::NONE: ppu_state = "NONE"; break;
    			}
    			ImGui::Text("PPU State: %s", ppu_state.c_str());

    			std::string pixel_fetcher_state;
    			switch (info.ppu_info.pixel_fetcher_state)
    			{
    			case PixelFetcherState::GET_TILE_DATA: pixel_fetcher_state = "GET_TILE_DATA"; break;
    			case PixelFetcherState::GET_TILE_DATA_LOW: pixel_fetcher_state = "GET_TILE_DATA_LOW"; break;
    			case PixelFetcherState::GET_TILE_DATA_HIGH: pixel_fetcher_state = "GET_TILE_DATA_HIGH"; break;
    			case PixelFetcherState::PUSH: pixel_fetcher_state = "PUSH"; break;
    			default: pixel_fetcher_state = "NONE"; break;
    			}
    			ImGui::Text("Pixel Fetcher State: %s", pixel_fetcher_state.c_str());
    			ImGui::Text("FIFO Size: %d", info.ppu_info.fifo_pixels);

    			ImGui::SeparatorText("Registers");
    			ImGui::SeparatorText("CPU Registers");

    			if (ImGui::BeginTable("##anything", 2))
    			{
    				ImGui::TableNextColumn();
    				PrintRegisterValue("PC: ", info.PC);
    				PrintRegisterValue("A: ", info.A);
    				PrintRegisterValue("B: ", info.B);
    				PrintRegisterValue("D: ", info.D);
    				PrintRegisterValue("H: ", info.H);
    				ImGui::TableNextColumn();
    				PrintRegisterValue("SP: ", info.SP);
    				PrintFRegisterValue(info.F);
    				PrintRegisterValue("C: ", info.C);
    				PrintRegisterValue("E: ", info.E);
    				PrintRegisterValue("L: ", info.L);
    			}
    			ImGui::EndTable();

    			ImGui::SeparatorText("General Component Registers");
    			if (ImGui::BeginTable("##anything", 3))
    			{
    				ImGui::TableNextColumn();
    				PrintRegisterValue("DIV: ", info.DIV);
    				PrintRegisterValue("TIMA: ", info.TIMA);
    				PrintRegisterValue("TMA: ", info.TMA);
    				PrintRegisterValue("TAC: ", info.TAC);
    				ImGui::TableNextColumn();
    				PrintRegisterValue("IF: ", info.IF);
    				PrintRegisterValue("IE: ", info.IE);
    				PrintRegisterValue("*IME: ", info.IME);
    				ImGui::TableNextColumn();
    				PrintRegisterValue("P1: ", info.P1);
    				PrintRegisterValue("SB: ", info.SB);
    				PrintRegisterValue("SC: ", info.SC);
    			}
    			ImGui::EndTable();

    			ImGui::SeparatorText("PPU Registers");
    			if (ImGui::BeginTable("##anything", 3))
    			{
    				const auto& ppu_info = info.ppu_info;
    				ImGui::TableNextColumn();
    				PrintRegisterValue("LCDC: ", ppu_info.LCDC);
    				PrintRegisterValue("STAT: ", ppu_info.STAT);
    				PrintRegisterValue("SCY: ", ppu_info.SCY);
    				PrintRegisterValue("SCX: ", ppu_info.SCX);
    				ImGui::TableNextColumn();
    				PrintRegisterValue("LY: ", ppu_info.LY);
    				PrintRegisterValue("LYC: ", ppu_info.LYC);
    				PrintRegisterValue("WX: ", ppu_info.WX);
    				PrintRegisterValue("WY: ", ppu_info.WY);
    				ImGui::TableNextColumn();
    				PrintRegisterValue("DMA: ", ppu_info.DMA);
    				PrintRegisterValue("BGP: ", ppu_info.BGP);
    				PrintRegisterValue("OBP0: ", ppu_info.OBP0);
    				PrintRegisterValue("OBP1: ", ppu_info.OBP1);
    			}
    			ImGui::EndTable();

    		}
    		ImGui::End();
    	}
    }

    inline void DoIfEnabled(bool condition, const std::function<void()>& func)
    {
        if (condition) ImGui::BeginDisabled();

        func();

        if (condition) ImGui::EndDisabled();
    }

    inline void DrawControlWindow(DMG& dmg, ImGuiContext& imgui_context, const Config& config)
    {
	    if (imgui_context.show_control_menu)
	    {
	    	if (ImGui::Begin("Control", &imgui_context.show_control_menu))
	    	{
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
    }

    	inline void DrawMemoryWindow(const DMG& dmg, const DMGInfo& register_info, ImGuiContext& imgui_context)
	    {
	    	if (imgui_context.show_memory_layout)
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
	    		mem_edit.DrawWindow("Memory", imgui_context.show_memory_layout, memory.data(), memory.size());
	    	}
	    }

    	inline void PrintEntry(DMG& dmg, const Config& config, ROMEntry* entry, ROMEntry** selected_rom_entry)
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
	    	//bool is_loaded = context.selected_rom == entry->path;
	    	//if (is_loaded)
	    	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
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
	    				if (config.bootrom_load_option == 1) dmg.LoadBootrom(config.bootrom_file);
	    				dmg.SetState(DMG::PlayState::PLAY);
	    				//context.selected_rom = entry->path;
	    			}
	    		}
	    	}
	    	//if (is_loaded)
	    	//ImGui::PopStyleColor();
	    	ImGui::PopID();
	    	++i;

	    	if (entry->is_directory && !entry->closed_directory)
	    	{
	    		ImGui::Indent();
	    		for (const auto& child : entry->children)
	    		{
	    			PrintEntry(dmg, config, child.get(), selected_rom_entry);
	    		}
	    		ImGui::Unindent();
	    	}
	    }

    	/*
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
				if (ImGui::Begin("Settings", &show_settings))
				{
					ImGui::SeparatorText("Select a path to your preferred rom folder");
					DrawTextInputField("1", "Select rom folder", {},
						"Select rom folder", config.rom_folder, ImGuiFileBrowserFlags_SelectDirectory);

					if (ImGui::Button("Search for ROMs"))
					{
						imgui_context.root_entry = std::move(SearchROMs(config.rom_folder));
					}

					ImGui::SeparatorText("Load bootrom");
					static const char* items[] = {"Initialize without a bootrom", "Specify path to bootrom"};
					int& current_item = config.bootrom_load_option;
					ImGui::Combo("##combo", &current_item, items, IM_ARRAYSIZE(items));

					if (current_item == 1)
					{
						DrawTextInputField("2", "Select bootrom", {".gb"},
							"Select bootrom", config.bootrom_file, 0);
					}
				}
				ImGui::End();

			}
		}
		*/

    	inline void DrawLibrary(ImGuiContext& imgui_context, const Config& config, DMG& dmg, GLuint viewport_texture)
	    {
	    	if (ImGui::Begin("Library"))
	    	{
	    		if (dmg.GetState() == DMG::PlayState::IDLE)
	    		{
	    			static ROMEntry* selected_rom_entry = nullptr;
	    			if (imgui_context.root_entry != nullptr)
	    				for ( const auto& child : imgui_context.root_entry->children )
	    					PrintEntry(dmg, config, child.get(), &selected_rom_entry);
	    		}
	    		else
	    		{
	    			ImVec2 avail_size = ImGui::GetContentRegionAvail();

	    			const float scale_x = avail_size.x / globals::LCD_WIDTH;
	    			const float scale_y = avail_size.y / globals::LCD_HEIGHT;
	    			const float final_scale = scale_x < scale_y ? scale_x : scale_y;
	    			ImVec2 viewport_size
	    			{
	    				globals::LCD_WIDTH * final_scale,
						globals::LCD_HEIGHT * final_scale,
					};

	    			const ImVec2 cursor = ImGui::GetCursorPos();
	    			const float offset_x = (avail_size.x - viewport_size.x) * 0.5f;
	    			const float offset_y = (avail_size.y - viewport_size.y) * 0.5f;

	    			ImGui::SetCursorPos(ImVec2(
						cursor.x + (offset_x > 0.0f ? offset_x : 0.0f),
						cursor.y + (offset_y > 0.0f ? offset_y : 0.0f)));

	    			ImGui::Image(static_cast<ImTextureID>(viewport_texture), viewport_size);
	    		}
	    	}
	    	ImGui::End();
	    }

    	inline void DrawMain2(ImGuiContext& imgui_context, DMG& dmg, Config& config)
	    {
	    	static bool show_settings = false;
	    	static bool show_roms = false;
	    	static bool show_themes = false;

	    	if (ImGui::BeginMainMenuBar())
	    	{
	    		if (ImGui::BeginMenu("System"))
	    		{
	    			if (dmg.GetState() == DMG::PlayState::PLAY)
	    			{
	    				if (ImGui::Button("Pause"))
	    				{
	    					dmg.SetState(DMG::PlayState::PAUSE);
	    				}
	    			}
	    			else if (dmg.GetState() == DMG::PlayState::PAUSE)
	    			{
	    				if (ImGui::Button("Resume"))
	    				{
	    					dmg.SetState(DMG::PlayState::PLAY);
	    				}
	    			}
	    			else
	    			{
	    				ImGui::BeginDisabled(); ImGui::Button("Pause"); ImGui::EndDisabled();
	    			}

	    			DoIfEnabled(dmg.GetState() == DMG::PlayState::IDLE, [&dmg]
					{
						if (ImGui::Button("Reset"))
							dmg.SetState(DMG::PlayState::IDLE);
					});

	    			if (ImGui::Button("Exit"))
	    			{
	    				std::exit(0);
	    			}
	    			ImGui::EndMenu();
	    		}
	    		if (ImGui::BeginMenu("Settings"))
	    		{
	    			if (ImGui::MenuItem("Roms", nullptr, &show_settings)) show_roms = true;
	    			if (ImGui::MenuItem("Themes", nullptr, &show_settings)) show_themes = true;
	    			ImGui::EndMenu();
	    		}
	    		if (ImGui::BeginMenu("Debug"))
	    		{
	    			ImGui::MenuItem("VRAM Viewer", nullptr, &imgui_context.show_vram_viewer);
	    			ImGui::MenuItem("Memory Layout", nullptr, &imgui_context.show_memory_layout);
	    			ImGui::MenuItem("Register Info", nullptr, &imgui_context.show_register_info);
	    			ImGui::MenuItem("Control Menu", nullptr, &imgui_context.show_control_menu);
	    			ImGui::EndMenu();
	    		}
	    		if (ImGui::BeginMenu("Help"))
	    		{
	    			ImGui::TextLinkOpenURL("GitHub Page", "https://github.com/StanislavDidus/Project-Thax");
	    			//if (ImGui::Button("GitHub Page"))
	    			//{
	    			//}
	    			ImGui::EndMenu();
	    		}
	    		ImGui::EndMainMenuBar();
	    	}
	    	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

	    	if (show_settings)
	    	{
	    		if (ImGui::Begin("Settings", &show_settings))
	    		{
	    			if (ImGui::BeginTabBar("##TabBar"))
	    			{
	    				ImGuiTabItemFlags_ flags = show_roms ? ImGuiTabItemFlags_SetSelected : static_cast<ImGuiTabItemFlags_>(0);
	    				if (ImGui::BeginTabItem("Roms", nullptr, flags))
	    				{
	    					show_roms = false;
	    					ImGui::SeparatorText("Select a path to your preferred rom folder");
	    					DrawTextInputField("1", "Select rom folder", {},
								"Select rom folder", config.rom_folder, ImGuiFileBrowserFlags_SelectDirectory);

	    					if (ImGui::Button("Search for ROMs"))
	    					{
	    						imgui_context.root_entry = std::move(SearchROMs(config.rom_folder));
	    					}

	    					ImGui::SeparatorText("Load bootrom");
	    					static const char* items[] = {"Initialize without a bootrom", "Specify path to bootrom"};
	    					int& current_item = config.bootrom_load_option;
	    					ImGui::Combo("##combo", &current_item, items, IM_ARRAYSIZE(items));

	    					if (current_item == 1)
	    					{
	    						DrawTextInputField("2", "Select bootrom", {".gb"},
									"Select bootrom", config.bootrom_file, 0);
	    					}
	    					ImGui::EndTabItem();
	    				}

	    				flags = show_themes ? ImGuiTabItemFlags_SetSelected : static_cast<ImGuiTabItemFlags_>(0);
	    				if (ImGui::BeginTabItem("Themes", nullptr, flags))
	    				{
	    					show_themes = false;
	    					static const char* themes[] = {"Light (Default)", "Visual Studio Code Like"};
	    					int& current_theme = config.theme;
	    					if (ImGui::Combo("Choose a theme", &current_theme, themes, IM_ARRAYSIZE(themes)))
	    					{
	    						if (current_theme == 0)
	    						{
	    							SetupImGuiStyleWhite();
	    						}
	    						if (current_theme == 1)
	    						{
	    							SetupImGuiStyleGrey();
	    						}
	    					}
	    					ImGui::EndTabItem();
	    				}
	    				ImGui::EndTabBar();
	    			}
	    		}
	    		ImGui::End();
	    	}
	    }
}
