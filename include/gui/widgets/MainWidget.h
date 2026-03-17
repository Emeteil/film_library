#pragma once

/// @file MainWidget.h
/// @brief Главный виджет (контейнер) для UI, возможно временно (возможно менять будем)

#include "gui/AppController.h"

struct GLFWwindow;

namespace FilmLibrary
{
    class MainWidget
    {
        public:
            MainWidget();
            ~MainWidget();

            /// @brief Отрисовка всех вложенных элементов.
            /// @param controller Контроллер приложения
            /// @param window     Указатель на текущее окно GLFW (например, для закрытия)
            void Render(AppController& controller, GLFWwindow* window);
    };
}