#pragma once

/// @file MovieTableWidget.h
/// @brief Виджет таблицы фильмов для Dear ImGui.
///
/// Отображает список фильмов с поддержкой выделения строки.
/// Не владеет данными - работает с указателями.

#include "core/models/Movie.h"

#include <vector>

namespace FilmLibrary
{
    class MovieTableWidget
    {
        public:
            MovieTableWidget();
            ~MovieTableWidget();

            /// @brief Отрисовать таблицу фильмов.
            /// @param movies  Текущий набор фильмов для отображения.
            void Render(const std::vector<Movie*>& movies);

            /// @brief Получить ID выделенного фильма (-1, если ничего не выделено).
            int GetSelectedMovieId() const;

            /// @brief Сбросить выделение.
            void ClearSelection();

            /// @brief Callback-тип для клика по заголовку столбца (для сортировки).
            using OnColumnClickCallback = void(*)(const char* columnName, void* userData);

            /// @brief Установить коллбэк клика по заголовку столбца.
            void SetOnColumnClick(OnColumnClickCallback callback, void* userData);

        private:
            int selectedMovieId = -1;
            OnColumnClickCallback onColumnClick = nullptr;
            void* columnClickUserData = nullptr;
    };
}
