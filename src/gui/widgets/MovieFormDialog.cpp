/// @file MovieFormDialog.cpp
/// @brief Add/Edit films

#include "gui/widgets/MovieFormDialog.h"
#include "gui/widgets/NotificationManager.h"

#include <imgui.h>
#include <cstring>
#include <cstdio>
#include <sstream>

namespace FilmLibrary
{
    MovieFormDialog::MovieFormDialog()  = default;
    MovieFormDialog::~MovieFormDialog() = default;

    bool MovieFormDialog::IsOpen() const { return isOpen; }

    void MovieFormDialog::SetOnSubmit(OnSubmitCallback cb) { onSubmit = std::move(cb); }

    void MovieFormDialog::OpenForAdd()
    {
        isEditMode    = false;
        editingMovieId = -1;
        memset(titleBuffer,       0, sizeof(titleBuffer));
        memset(studioBuffer,      0, sizeof(studioBuffer));
        memset(descriptionBuffer, 0, sizeof(descriptionBuffer));
        memset(coverBuffer,       0, sizeof(coverBuffer));
        memset(streamLinkBuffer,  0, sizeof(streamLinkBuffer));
        memset(genresBuffer,      0, sizeof(genresBuffer));
        year          = 2024;
        lengthMinutes = 90;
        rating        = 7.0f;
        isOpen        = true;
        ImGui::OpenPopup("##movie_form");
    }

    void MovieFormDialog::OpenForEdit(const Movie& movie)
    {
        isEditMode     = true;
        editingMovieId = movie.id;
        FillFromMovie(movie);
        isOpen = true;
        ImGui::OpenPopup("##movie_form");
    }

    void MovieFormDialog::FillFromMovie(const Movie& movie)
    {
        snprintf(titleBuffer,       sizeof(titleBuffer),       "%s", movie.title.c_str());
        snprintf(studioBuffer,      sizeof(studioBuffer),      "%s", movie.studio.c_str());
        snprintf(descriptionBuffer, sizeof(descriptionBuffer), "%s", movie.description.c_str());
        snprintf(coverBuffer,       sizeof(coverBuffer),       "%s", movie.cover.c_str());
        snprintf(streamLinkBuffer,  sizeof(streamLinkBuffer),  "%s", movie.streamLink.c_str());

        std::string gs;
        for (size_t i = 0; i < movie.genres.size(); ++i)
        {
            if (i) gs += ", ";
            gs += movie.genres[i];
        }
        snprintf(genresBuffer, sizeof(genresBuffer), "%s", gs.c_str());

        year          = movie.year;
        lengthMinutes = movie.length / 60;
        rating        = (float)movie.rating;
    }

    Movie MovieFormDialog::BuildMovieFromBuffers() const
    {
        Movie m;
        m.id          = editingMovieId;
        m.title       = titleBuffer;
        m.studio      = studioBuffer;
        m.description = descriptionBuffer;
        m.cover       = coverBuffer;
        m.streamLink  = streamLinkBuffer;
        m.year        = year;
        m.length      = lengthMinutes * 60;
        m.rating      = (double)rating;

        std::istringstream ss(genresBuffer);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            size_t s = token.find_first_not_of(" \t");
            size_t e = token.find_last_not_of(" \t");
            if (s != std::string::npos)
                m.genres.push_back(token.substr(s, e - s + 1));
        }
        return m;
    }

    std::string MovieFormDialog::Validate() const
    {
        if (titleBuffer[0] == '\0') return "Название не может быть пустым";
        if (year < 1888 || year > 2100) return "Год должен быть от 1888 до 2100";
        if (rating < 0.0f || rating > 10.0f) return "Рейтинг должен быть от 0 до 10";
        return "";
    }

    static void FieldLabel(const char* label)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.85f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine(110);
    }

    void MovieFormDialog::Render()
    {
        if (!isOpen) return;

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

        if (ImGui::BeginPopupModal("##movie_form", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::SetWindowFontScale(1.2f);
            ImGui::TextUnformatted(isEditMode ? "✎  Редактировать фильм" : "＋  Добавить фильм");
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

            FieldLabel("Название *");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##title", titleBuffer, sizeof(titleBuffer));

            FieldLabel("Студия");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##studio", studioBuffer, sizeof(studioBuffer));

            FieldLabel("Год *");
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("##year", &year, 1, 10);

            FieldLabel("Длит. (мин)");
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("##length", &lengthMinutes, 1, 10);

            FieldLabel("Рейтинг");
            ImGui::SetNextItemWidth(200);
            ImGui::SliderFloat("##rating", &rating, 0.0f, 10.0f, "%.1f");
            ImGui::SameLine(0, 8);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::Text("★ %.1f", rating);
            ImGui::PopStyleColor();

            FieldLabel("Жанры");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##genres", genresBuffer, sizeof(genresBuffer));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.44f, 0.48f, 1.0f));
            ImGui::TextUnformatted("          Через запятую: Драма, Триллер");
            ImGui::PopStyleColor();

            FieldLabel("Обложка");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##cover", coverBuffer, sizeof(coverBuffer));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.44f, 0.48f, 1.0f));
            ImGui::TextUnformatted("          Путь к файлу или URL");
            ImGui::PopStyleColor();

            FieldLabel("Ссылка");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##stream", streamLinkBuffer, sizeof(streamLinkBuffer));

            ImGui::Dummy({0, 4});
            FieldLabel("Описание");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##desc", descriptionBuffer, sizeof(descriptionBuffer),
                {0, 90});

            ImGui::PopStyleColor(2);

            ImGui::Dummy({0, 12});

            std::string err = Validate();
            static std::string lastErr;

            if (!err.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.30f, 0.25f, 1.0f));
                ImGui::TextUnformatted(("⚠  " + err).c_str());
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
                Movie m = BuildMovieFromBuffers();
                if (onSubmit) onSubmit(m);
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
