#pragma once

/// @file ConfirmDialog.h
/// @brief Модальное окно подтверждения действия (например, удаление фильма).

#include <functional>
#include <string>

namespace FilmLibrary
{
    class ConfirmDialog
    {
        public:
            /// @brief Callback, вызываемый при подтверждении.
            using OnConfirmCallback = std::function<void()>;

            ConfirmDialog();
            ~ConfirmDialog();

            /// @brief Показать окно подтверждения.
            /// @param title    Заголовок окна.
            /// @param message  Текст сообщения.
            /// @param onConfirm Callback при нажатии "Да".
            void Show(const std::string& title, const std::string& message, OnConfirmCallback onConfirm);

            /// @brief Отрисовать диалог (вызывать каждый кадр).
            void Render();

            bool IsOpen() const;

        private:
            bool isOpen = false;
            std::string dialogTitle;
            std::string dialogMessage;
            OnConfirmCallback onConfirm;
    };
}
