/// @file MainWidget.cpp
/// @brief Реализация главного виджета (контейнера) для UI, возможно временно (возможно менять будем)

#include "gui/widgets/MainWidget.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <im_anim.h>

namespace FilmLibrary
{
    MainWidget::MainWidget() = default;
    MainWidget::~MainWidget() = default;

    void MainWidget::Render(AppController& controller, GLFWwindow* window)
    {
        iam_update_begin_frame(); // обновление анимаций

        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Film Library Debug", nullptr);
        
        // Плавное случайное покачивание для текста
        float dt = ImGui::GetIO().DeltaTime;
        float wiggle_x = iam_wiggle(ImGui::GetID("wiggle_text"), 5.0f, 2.0f, dt);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + wiggle_x);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Hello, Animated World!");
        
        ImGui::Separator();
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        ImGui::Text("Window Size: %d x %d", width, height);
        
        ImGui::Spacing();

        // Делаем пульсирующую кнопку Close
        float pulse = iam_oscillate(ImGui::GetID("pulse_btn"), 10.0f, 1.5f, iam_wave_sine, 0.0f, dt);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f - pulse/2.0f);
        if (ImGui::Button("Close Application", ImVec2(200 + pulse, 40)))
            glfwSetWindowShouldClose(window, true);
            
        ImGui::End();

        // Рекомендуется вызывать GC раз в некоторое время, чтобы очищать старые состояния анимаций
        static int frame_count = 0;
        if (++frame_count % 600 == 0) iam_gc();

        (void)controller;
    }
}