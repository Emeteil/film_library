/// @file AppWindow.cpp
/// @brief Реализация главного окна приложения (GLFW + Dear ImGui).

#include "gui/AppWindow.h"
#include "gui/widgets/MovieTableWidget.h"
#include "gui/widgets/SearchPanel.h"
#include "gui/widgets/MovieFormDialog.h"
#include "gui/widgets/ConfirmDialog.h"
#include "gui/widgets/NotificationManager.h"
#include "core/utils/Logger.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace FilmLibrary
{
    AppWindow::AppWindow(const char* title, int width, int height) : title(title), width(width), height(height)
    {
    }

    AppWindow::~AppWindow()
    {
        Shutdown();
    }

    bool AppWindow::Init()
    {
        // TODO: Реализовать инициализацию.
        //
        // 1. glfwInit()
        // 2. Задать GLFW window hints (OpenGL 3.3 Core).
        // 3. glfwCreateWindow(width, height, title, nullptr, nullptr).
        // 4. glfwMakeContextCurrent(window).
        // 5. glfwSwapInterval(1) - vsync.
        // 6. ImGui::CreateContext().
        // 7. ImGui_ImplGlfw_InitForOpenGL(window, true).
        // 8. ImGui_ImplOpenGL3_Init("#version 330").
        // 9. SetupImGuiStyle().
        // 10. Вернуть true при успехе.

        return false;
    }

    void AppWindow::Run(AppController& controller)
    {
        // TODO: Реализовать главный цикл отрисовки.
        //
        // while (!glfwWindowShouldClose(window))
        // {
        //     glfwPollEvents();
        //     ImGui_ImplOpenGL3_NewFrame();
        //     ImGui_ImplGlfw_NewFrame();
        //     ImGui::NewFrame();
        //
        //     // --- Отрисовка UI ---
        //     // SearchPanel::Render(controller)
        //     // MovieTableWidget::Render(controller.GetDisplayMovies())
        //     // CRUD-кнопки (Добавить, Редактировать, Удалить)
        //     // MovieFormDialog::Render()
        //     // ConfirmDialog::Render()
        //     // NotificationManager::Instance().Render()
        //
        //     ImGui::Render();
        //     glClear(GL_COLOR_BUFFER_BIT);
        //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //     glfwSwapBuffers(window);
        // }

        (void)controller;
    }

    void AppWindow::Shutdown()
    {
        // TODO: Реализовать очистку.
        //
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
        // glfwDestroyWindow(window);
        // glfwTerminate();
    }

    void AppWindow::SetupImGuiStyle()
    {
        // TODO: Настроить тёмную тему Dear ImGui.
        //
        // ImGuiStyle& style = ImGui::GetStyle();
        // ImGui::StyleColorsDark();
        // Настроить рамки, отступы, цвета под дизайн приложения.
    }
}
