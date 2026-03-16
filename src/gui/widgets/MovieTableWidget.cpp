/// @file MovieTableWidget.cpp
/// @brief Реализация виджета таблицы фильмов.

#include "gui/widgets/MovieTableWidget.h"

#include <imgui.h>

namespace FilmLibrary
{
    MovieTableWidget::MovieTableWidget() = default;
    MovieTableWidget::~MovieTableWidget() = default;

    void MovieTableWidget::Render(const std::vector<Movie*>& movies)
    {
        // TODO: Реализовать отрисовку таблицы через ImGui::BeginTable / ImGui::EndTable.
        //
        // Столбцы: Название | Студия | Год | Рейтинг | Длительность
        //
        // 1. ImGui::BeginTable("movies_table", 5, flags).
        // 2. ImGui::TableSetupColumn("Название"), etc.
        // 3. ImGui::TableHeadersRow().
        // 4. Для каждого Movie* - ImGui::TableNextRow(), заполнить ячейки.
        // 5. Обработать клик по строке - обновить selectedMovieId.
        // 6. Обработать клик по заголовку - вызвать onColumnClick.
        // 7. ImGui::EndTable().

        (void)movies;
    }

    int MovieTableWidget::GetSelectedMovieId() const
    {
        return selectedMovieId;
    }

    void MovieTableWidget::ClearSelection()
    {
        selectedMovieId = -1;
    }

    void MovieTableWidget::SetOnColumnClick(OnColumnClickCallback callback, void* userData)
    {
        onColumnClick = callback;
        columnClickUserData = userData;
    }
}
