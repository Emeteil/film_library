#pragma once

/// @file MovieFormDialog.h
/// @brief Модальное окно (диалог) для добавления / редактирования фильма.
///
/// Работает как popup в Dear ImGui. Поддерживает ввод жанров и выбор актёров.

#include "core/models/Movie.h"
#include "core/models/Actor.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    class MovieFormDialog
    {
        public:
            /// @brief Callback, вызываемый при подтверждении ввода.
            using OnSubmitCallback = std::function<void(const Movie& movie)>;
            
            /// @brief Callback для добавления нового актёра.
            using OnAddActorCallback = std::function<int(const Actor& actor)>;

            MovieFormDialog();
            ~MovieFormDialog();

            /// @brief Открыть диалог для добавления нового фильма.
            void OpenForAdd();

            /// @brief Открыть диалог для редактирования существующего фильма.
            void OpenForEdit(const Movie& movie, const std::vector<std::unique_ptr<Actor>>& allActors);

            /// @brief Отрисовать диалог (вызывать каждый кадр).
            /// @param allActors  Список всех актёров для выбора.
            void Render(const std::vector<std::unique_ptr<Actor>>& allActors);

            /// @brief Установить callback, вызываемый при подтверждении.
            void SetOnSubmit(OnSubmitCallback callback);
            
            /// @brief Установить callback для добавления нового актёра.
            void SetOnAddActor(OnAddActorCallback callback);

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
            char newActorNameBuffer[256] = {};

            std::vector<bool> actorSelected;
            bool showAddActorPopup = false;

            OnSubmitCallback onSubmit;
            OnAddActorCallback onAddActor;

            /// @brief Заполнить буферы из объекта Movie.
            void FillFromMovie(const Movie& movie, const std::vector<std::unique_ptr<Actor>>& allActors);

            /// @brief Собрать объект Movie из текущих буферов.
            Movie BuildMovieFromBuffers(const std::vector<std::unique_ptr<Actor>>& allActors) const;

            /// @brief Валидировать введённые данные.
            /// @return Текст ошибки или пустая строка при успехе.
            std::string Validate() const;

            /// @brief Отрисовать секцию выбора актёров.
            void RenderActorSelection(const std::vector<std::unique_ptr<Actor>>& allActors);

            /// @brief Отрисовать popup для добавления нового актёра.
            void RenderAddActorPopup();
    };
}
