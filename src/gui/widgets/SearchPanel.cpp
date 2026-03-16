/// @file SearchPanel.cpp
/// @brief Реализация панели поиска и фильтрации.

#include "gui/widgets/SearchPanel.h"

#include <imgui.h>

namespace FilmLibrary
{
    SearchPanel::SearchPanel() = default;
    SearchPanel::~SearchPanel() = default;

    void SearchPanel::Render(AppController& controller)
    {
        // TODO: Реализовать панель поиска.
        //
        // 1. ImGui::InputText("Поиск", searchBuffer, sizeof(searchBuffer)).
        // 2. ImGui::Combo для выбора режима поиска (Title, Studio, Year, Description).
        // 3. Кнопка «Найти» → controller.PerformSearch().
        // 4. Кнопка «Сбросить» → controller.ClearSearch().
        //
        // Фильтрация по диапазону:
        // 5. ImGui::SliderFloat("Рейтинг от", &ratingLow, 0, 10).
        // 6. ImGui::SliderFloat("Рейтинг до", &ratingHigh, 0, 10).
        // 7. ImGui::SliderInt("Длительность от", &lengthLowMinutes, 0, 300).
        // 8. Кнопка «Применить фильтр» → controller.FilterByRatingRange() / FilterByLengthRange().

        (void)controller;
    }
}
