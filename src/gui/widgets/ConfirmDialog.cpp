/// @file ConfirmDialog.cpp
/// @brief Modals confirm

#include "gui/widgets/ConfirmDialog.h"

#include <imgui.h>

namespace FilmLibrary
{
    ConfirmDialog::ConfirmDialog()  = default;
    ConfirmDialog::~ConfirmDialog() = default;

    void ConfirmDialog::Show(const std::string& title,
                             const std::string& message,
                             OnConfirmCallback  cb)
    {
        dialogTitle   = title;
        dialogMessage = message;
        onConfirm     = std::move(cb);
        isOpen        = true;
    }

    bool ConfirmDialog::IsOpen() const { return isOpen; }

    void ConfirmDialog::Render()
    {
        if (!isOpen) return;
\
        ImGui::OpenPopup(dialogTitle.c_str());

        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, {0.5f, 0.5f});

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(28, 24));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.09f, 0.06f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border,  ImVec4(0.30f, 0.22f, 0.08f, 1.00f));

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize    |
            ImGuiWindowFlags_NoMove      |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_AlwaysAutoResize;

        if (ImGui::BeginPopupModal(dialogTitle.c_str(), nullptr, flags))
        {
            // Иконка предупреждения
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.65f, 0.20f, 1.0f));
            ImGui::SetWindowFontScale(1.6f);
            ImGui::Text("!");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::SameLine(0, 12);

            // Заголовок
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f, 0.92f, 0.94f, 1.0f));
            ImGui::SetWindowFontScale(1.15f);
            ImGui::TextUnformatted(dialogTitle.c_str());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 8});

            // Разделитель
            ImGui::GetWindowDrawList()->AddLine(
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + 340, ImGui::GetCursorScreenPos().y},
                IM_COL32(90, 68, 22, 180), 1.0f);
            ImGui::Dummy({0, 8});

            // Сообщение
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.84f, 1.0f));
            ImGui::PushTextWrapPos(340.0f);
            ImGui::TextWrapped("%s", dialogMessage.c_str());
            ImGui::PopTextWrapPos();
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 16});

            // Кнопки
            float btnW = 130.0f;
            ImGui::SetCursorPosX((340.0f - (btnW * 2 + 12)) * 0.5f);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.15f, 0.10f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.72f, 0.22f, 0.14f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.85f, 0.28f, 0.18f, 1.0f));
            if (ImGui::Button("Да, удалить", {btnW, 34}))
            {
                if (onConfirm) onConfirm();
                isOpen = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine(0, 12);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.13f, 0.09f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.22f, 0.08f, 1.0f));
            if (ImGui::Button("Отмена", {btnW, 34}))
            {
                isOpen = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(2);

            ImGui::EndPopup();
        }
        else
        {
            isOpen = false;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }
}
