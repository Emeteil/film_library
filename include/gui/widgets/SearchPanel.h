#pragma once

/// @file SearchPanel.h
/// @brief Виджет панели поиска и фильтрации для Dear ImGui.
///
/// Содержит строку поиска, выбор режима и элементы фильтрации по диапазону.

#include "gui/AppController.h"

namespace FilmLibrary
{
    class SearchPanel
    {
        public:
            SearchPanel();
            ~SearchPanel();

            /// @brief Отрисовать панель поиска.
            /// @param controller  Контроллер - для выполнения поисковых запросов.
            void Render(AppController& controller);

        private:
            char searchBuffer[256] = {};
            int currentSearchMode = 0;

            float ratingLow = 0.0f;
            float ratingHigh = 10.0f;
            int lengthLowMinutes = 0;
            int lengthHighMinutes = 300;

            bool filterActive = false;
    };
}