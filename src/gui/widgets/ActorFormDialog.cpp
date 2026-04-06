/// @file ActorFormDialog.cpp
/// @brief Add/Edit actors

#include "gui/widgets/ActorFormDialog.h"
#include "gui/widgets/NotificationManager.h"

#include <imgui.h>
#include <cstring>
#include <cstdio>
#include <ctime>

namespace FilmLibrary
{
    ActorFormDialog::ActorFormDialog() = default;
    ActorFormDialog::~ActorFormDialog() = default;

    bool ActorFormDialog::IsOpen() const { return isOpen; }

    void ActorFormDialog::SetOnSubmit(OnSubmitCallback cb) { onSubmit = std::move(cb); }

    void ActorFormDialog::OpenForAdd()
    {
        isEditMode    = false;
        editingActorId = -1;
        memset(nameBuffer,       0, sizeof(nameBuffer));
        memset(descriptionBuffer, 0, sizeof(descriptionBuffer));
        memset(photoBuffer,       0, sizeof(photoBuffer));
        birthDay   = 1;
        birthMonth = 1;
        birthYear  = 1990;
        isOpen     = true;
    }

    void ActorFormDialog::OpenForEdit(const Actor& actor)
    {
        isEditMode     = true;
        editingActorId = actor.id;
        FillFromActor(actor);
        isOpen = true;
    }

    void ActorFormDialog::FillFromActor(const Actor& actor)
    {
        snprintf(nameBuffer,       sizeof(nameBuffer),       "%s", actor.name.c_str());
        snprintf(descriptionBuffer, sizeof(descriptionBuffer), "%s", actor.description.c_str());
        snprintf(photoBuffer,       sizeof(photoBuffer),       "%s", actor.photo.c_str());

        if (actor.birthdate > 0)
        {
            time_t t = (time_t)actor.birthdate;
            struct tm tm_info;
            
#ifdef _WIN32
            localtime_s(&tm_info, &t);
#else
            localtime_r(&t, &tm_info);
#endif
            
            birthDay   = tm_info.tm_mday;
            birthMonth = tm_info.tm_mon + 1;
            birthYear  = tm_info.tm_year + 1900;
        }
        else
        {
            birthDay   = 1;
            birthMonth = 1;
            birthYear  = 1990;
        }
    }

    Actor ActorFormDialog::BuildActorFromBuffers() const
    {
        Actor a;
        a.id           = editingActorId;
        a.name         = nameBuffer;
        a.description  = descriptionBuffer;
        a.photo        = photoBuffer;

        struct tm tm_info = {};
        tm_info.tm_mday  = birthDay;
        tm_info.tm_mon   = birthMonth - 1;
        tm_info.tm_year  = birthYear - 1900;
        tm_info.tm_hour  = 0;
        tm_info.tm_min   = 0;
        tm_info.tm_sec   = 0;
        tm_info.tm_isdst = -1;

        time_t t = mktime(&tm_info);
        a.birthdate = (long int)t;

        return a;
    }

    std::string ActorFormDialog::Validate() const
    {
        if (nameBuffer[0] == '\0') return "Имя не может быть пустым";
        if (birthYear < 1800 || birthYear > 2100) return "Проверьте год рождения";
        if (birthMonth < 1 || birthMonth > 12) return "Проверьте месяц";
        if (birthDay < 1 || birthDay > 31) return "Проверьте день";
        return "";
    }

    static void FieldLabel(const char* label)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.85f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine(130);
    }

    void ActorFormDialog::Render()
    {
        if (!isOpen) return;

        ImGui::OpenPopup("##actor_form");

        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::SetNextWindowSize({520, 0});

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.09f, 0.06f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border,  ImVec4(0.28f, 0.20f, 0.07f, 1.00f));

        if (ImGui::BeginPopupModal("##actor_form", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::SetWindowFontScale(1.2f);
            ImGui::TextUnformatted(isEditMode ? "📏 Редактировать" : "➕ Добавить актёра");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 4});
            ImGui::GetWindowDrawList()->AddLine(
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + 470, ImGui::GetCursorScreenPos().y},
                IM_COL32(100, 76, 24, 180), 1.0f);
            ImGui::Dummy({0, 8});

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.14f, 0.13f, 0.08f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.18f, 0.16f, 0.09f, 1.0f));

            FieldLabel("Имя *");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer));

            FieldLabel("Дата рождения");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##day", &birthDay, 1, 5);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##month", &birthMonth, 1, 5);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputInt("##year", &birthYear, 1, 10);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.44f, 0.48f, 1.0f));
            ImGui::TextUnformatted("          День / Месяц / Год");
            ImGui::PopStyleColor();

            FieldLabel("Фото");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##photo", photoBuffer, sizeof(photoBuffer));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.44f, 0.48f, 1.0f));
            ImGui::TextUnformatted("          Путь к файлу или URL");
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 4});
            FieldLabel("Биография");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##desc", descriptionBuffer, sizeof(descriptionBuffer),
                {0, 150});

            ImGui::PopStyleColor(2);

            ImGui::Dummy({0, 12});

            std::string err = Validate();

            if (!err.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.30f, 0.25f, 1.0f));
                ImGui::TextUnformatted((err).c_str());
                ImGui::PopStyleColor();
                ImGui::Dummy({0, 4});
            }

            float bw = 150.0f;
            ImGui::SetCursorPosX((470.0f - (bw * 2 + 12)) * 0.5f);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.44f, 0.14f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.75f, 0.56f, 0.18f, 1.0f));

            bool canSubmit = err.empty();
            if (!canSubmit) ImGui::BeginDisabled();
            if (ImGui::Button(isEditMode ? "Сохранить" : "Добавить", {bw, 34}))
            {
                Actor a = BuildActorFromBuffers();
                if (onSubmit) onSubmit(a);
                isOpen = false;
                ImGui::CloseCurrentPopup();
            }
            if (!canSubmit) ImGui::EndDisabled();
            ImGui::PopStyleColor(3);

            ImGui::SameLine(0, 12);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.13f, 0.09f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.20f, 0.08f, 1.0f));
            if (ImGui::Button("Отмена", {bw, 34}))
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
        ImGui::PopStyleVar(3);
    }
}
