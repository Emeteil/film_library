/// @file AppWindow.cpp
/// @brief Реализация главного окна приложения (GLFW + Dear ImGui).

#include "gui/AppWindow.h"
#include "gui/widgets/MainWidget.h"
#include "core/utils/Logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <cstdio>

namespace FilmLibrary
{
    AppWindow::AppWindow(const char* title, int width, int height)
        : title(title), width(width), height(height)
    {
    }

    AppWindow::~AppWindow()
    {
        Shutdown();
    }

    bool AppWindow::Init()
    {
        if (!glfwInit())
        {
            Logger::Instance().Error("Не удалось инициализировать GLFW");
            return false;
        }

#if defined(__arm__) || defined(__aarch64__)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_PROFILE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
        {
            Logger::Instance().Error("Не удалось создать окно GLFW");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 3;
        fontConfig.OversampleV = 3;
        const char* customFont = "fonts/NotoSans-Bold.ttf";
        const char* fallbackFont = "NotoSans-Bold.ttf";

        FILE* f1 = std::fopen(customFont, "rb");
        if (f1)
        {
            std::fclose(f1);
            io.Fonts->AddFontFromFileTTF(customFont, 16.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
        }
        else
        {
            FILE* f2 = std::fopen(fallbackFont, "rb");
            if (f2)
            {
                std::fclose(f2);
                io.Fonts->AddFontFromFileTTF(fallbackFont, 16.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
            }
            else
            {
                io.Fonts->AddFontDefault();
            }
        }

        static const ImWchar emoji_ranges[] = {
            0x2000, 0x2BFF,
            0x2600, 0x27BF,
            0x1F000, 0x1FAD6,
            0
        };

        const char* emojiFont = "fonts/NotoEmoji.ttf";
        const char* fallbackEmojiFont = "NotoEmoji.ttf";
        FILE* f3 = std::fopen(emojiFont, "rb");
        if (f3)
        {
            std::fclose(f3);
            ImFontConfig emojiConfig;
            emojiConfig.MergeMode = true;
            emojiConfig.OversampleH = 1;
            emojiConfig.OversampleV = 1;
            io.Fonts->AddFontFromFileTTF(emojiFont, 16.0f, &emojiConfig, emoji_ranges);
        }
        else
        {
            FILE* f4 = std::fopen(fallbackEmojiFont, "rb");
            if (f4)
            {
                std::fclose(f4);
                ImFontConfig emojiConfig;
                emojiConfig.MergeMode = true;
                emojiConfig.OversampleH = 1;
                emojiConfig.OversampleV = 1;
                io.Fonts->AddFontFromFileTTF(fallbackEmojiFont, 16.0f, &emojiConfig, emoji_ranges);
            }
        }

#if defined(__arm__) || defined(__aarch64__)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 300 es");
#else
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
#endif

        SetupImGuiStyle();

        Logger::Instance().Info("GUI инициализировано");
        return true;
    }

    void AppWindow::Run(AppController& controller)
    {
        MainWidget mainWidget;

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            mainWidget.Render(controller, window);

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);

            glClearColor(0.07f, 0.07f, 0.09f, 1.0f);
            glClear(0x00004000); // GL_COLOR_BUFFER_BIT

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    }

    void AppWindow::Shutdown()
    {
        if (!window)
            return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        Logger::Instance().Info("GUI остановлен");
    }

    void AppWindow::SetupImGuiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        style.WindowRounding       = 10.0f;
        style.ChildRounding        = 8.0f;
        style.FrameRounding        = 6.0f;
        style.PopupRounding        = 10.0f;
        style.ScrollbarRounding    = 8.0f;
        style.GrabRounding         = 6.0f;
        style.TabRounding          = 6.0f;
        style.WindowBorderSize     = 1.0f;
        style.ChildBorderSize      = 1.0f;
        style.FrameBorderSize      = 0.0f;

        style.WindowTitleAlign     = ImVec2(0.5f, 0.5f);
        style.WindowPadding        = ImVec2(14.0f, 12.0f);
        style.FramePadding         = ImVec2(10.0f, 6.0f);
        style.ItemSpacing          = ImVec2(10.0f, 8.0f);
        style.ItemInnerSpacing     = ImVec2(8.0f, 6.0f);
        style.IndentSpacing        = 20.0f;
        style.ScrollbarSize        = 10.0f;
        style.GrabMinSize          = 10.0f;

        colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.10f, 0.10f, 0.13f, 0.97f);

        colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.40f, 0.45f, 1.00f);

        colors[ImGuiCol_Border]                = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_FrameBg]               = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);

        colors[ImGuiCol_TitleBg]               = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.04f, 0.04f, 0.05f, 0.90f);
        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);

        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.07f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.30f, 0.22f, 0.10f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.75f, 0.58f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);

        colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.75f, 0.58f, 0.22f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);

        colors[ImGuiCol_Button]                = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.50f, 0.38f, 0.12f, 1.00f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.70f, 0.54f, 0.18f, 1.00f);

        colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.17f, 0.07f, 1.00f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.50f, 0.38f, 0.12f, 0.90f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.70f, 0.54f, 0.18f, 1.00f);

        colors[ImGuiCol_Separator]             = ImVec4(0.22f, 0.18f, 0.10f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.75f, 0.58f, 0.22f, 0.80f);
        colors[ImGuiCol_SeparatorActive]       = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);

        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.75f, 0.58f, 0.22f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.75f, 0.58f, 0.22f, 0.60f);
        colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.90f, 0.72f, 0.28f, 0.95f);

        colors[ImGuiCol_Tab]                   = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.50f, 0.38f, 0.12f, 0.90f);
        colors[ImGuiCol_TabActive]             = ImVec4(0.30f, 0.22f, 0.08f, 1.00f);
        colors[ImGuiCol_TabUnfocused]          = ImVec4(0.09f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.11f, 0.05f, 1.00f);

        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.11f, 0.09f, 0.05f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.25f, 0.20f, 0.10f, 1.00f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.16f, 0.14f, 0.08f, 1.00f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

        colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.75f, 0.58f, 0.22f, 0.35f);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(0.90f, 0.72f, 0.28f, 0.90f);
        colors[ImGuiCol_NavHighlight]          = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.05f, 0.05f, 0.07f, 0.70f);

        colors[ImGuiCol_PlotLines]             = ImVec4(0.75f, 0.58f, 0.22f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.75f, 0.58f, 0.22f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.90f, 0.72f, 0.28f, 1.00f);
    }
}
