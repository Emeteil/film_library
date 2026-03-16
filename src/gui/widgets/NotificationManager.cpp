/// @file NotificationManager.cpp
/// @brief Реализация менеджера уведомлений (toast).

#include "gui/widgets/NotificationManager.h"

#include <imgui.h>
#include <algorithm>

namespace FilmLibrary
{
    NotificationManager& NotificationManager::Instance()
    {
        static NotificationManager instance;
        return instance;
    }

    void NotificationManager::Push(NotificationType type, const std::string& message, float durationSec)
    {
        notifications.push_back({type, message, durationSec});
    }

    void NotificationManager::Render()
    {
        // TODO: Реализовать отрисовку уведомлений.
        //
        // 1. Получить deltaTime из ImGui::GetIO().DeltaTime.
        // 2. Для каждого уведомления:
        //    a. Уменьшить remainingTime на deltaTime.
        //    b. Если remainingTime <= 0 - пометить на удаление.
        //    c. Иначе - отрисовать ImGui::Begin("Notification_N", ...) с
        //       флагом ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize.
        //    d. Установить позицию в правом верхнем углу, стекая вниз.
        //    e. Выбрать цвет фона в зависимости от type.
        // 3. Удалить истёкшие (erase-remove idiom).
    }
}
