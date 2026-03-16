#pragma once

/// @file AppWindow.h
/// @brief Главное окно приложения - инициализация GLFW, Dear ImGui,
///        основной render-loop.

#include "gui/AppController.h"

struct GLFWwindow;

namespace FilmLibrary
{
    class AppWindow
    {
        public:
            /// @param title  Заголовок окна.
            /// @param width  Начальная ширина (пикс.).
            /// @param height Начальная высота (пикс.).
            AppWindow(const char* title, int width, int height);
            ~AppWindow();

            AppWindow(const AppWindow&) = delete;
            AppWindow& operator=(const AppWindow&) = delete;

            /// @brief Инициализировать GLFW + ImGui.
            /// @return true при успехе.
            bool Init();

            /// @brief Войти в главный цикл отрисовки.
            /// @param controller  Контроллер приложения, поставляющий данные.
            void Run(AppController& controller);

            /// @brief Освободить ресурсы.
            void Shutdown();

        private:
            const char* title;
            int width;
            int height;
            GLFWwindow* window = nullptr;

            /// @brief Настроить стиль Dear ImGui.
            void SetupImGuiStyle();
    };
}
