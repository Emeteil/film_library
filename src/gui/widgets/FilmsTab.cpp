#include "gui/widgets/FilmsTab.h"
#include "gui/widgets/GuiUtils.h"
#include "gui/widgets/MainWidget.h"
#include "gui/widgets/NotificationManager.h"
#include "gui/Style.h"
#include "gui/ImageLoader.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <im_anim.h>
#include <algorithm>

namespace FilmLibrary
{
    void FilmsTab::HandleMovieSubmit(const Movie& movie, AppController& ctrl, MainWidgetState& state)
    {
        if (movie.id >= 0)
        {
            bool ok = ctrl.UpdateMovie(movie.id, movie);
            if (ok)
            {
                NotificationManager::Instance().Push(
                    NotificationType::Success, "Фильм '" + movie.title + "' обновлён");
            }
            else
            {
                NotificationManager::Instance().Push(
                    NotificationType::Error, "Ошибка при обновждении фильма");
            }
        }
        else
        {
            Movie newMovie = movie;
            int newId = ctrl.AddMovie(std::move(newMovie));
            if (newId > 0)
            {
                state.filmSelected = newId;
                NotificationManager::Instance().Push(
                    NotificationType::Success, "Фильм '" + movie.title + "' добавлен");
            }
            else
            {
                NotificationManager::Instance().Push(
                    NotificationType::Error, "Ошибка при добавлении фильма");
            }
        }
    }

    void FilmsTab::HandleMovieDelete(int movieId, AppController& ctrl, MainWidgetState& state)
    {
        const Movie* movie = ctrl.GetMovieById(movieId);
        if (!movie) return;

        std::string title = movie->title;
        bool ok = ctrl.DeleteMovie(movieId);
        
        if (ok)
        {
            if (state.filmSelected == movieId)
            {
                state.filmSelected = -1;
            }
            NotificationManager::Instance().Push(
                NotificationType::Success, "Фильм '" + title + "' удалён");
        }
        else
        {
            NotificationManager::Instance().Push(
                NotificationType::Error, "Ошибка при удалении фильма");
        }
    }

    void FilmsTab::RenderFilmSearchRow(const Movie* m, bool selected)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float rowH = 64.0f;
        float w = ImGui::GetContentRegionAvail().x - 4;

        ImU32 bgCol = selected ? IM_COL32(55, 42, 12, 220) : IM_COL32(18, 17, 22, 200);
        ImU32 hoverCol = IM_COL32(35, 27, 8, 220);

        bool hovered = ImGui::IsMouseHoveringRect(p, {p.x + w, p.y + rowH});

        ImGuiID rowId = ImGui::GetID((void*)m);
        float hovAlpha = iam_tween_float(rowId, ImHashStr("hov"), hovered ? 1.0f : 0.0f, 0.15f, {iam_ease_out_quad, 0,0,0,0}, iam_policy_crossfade, ImGui::GetIO().DeltaTime, 0.0f);

        ImU32 currentBg = bgCol;
        if (!selected && hovAlpha > 0.01f)
        {
            int r = 18 + (int)((35 - 18) * hovAlpha);
            int g = 17 + (int)((27 - 17) * hovAlpha);
            int b = 22 + (int)((8 - 22) * hovAlpha);
            currentBg = IM_COL32(r, g, b, 200 + (int)(20 * hovAlpha));
        }

        dl->AddRectFilled(p, {p.x + w, p.y + rowH}, currentBg, 6.0f);

        if (selected)
        {
            float selAlpha = iam_tween_float(rowId + 1, ImHashStr("sel"), 1.0f, 0.2f, {iam_ease_out_cubic, 0,0,0,0}, iam_policy_crossfade, ImGui::GetIO().DeltaTime, 0.0f);
            dl->AddRect(p, {p.x + w, p.y + rowH}, IM_COL32(140, 105, 35, (int)(200 * selAlpha)), 6.0f, 0, 1.5f);
        }

        float thumbW = 42.0f;
        float thumbH = 60.0f;
        float thumbX = p.x + 8;
        float thumbY = p.y + 2;
        
        uint32_t tex = ImageLoader::GetOrLoadTexture(m->cover);
        if (tex)
        {
            int tw = 0;
            int th = 0;
            if (ImageLoader::GetTextureSize(m->cover, tw, th) && tw > 0 && th > 0)
            {
                thumbW = thumbH * ((float)tw / th);
            }
            dl->AddImage((void*)(intptr_t)tex, {thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH});
        }
        else
        {
            dl->AddRectFilled({thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH}, IM_COL32(30, 24, 10, 255), 3.0f);
            dl->AddRect({thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH}, IM_COL32(80, 62, 20, 150), 3.0f, 0, 1.0f);
        }

        float textX = p.x + 8 + 42.0f + 12;
        dl->AddText({textX, p.y + 8}, IM_COL32(230, 230, 235, 255), m->title.c_str());
        char meta[128];
        snprintf(meta, sizeof(meta), "%d  ·  %.1f  ·  %s", m->year, m->rating, m->studio.c_str());
        dl->AddText({textX, p.y + 28}, IM_COL32(120, 115, 125, 255), meta);

        if (!m->genres.empty())
        {
            std::string gs;
            for (size_t gi = 0; gi < m->genres.size() && gi < 3; ++gi)
            {
                if (gi) gs += ", ";
                gs += m->genres[gi];
            }
            dl->AddText({textX, p.y + 46}, IM_COL32(140, 105, 35, 220), gs.c_str());
        }

        ImGui::Dummy({w, rowH + 4});
    }

    void FilmsTab::RenderFilmDetail(const Movie* m, AppController& ctrl, MainWidgetState& state)
    {
        if (!m) 
        {
            ImGui::Dummy({0, 20});
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Выберите фильм из списка").x) * 0.5f);
            ImGui::TextDisabled("Выберите фильм из списка"); 
            return; 
        }

        float coverW = ImGui::GetContentRegionAvail().x * 0.55f;
        if (coverW > 240) coverW = 240;
        float coverH = coverW * 1.42f;

        uint32_t tex = ImageLoader::GetOrLoadTexture(m->cover);

        if (tex)
        {
            int tw = 0;
            int th = 0;
            if (ImageLoader::GetTextureSize(m->cover, tw, th) && tw > 0 && th > 0)
            {
                coverH = coverW * ((float)th / tw);
            }
        }

        float cx = (ImGui::GetContentRegionAvail().x - coverW) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);

        ImVec2 cp = ImGui::GetCursorScreenPos();
        if (tex)
        {
            ImGuiID fadeId = ImGui::GetID(("detail_fade_" + std::to_string(m->id) + "_" + std::to_string(state.filmVisitCounter)).c_str());
            float alpha = iam_tween_float(fadeId, ImHashStr("alpha"), 1.0f, 0.4f, {iam_ease_out_sine, 0,0,0,0}, iam_policy_crossfade, ImGui::GetIO().DeltaTime, 0.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::Image((void*)(intptr_t)tex, {coverW, coverH});
            ImGui::PopStyleVar();
        }
        else
        {
            GuiUtils::DrawCoverPlaceholder(cp, {coverW, coverH}, m->title.c_str());
            ImGui::Dummy({coverW, coverH});
        }

        ImGui::Dummy({0, 8});
        GuiUtils::GoldSeparator();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
        ImGui::SetWindowFontScale(1.18f);
        ImGui::PushTextWrapPos(0);
        ImGui::TextWrapped("%s", m->title.c_str());
        ImGui::PopTextWrapPos();
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.0f));
        ImGui::Text("%s · %d · %s", m->studio.c_str(), m->year, GuiUtils::FormatDuration(m->length).c_str());
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 4});
        ImGui::BeginGroup();
        GuiUtils::DrawStars((float)m->rating, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.9f));
        char ratStr[32];
        snprintf(ratStr, sizeof(ratStr), "%.1f / 10", m->rating);
        float starW = (13.0f + 2.0f) * 5;
        ImGui::SameLine(0, 10);
        ImGui::TextUnformatted(ratStr);
        ImGui::PopStyleColor();
        ImGui::EndGroup();

        ImGui::Dummy({0, 6});

        ImGui::BeginGroup();
        for (const auto& g : m->genres)
        {
            GuiUtils::DrawGenreTag(g.c_str());
        }
        ImGui::EndGroup();
        if (!m->genres.empty()) 
        {
            ImGui::NewLine(); 
            ImGui::Dummy({0, 4}); 
        }

        ImGui::Dummy({0, 8});
        GuiUtils::GoldSeparator();

        GuiUtils::SectionTitle("Описание");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.84f, 1.0f));
        ImGui::PushTextWrapPos(0);
        ImGui::TextWrapped("%s", m->description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();

        if (!m->actorIds.empty())
        {
            ImGui::Dummy({0, 8});
            GuiUtils::GoldSeparator();
            GuiUtils::SectionTitle("Актёры");
            ImGui::Dummy({0, 4});

            ImGui::PushTextWrapPos(0);
            for (int aid : m->actorIds)
            {
                const Actor* a = ctrl.GetActorById(aid);
                if (!a) continue;
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 1.0f));
                char fbtn[256];
                snprintf(fbtn, sizeof(fbtn), " %s ##ad%d", a->name.c_str(), aid);
                if (ImGui::SmallButton(fbtn))
                {
                    if (state.actorSelected != aid) {
                        state.actorSelected = aid;
                        state.actorVisitCounter++;
                    }
                    state.activeTab = 2;
                }
                ImGui::PopStyleColor();
            }
            ImGui::PopTextWrapPos();
            ImGui::NewLine();
        }

        ImGui::Dummy({0, 16});
        GuiUtils::GoldSeparator();
        ImGui::Dummy({0, 8});

        float btnW = 130.0f;
        float totalBtnW = btnW * 2 + 12;
        float btnX = (ImGui::GetContentRegionAvail().x - totalBtnW) * 0.5f;
        ImGui::SetCursorPosX(btnX);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.56f, 0.18f, 1.0f));
        if (ImGui::Button("Редактировать", {btnW, 34}))
        {
            movieFormDialog.OpenForEdit(*m, ctrl.GetAllActors());
            movieFormDialog.SetOnSubmit([&ctrl, this, &state](const Movie& movie) {
                HandleMovieSubmit(movie, ctrl, state);
            });
            movieFormDialog.SetOnAddActor([&ctrl](const Actor& actor) {
                return ctrl.AddActor(Actor(actor));
            });
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, 12);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.15f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.72f, 0.22f, 0.14f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.28f, 0.18f, 1.0f));
        if (ImGui::Button("Удалить", {btnW, 34}))
        {
            confirmDialog.Show("Удалить фильм", 
                "Вы уверены, что хотите удалить фильм '" + m->title + "'?",
                [&ctrl, this, &state, movieId = m->id]() {
                    HandleMovieDelete(movieId, ctrl, state);
                });
        }
        ImGui::PopStyleColor(3);
    }

    void FilmsTab::Render(AppController& ctrl, MainWidgetState& state)
    {
        ImGui::Dummy({0, 6});

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
        ImGui::BeginChild("##film_search_panel", {0, 64}, false, ImGuiWindowFlags_NoScrollbar);

        ImGui::SetCursorPos({24.0f, (64.0f - ImGui::GetFrameHeight()) * 0.5f});

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        bool searchChanged = ImGui::InputText("##fsq", filmSearch, sizeof(filmSearch));

        ImGui::SameLine(0, 8);
        const char* modes[] = {"Название", "Студия", "Год", "Описание", "Жанр"};
        ImGui::SetNextItemWidth(110);
        if (ImGui::Combo("##fsmode", &filmSearchMode, modes, 5))
        {
            searchChanged = true;
        }

        ImGui::SameLine(0, 8);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
        /*
        if (ImGui::Button("Найти", {70, 0}) || searchChanged)
        {
            std::string q(filmSearch);
            if (q.empty()) 
            {
                ctrl.ClearSearch();
            }
            else
            {
                auto mode = static_cast<AppController::SearchMode>(filmSearchMode);
                ctrl.PerformSearch(mode, q);
            }
        }
        */
        if (searchChanged)
        {
            std::string q(filmSearch);
            if (q.empty()) ctrl.ClearSearch();
            else ctrl.PerformSearch(static_cast<AppController::SearchMode>(filmSearchMode), q);
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0, 16);
        static const char* filmSortLabels[] = {"", "Название", "Год", "Рейтинг", "Длительность"};
        if (GuiUtils::RenderSortButton("##film_sort_popup", filmSortLabels, 5, filmSortKey, filmSortAsc))
        {
            using SK = AppController::SortKey;
            SK keys[] = {SK::None, SK::Title, SK::Year, SK::Rating, SK::Length};
            if (filmSortKey == 0) 
            {
                ctrl.ClearSort();
            }
            else
            {
                ctrl.SetSort(keys[filmSortKey], filmSortAsc);
            }
        }

        ImGui::SameLine(0, 6);
        
        ImGui::PushStyleColor(ImGuiCol_Button, showFilters ? ImVec4(0.50f, 0.38f, 0.12f, 1.0f) : ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.38f, 0.12f, 1.0f));
        if (ImGui::Button("Фильтры", {80, 0}))
        {
            ImGui::OpenPopup("##film_filters_popup");
        }
        ImGui::PopStyleColor(2);

        ImGui::SetNextWindowBgAlpha(0.97f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.50f, 0.38f, 0.12f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.20f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.85f, 0.40f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
        if (ImGui::BeginPopup("##film_filters_popup"))
        {
            showFilters = true;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::TextUnformatted("Фильтры:");
            ImGui::PopStyleColor();
            ImGui::Dummy({0, 4});

            bool changedFilters = false;

            ImGui::TextUnformatted("Рейтинг");
            if (ImGui::SliderFloat2("##rating", ratingRange, 0.0f, 10.0f, "%.1f"))
            {
                changedFilters = true;
            }

            ImGui::TextUnformatted("Длительность (мин)");
            int mins[2] = { lengthRange[0] / 60, lengthRange[1] / 60 };
            if (ImGui::SliderInt2("##duration", mins, 0, 600))
            {
                lengthRange[0] = mins[0] * 60;
                lengthRange[1] = mins[1] * 60;
                changedFilters = true;
            }

            if (changedFilters)
            {
                ctrl.FilterByRatingRange(ratingRange[0], ratingRange[1]);
                ctrl.FilterByLengthRange(lengthRange[0], lengthRange[1]);
            }

            ImGui::Dummy({0, 8});
            if (ImGui::Button("Очистить фильтры", {ImGui::GetContentRegionAvail().x, 0}))
            {
                ratingRange[0] = 0.0f;
                ratingRange[1] = 10.0f;
                lengthRange[0] = 0;
                lengthRange[1] = 600 * 60;
                ctrl.ClearFilter();
            }

            ImGui::EndPopup();
        }
        else
        {
            showFilters = false;
        }
        ImGui::PopStyleVar();

        ImGui::PopStyleColor(7);

        ImGui::SameLine(0, 6);
        if (ImGui::Button("X##fclr", {28, 0}))
        {
            filmSearch[0] = '\0';
            filmSortKey = 0;
            ratingRange[0] = 0.0f; ratingRange[1] = 10.0f;
            lengthRange[0] = 0; lengthRange[1] = 600 * 60;
            ctrl.ClearSearch();
            ctrl.ClearSort();
            ctrl.ClearFilter();
        }

        ImGui::SameLine(0, 16);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.56f, 0.18f, 1.0f));
        if (ImGui::Button("Добавить фильм", {140, 0}))
        {
            movieFormDialog.OpenForAdd();
            movieFormDialog.SetOnSubmit([&ctrl, this, &state](const Movie& movie) {
                HandleMovieSubmit(movie, ctrl, state);
            });
            movieFormDialog.SetOnAddActor([&ctrl](const Actor& actor) {
                return ctrl.AddActor(Actor(actor));
            });
        }
        ImGui::PopStyleColor(3);

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        GuiUtils::GoldSeparator();

        float totalH = ImGui::GetContentRegionAvail().y;
        float listW = ImGui::GetContentRegionAvail().x * 0.45f;
        float detailW = ImGui::GetContentRegionAvail().x - listW - 10;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
        ImGui::BeginChild("##film_list", {listW, totalH}, ImGuiChildFlags_AlwaysUseWindowPadding, 0);

        const auto& movies = ctrl.GetDisplayMovies();
        if (movies.empty())
        {
            ImGui::Dummy({0, 20});
            ImGui::SetCursorPosX((listW - ImGui::CalcTextSize("Ничего не найдено").x) * 0.5f);
            ImGui::TextDisabled("Ничего не найдено");
        }

        const Movie* selectedMovie = nullptr;

        for (const auto* m : movies)
        {
            bool sel = (m->id == state.filmSelected);
            if (sel) selectedMovie = m;

            ImGui::PushID(m->id);
            ImVec2 btnPos = ImGui::GetCursorScreenPos();
            float rowH = 68.0f;

            RenderFilmSearchRow(m, sel);

            ImGui::SetCursorScreenPos(btnPos);
            if (ImGui::InvisibleButton("##row", {listW - 4, rowH}))
            {
                if (state.filmSelected != m->id) {
                    state.filmSelected = m->id;
                    state.filmVisitCounter++;
                }
            }

            ImGui::PopID();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::SameLine(0, 10);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.11f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 24));
        ImGui::BeginChild("##film_detail", {detailW, totalH}, ImGuiChildFlags_AlwaysUseWindowPadding, 0);

        if (!selectedMovie && state.filmSelected >= 0)
        {
            selectedMovie = ctrl.GetMovieById(state.filmSelected);
        }

        RenderFilmDetail(selectedMovie, ctrl, state);

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        movieFormDialog.Render(ctrl.GetAllActors());
        confirmDialog.Render();
        ImGui::PopStyleVar();
    }
}