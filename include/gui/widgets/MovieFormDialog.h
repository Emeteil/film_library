#pragma once

/// @file MovieFormDialog.h
/// @brief Модальное окно (диалог) для добавления / редактирования фильма.
///
/// Работает как popup в Dear ImGui. Поддерживает ввод жанров.

#include "core/models/Movie.h"

#include <functional>
#include <string>

namespace FilmLibrary
{
    class MovieFormDialog
    {
        public:
            /// @brief Callback, вызываемый при подтверждении ввода.
            using OnSubmitCallback = std::function<void(const Movie& movie)>;

            MovieFormDialog();
            ~MovieFormDialog();

            /// @brief Открыть диалог для добавления нового фильма.
            void OpenForAdd();

            /// @brief Открыть диалог для редактирования существующего фильма.
            void OpenForEdit(const Movie& movie);

            /// @brief Отрисовать диалог (вызывать каждый кадр).
            void Render();

            /// @brief Установить callback, вызываемый при подтверждении.
            void SetOnSubmit(OnSubmitCallback callback);

            /// @brief Проверить, открыт ли диалог.
            bool IsOpen() const;

        private:
            bool isOpen = false;
            bool isEditMode = false;
            int editingMovieId = -1;

            char titleBuffer[256] = {};
            char studioBuffer[256] = {};
            char descriptionBuffer[2048] = {};
            int year = 2024;
            int lengthMinutes = 90;
            float rating = 5.0f;
            char coverBuffer[512] = {};
            char streamLinkBuffer[512] = {};
            char genresBuffer[512] = {};

            OnSubmitCallback onSubmit;

            /// @brief Заполнить буферы из объекта Movie.
            void FillFromMovie(const Movie& movie);

            /// @brief Собрать объект Movie из текущих буферов.
            Movie BuildMovieFromBuffers() const;

            /// @brief Валидировать введённые данные.
            /// @return Текст ошибки или пустая строка при успехе.
            std::string Validate() const;
    };
}
