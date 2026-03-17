/// @file ConfirmDialog.cpp
/// @brief Реализация модального окна подтверждения.

#include "gui/widgets/ConfirmDialog.h"

#include <imgui.h>

namespace FilmLibrary
{
    ConfirmDialog::ConfirmDialog() = default;
    ConfirmDialog::~ConfirmDialog() = default;

    void ConfirmDialog::Show(const std::string& title, const std::string& message, OnConfirmCallback onConfirmCb)
    {
        isOpen = true;
        dialogTitle = title;
        dialogMessage = message;
        onConfirm = std::move(onConfirmCb);
    }

    void ConfirmDialog::Render()
    {
        if (!isOpen)
        {
            return;
        }

        // TODO: Реализовать ImGui popup.
        //
        // 1. ImGui::OpenPopup(dialogTitle.c_str()).
        // 2. ImGui::BeginPopupModal(dialogTitle.c_str()).
        // 3. ImGui::Text(dialogMessage).
        // 4. Кнопка "Да" → onConfirm(), isOpen = false.
        // 5. Кнопка "Нет" → isOpen = false.
        // 6. ImGui::EndPopup().
    }

    bool ConfirmDialog::IsOpen() const
    {
        return isOpen;
    }
}
