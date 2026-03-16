#pragma once

/// @file NotificationManager.h
/// @brief Менеджер всплывающих уведомлений (toast-нотификации) через ImGui.
///
/// Показывает временные сообщения об ошибках, предупреждениях и успешных действиях.

#include <string>
#include <vector>

namespace FilmLibrary
{
    enum class NotificationType
    {
        Info,
        Warning,
        Error,
        Success
    };

    class NotificationManager
    {
        public:
            /// @brief Получить единственный экземпляр.
            static NotificationManager& Instance();

            /// @brief Добавить уведомление для показа.
            /// @param type     Тип уведомления.
            /// @param message  Текст.
            /// @param durationSec Время показа в секундах.
            void Push(NotificationType type, const std::string& message, float durationSec = 3.0f);

            /// @brief Отрисовать все активные уведомления. Вызывать каждый кадр.
            void Render();

        private:
            NotificationManager() = default;
            ~NotificationManager() = default;

            NotificationManager(const NotificationManager&) = delete;
            NotificationManager& operator=(const NotificationManager&) = delete;

            struct Notification
            {
                NotificationType type;
                std::string message;
                float remainingTime;
            };

            std::vector<Notification> notifications;
    };
}
