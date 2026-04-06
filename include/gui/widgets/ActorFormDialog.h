#pragma once

/// @file ActorFormDialog.h
/// @brief Модальное окно (диалог) для добавления / редактирования актёра.
///
/// Работает как popup в Dear ImGui.

#include "core/models/Actor.h"

#include <functional>
#include <string>

namespace FilmLibrary
{
    class ActorFormDialog
    {
        public:
            /// @brief Callback, вызываемый при подтверждении ввода.
            using OnSubmitCallback = std::function<void(const Actor& actor)>;

            ActorFormDialog();
            ~ActorFormDialog();

            /// @brief Открыть диалог для добавления нового актёра.
            void OpenForAdd();

            /// @brief Открыть диалог для редактирования существующего актёра.
            void OpenForEdit(const Actor& actor);

            /// @brief Отрисовать диалог (вызывать каждый кадр).
            void Render();

            /// @brief Установить callback, вызываемый при подтверждении.
            void SetOnSubmit(OnSubmitCallback callback);

            /// @brief Проверить, открыт ли диалог.
            bool IsOpen() const;

        private:
            bool isOpen = false;
            bool isEditMode = false;
            int editingActorId = -1;

            char nameBuffer[256] = {};
            char descriptionBuffer[2048] = {};
            char photoBuffer[512] = {};
            int birthDay = 1;
            int birthMonth = 1;
            int birthYear = 1990;

            OnSubmitCallback onSubmit;

            /// @brief Заполнить буферы из объекта Actor.
            void FillFromActor(const Actor& actor);

            /// @brief Собрать объект Actor из текущих буферов.
            Actor BuildActorFromBuffers() const;

            /// @brief Валидировать введённые данные.
            /// @return Текст ошибки или пустая строка при успехе.
            std::string Validate() const;
    };
}
