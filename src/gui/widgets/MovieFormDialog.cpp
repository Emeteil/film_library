/// @file MovieFormDialog.cpp
/// @brief Add/Edit films

#include "gui/widgets/MovieFormDialog.h"
#include "gui/widgets/NotificationManager.h"
#include "core/models/Actor.h"

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
    
    void MovieFormDialog::SetOnAddActor(OnAddActorCallback cb) { onAddActor = std::move(cb); }

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
        memset(newActorNameBuffer, 0, sizeof(newActorNameBuffer));
        year          = 2024;
        lengthMinutes = 90;
        rating        = 7.0f;
        actorSelected.clear();
        showAddActorPopup = false;
        isOpen        = true;
    }

    void MovieFormDialog::OpenForEdit(const Movie& movie, const std::vector<std::unique_ptr<Actor>>& allActors)
    {
        isEditMode     = true;
        editingMovieId = movie.id;
        FillFromMovie(movie, allActors);
        isOpen = true;
    }

    void MovieFormDialog::FillFromMovie(const Movie& movie, const std::vector<std::unique_ptr<Actor>>& allActors)
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

        actorSelected.clear();
        for (size_t i = 0; i < allActors.size(); ++i)
        {
            bool hasActor = false;
            for (int aid : movie.actorIds)
            {
                if (allActors[i]->id == aid)
                {
                    hasActor = true;
                    break;
                }
            }
            actorSelected.push_back(hasActor);
        }
    }

    Movie MovieFormDialog::BuildMovieFromBuffers(const std::vector<std::unique_ptr<Actor>>& allActors) const
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
        
        // Собрать выбранные ID актёров
        for (size_t i = 0; i < allActors.size() && i < actorSelected.size(); ++i)
        {
            if (actorSelected[i])
            {
                m.actorIds.push_back(allActors[i]->id);
            }
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

    void MovieFormDialog::Render(const std::vector<std::unique_ptr<Actor>>& allActors)
    {
        if (!isOpen) return;

        if (actorSelected.size() != allActors.size())
        {
            std::vector<bool> oldSelected = actorSelected;
            actorSelected.clear();
            
            if (isEditMode && editingMovieId >= 0 && !oldSelected.empty())
            {
                if (oldSelected.size() <= allActors.size())
                {
                    actorSelected = oldSelected;
                    while (actorSelected.size() < allActors.size())
                    {
                        actorSelected.push_back(false);
                    }
                }
            }
            else
            {
                actorSelected.assign(allActors.size(), false);
            }
        }

        ImGui::OpenPopup("##movie_form");

        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::SetNextWindowSizeConstraints({520, 300}, {520, 600});

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.09f, 0.06f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border,  ImVec4(0.28f, 0.20f, 0.07f, 1.00f));

        if (ImGui::BeginPopupModal("##movie_form", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::SetWindowFontScale(1.2f);
            ImGui::TextUnformatted(isEditMode ? "📏  Редактировать фильм" : "➕  Добавить фильм");
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

            RenderActorSelection(allActors);

            ImGui::Dummy({0, 4});
            FieldLabel("Описание");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##desc", descriptionBuffer, sizeof(descriptionBuffer),
                {0, 90});

            ImGui::PopStyleColor(2);

            ImGui::Dummy({0, 12});

            std::string err = Validate();

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
                Movie m = BuildMovieFromBuffers(allActors);
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
            
            if (showAddActorPopup)
            {
                RenderAddActorPopup();
            }
        }
        else
        {
            isOpen = false;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
    }

    void MovieFormDialog::RenderActorSelection(const std::vector<std::unique_ptr<Actor>>& allActors)
    {
        FieldLabel("Актёры");
        
        ImGui::SetNextItemWidth(-1);
        
        if (ImGui::Button("Выбрать актёров...", {ImGui::GetContentRegionAvail().x, 0}))
        {
            ImGui::OpenPopup("##actor_selection_popup");
        }

        int selectedCount = 0;
        for (bool sel : actorSelected) {
            if (sel) selectedCount++;
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.44f, 0.48f, 1.0f));
        ImGui::TextUnformatted(selectedCount > 0 
            ? ("Выбрано актёров: " + std::to_string(selectedCount)).c_str()
            : "          Актёры не выбраны");
        ImGui::PopStyleColor();

        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        ImGui::SetNextWindowSize({400, 300});
        
        if (ImGui::BeginPopup("##actor_selection_popup"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::TextUnformatted("Выбор актёров:");
            ImGui::PopStyleColor();
            
            ImGui::Dummy({0, 8});
            
            ImGui::BeginChild("##actor_list", {0, 200}, true);
            
            for (size_t i = 0; i < allActors.size(); ++i)
            {
                const Actor& actor = *allActors[i];
                char label[320];
                snprintf(label, sizeof(label), "%s %s", 
                    actorSelected[i] ? "[*]" : "[  ]",
                    actor.name.c_str());
                
                if (ImGui::Selectable(label, actorSelected[i]))
                {
                    actorSelected[i] = !actorSelected[i];
                }
            }
            
            ImGui::EndChild();
            
            ImGui::Dummy({0, 8});
            
            // if (ImGui::Button("Добавить актёра", {ImGui::GetContentRegionAvail().x, 0}))
            // {
            //     memset(newActorNameBuffer, 0, sizeof(newActorNameBuffer));
            //     showAddActorPopup = true;
            //     ImGui::OpenPopup("##add_actor_popup");
            // }
            
            ImGui::Dummy({0, 4});
            
            if (ImGui::Button("Close", {ImGui::GetContentRegionAvail().x, 0}))
            {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
            
            // Render nested popup INSIDE movie_form modal
            if (showAddActorPopup)
            {
                RenderAddActorPopup();
            }
        }
    }

    void MovieFormDialog::RenderAddActorPopup()
    {
        if (!showAddActorPopup) return;

        ImGui::OpenPopup("##add_actor_popup");

        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::SetNextWindowSize({350, 0});

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 16));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.09f, 0.06f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border,  ImVec4(0.28f, 0.20f, 0.07f, 1.00f));

        if (ImGui::BeginPopupModal("##add_actor_popup", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::TextUnformatted("➕ Добавить Актёра");
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 8});
            ImGui::GetWindowDrawList()->AddLine(
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + 310, ImGui::GetCursorScreenPos().y},
                IM_COL32(100, 76, 24, 180), 1.0f);
            ImGui::Dummy({0, 8});

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.14f, 0.13f, 0.08f, 1.0f));
            
            ImGui::TextUnformatted("Имя актёра:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##new_actor_name", newActorNameBuffer, sizeof(newActorNameBuffer));

            ImGui::PopStyleColor();

            ImGui::Dummy({0, 12});

            float btnW = 130.0f;
            ImGui::SetCursorPosX((310.0f - (btnW * 2 + 12)) * 0.5f);

            bool canAdd = (newActorNameBuffer[0] != '\0');
            if (!canAdd) ImGui::BeginDisabled();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.44f, 0.14f, 1.0f));
            if (ImGui::Button("Добавить", {btnW, 32}))
            {
                Actor newActor;
                newActor.name = newActorNameBuffer;

                if (onAddActor)
                {
                    int newId = onAddActor(newActor);
                    if (newId > 0)
                    {
                        actorSelected.push_back(true);

                        NotificationManager::Instance().Push(
                            NotificationType::Success,
                            "Актёр '" + newActor.name + "' добавлен");
                    }
                }

                memset(newActorNameBuffer, 0, sizeof(newActorNameBuffer));
                showAddActorPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(2);
            
            if (!canAdd) ImGui::EndDisabled();

            ImGui::SameLine(0, 12);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.13f, 0.09f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.20f, 0.08f, 1.0f));
            if (ImGui::Button("Отмена", {btnW, 32}))
            {
                showAddActorPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(2);

            ImGui::EndPopup();
        }
        else
        {
            showAddActorPopup = false;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }
}
