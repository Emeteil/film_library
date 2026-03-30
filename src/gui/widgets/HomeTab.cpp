#include "gui/widgets/HomeTab.h"
#include "gui/widgets/GuiUtils.h"
#include "gui/widgets/MainWidget.h"
#include "gui/Style.h"
#include "gui/ImageLoader.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <im_anim.h>
#include <algorithm>
#include <cstdlib>

namespace FilmLibrary
{
    void HomeTab::RebuildHomeStack(const std::vector<Movie*>& movies)
    {
        homeStack.clear();
        for (auto* m : movies)
        {
            homeStack.push_back(m->id);
        }

        for (int i = (int)homeStack.size() - 1; i > 0; --i)
        {
            int j = rand() % (i + 1);
            std::swap(homeStack[i], homeStack[j]);
        }
        homeIndex = 0;
    }

    void HomeTab::Render(AppController& ctrl, MainWidgetState& state, float dt)
    {
        const auto& movies = ctrl.GetDisplayMovies();
        (void)movies;

        if (homeNeedRefresh || homeStack.empty())
        {
            ctrl.ClearSearch();
            ctrl.ClearFilter();

            const auto& all = ctrl.GetDisplayMovies();
            RebuildHomeStack(all);
            homeNeedRefresh = false;
            if (homeStack.empty()) return;
        }

        if (homeStack.empty())
        {
            ImGui::TextDisabled("База данных пуста");
            return;
        }

        if (homeIndex < 0) homeIndex = 0;
        if (homeIndex >= (int)homeStack.size()) homeIndex = (int)homeStack.size() - 1;

        int targetId = homeStack[homeIndex];
        const Movie* movie = ctrl.GetMovieById(targetId);
        if (!movie)
        {
            homeNeedRefresh = true;
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32.0f, 24.0f));
        ImGui::BeginChild("##home_content", {0, 0}, ImGuiChildFlags_AlwaysUseWindowPadding, 0);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        float availW = ImGui::GetContentRegionAvail().x;
        float availH = ImGui::GetContentRegionAvail().y;

        float coverW = std::min(280.0f, availW * 0.30f);
        float coverH = coverW * 1.42f;

        ImGui::BeginGroup();

        uint32_t tex = ImageLoader::GetOrLoadTexture(movie->cover);
        ImVec2 coverPos = ImGui::GetCursorScreenPos();

        if (tex)
        {
            int w = 0;
            int h = 0;
            if (ImageLoader::GetTextureSize(movie->cover, w, h) && w > 0 && h > 0)
            {
                coverH = coverW * ((float)h / w);
            }

            ImGuiID texFadeId = ImGui::GetID(("cover_fade_" + std::to_string(tex) + "_" + std::to_string(state.homeVisitCounter)).c_str());
            iam_ease_desc ez = {iam_ease_out_sine, 0, 0, 0, 0};
            float alpha = iam_tween_float(texFadeId, ImHashStr("alpha"), 1.0f, 0.5f, ez, iam_policy_crossfade, dt, 0.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::Image((void*)(intptr_t)tex, {coverW, coverH});
            ImGui::PopStyleVar();
        }
        else
        {
            GuiUtils::DrawCoverPlaceholder(coverPos, {coverW, coverH}, movie->title.c_str());
            ImGui::Dummy({coverW, coverH});
        }

        ImGui::Dummy({0, 6});
        float starOffX = (coverW - (13.0f + 2.0f) * 5) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + starOffX);
        GuiUtils::DrawStars((float)movie->rating);
        
        char ratStr[32];
        snprintf(ratStr, sizeof(ratStr), "%.1f / 10", movie->rating);
        float ratW = ImGui::CalcTextSize(ratStr).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (coverW - ratW) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.9f));
        ImGui::TextUnformatted(ratStr);
        ImGui::PopStyleColor();

        ImGui::EndGroup();

        ImGui::SameLine(0, 32);

        ImGui::BeginGroup();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.96f, 0.96f, 0.96f, 1.0f));
        ImGui::SetWindowFontScale(1.35f);
        ImGui::PushTextWrapPos(0);
        ImGui::TextWrapped("%s", movie->title.c_str());
        ImGui::PopTextWrapPos();
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 4});
        GuiUtils::GoldSeparator();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.65f, 0.70f, 1.0f));
        ImGui::Text("%s  ·  %d  ·  %s", movie->studio.c_str(), movie->year, GuiUtils::FormatDuration(movie->length).c_str());
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 6});

        if (!movie->genres.empty())
        {
            ImGui::BeginGroup();
            for (size_t gi = 0; gi < movie->genres.size(); ++gi)
            {
                GuiUtils::DrawGenreTag(movie->genres[gi].c_str());
            }
            ImGui::EndGroup();
            ImGui::NewLine();
            ImGui::Dummy({0, 8});
        }

        GuiUtils::GoldSeparator();

        GuiUtils::SectionTitle("Описание");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.82f, 1.0f));
        float descH = std::min(120.0f, availH * 0.25f);
        ImGui::BeginChild("##desc", {0, descH}, false, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushTextWrapPos(0);
        ImGui::TextWrapped("%s", movie->description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 8});
        GuiUtils::GoldSeparator();

        if (!movie->actorIds.empty())
        {
            GuiUtils::SectionTitle("В ролях");
            ImGui::Dummy({0, 4});

            float actorCellSz = 86.0f;
            float actorAvatarSz = 72.0f;
            ImGui::BeginChild("##actors_row", {0, actorCellSz + ImGui::GetTextLineHeightWithSpacing() * 2 + 16}, false, ImGuiWindowFlags_HorizontalScrollbar);

            for (int actorId : movie->actorIds)
            {
                const Actor* actor = ctrl.GetActorById(actorId);
                if (!actor) continue;

                ImGui::BeginGroup();
                ImVec2 cellPos = ImGui::GetCursorScreenPos();
                
                float px = cellPos.x + (actorCellSz - actorAvatarSz) * 0.5f;
                float py = cellPos.y;

                uint32_t atex = ImageLoader::GetOrLoadTexture(actor->photo);
                if (atex)
                {
                    ImGuiID fadeId = ImGui::GetID(("actor_fade_" + std::to_string(atex)).c_str());
                    iam_ease_desc ez = {iam_ease_out_sine, 0, 0, 0, 0};
                    float alpha = iam_tween_float(fadeId, ImHashStr("alpha"), 1.0f, 0.5f, ez, iam_policy_crossfade, dt, 0.0f);

                    float aw = actorAvatarSz;
                    float ah = actorAvatarSz;
                    int tw = 0, th = 0;
                    if (ImageLoader::GetTextureSize(actor->photo, tw, th) && tw > 0 && th > 0)
                    {
                        if (tw > th) ah = aw * ((float)th / tw);
                        else aw = ah * ((float)tw / th);
                    }
                    float ix = px + (actorAvatarSz - aw) * 0.5f;
                    float iy = py + (actorAvatarSz - ah) * 0.5f;

                    ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)atex, {ix, iy}, {ix + aw, iy + ah}, {0,0}, {1,1}, IM_COL32(255, 255, 255, (int)(255 * alpha)));
                }
                else
                {
                    ImDrawList* adl = ImGui::GetWindowDrawList();
                    adl->AddCircleFilled({px + actorAvatarSz * 0.5f, py + actorAvatarSz * 0.5f}, actorAvatarSz * 0.5f, IM_COL32(35, 28, 10, 255));
                    adl->AddCircle({px + actorAvatarSz * 0.5f, py + actorAvatarSz * 0.5f}, actorAvatarSz * 0.5f, IM_COL32(90, 70, 25, 180), 32, 1.5f);

                    char initials[4] = "?";
                    if (!actor->name.empty())
                    {
                        initials[0] = actor->name[0];
                        initials[1] = '\0';
                        size_t sp = actor->name.find(' ');
                        if (sp != std::string::npos && sp + 1 < actor->name.size())
                        {
                            initials[1] = actor->name[sp + 1];
                            initials[2] = '\0';
                        }
                    }
                    ImVec2 iSz = ImGui::CalcTextSize(initials);
                    adl->AddText({px + actorAvatarSz * 0.5f - iSz.x * 0.5f, py + actorAvatarSz * 0.5f - iSz.y * 0.5f}, IM_COL32(200, 160, 60, 255), initials);
                }

                ImGui::SetCursorScreenPos(cellPos);
                ImGui::Dummy({actorCellSz, actorAvatarSz + ImGui::GetTextLineHeightWithSpacing() * 2});

                ImGui::SetCursorScreenPos(cellPos);
                ImGui::PushID(actorId);

                ImGuiID btnId = ImGui::GetID("##actor_btn");
                bool isHovered = ImGui::IsMouseHoveringRect(cellPos, {cellPos.x + actorCellSz, cellPos.y + actorAvatarSz + ImGui::GetTextLineHeightWithSpacing() * 2});
                float hovAlpha = iam_tween_float(btnId, ImHashStr("hov"), isHovered ? 1.0f : 0.0f, 0.2f, {iam_ease_out_quad, 0,0,0,0}, iam_policy_crossfade, dt, 0.0f);

                if (hovAlpha > 0.01f)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(cellPos, {cellPos.x + actorCellSz, cellPos.y + actorAvatarSz + ImGui::GetTextLineHeightWithSpacing() * 2}, IM_COL32(230, 184, 71, (int)(30 * hovAlpha)), 8.0f);
                }

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.9f, 0.72f, 0.28f, 0.15f));
                if (ImGui::Button("##actor_btn", {actorCellSz, actorAvatarSz + ImGui::GetTextLineHeightWithSpacing() * 2}))
                {
                    if (state.actorSelected != actorId) {
                        state.actorSelected = actorId;
                        state.actorVisitCounter++;
                    }
                    state.activeTab = 2;
                }
                ImGui::PopStyleColor(3);
                ImGui::PopID();

                std::string shortName = actor->name;
                size_t spacePos = shortName.find(' ');
                std::string firstLine = shortName;
                std::string secondLine = "";
                if (spacePos != std::string::npos) {
                    firstLine = shortName.substr(0, spacePos);
                    secondLine = shortName.substr(spacePos + 1);
                }

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.88f, 1.0f));
                float w1 = ImGui::CalcTextSize(firstLine.c_str()).x;
                ImGui::SetCursorScreenPos({cellPos.x + (actorCellSz - w1) * 0.5f, cellPos.y + actorAvatarSz + 4});
                ImGui::TextUnformatted(firstLine.c_str());
                
                if (!secondLine.empty()) {
                    float w2 = ImGui::CalcTextSize(secondLine.c_str()).x;
                    ImGui::SetCursorScreenPos({cellPos.x + (actorCellSz - w2) * 0.5f, cellPos.y + actorAvatarSz + 4 + ImGui::GetTextLineHeight()});
                    ImGui::TextUnformatted(secondLine.c_str());
                }
                ImGui::PopStyleColor();

                ImGui::EndGroup();
                ImGui::SameLine(0, 12);
            }
            ImGui::EndChild();
        }

        ImGui::EndGroup();

        ImGui::Dummy({0, 12});
        GuiUtils::GoldSeparator();
        ImGui::Dummy({0, 4});

        float btnW = 120.0f;
        float totalW = btnW * 2 + 16 + ImGui::CalcTextSize("0000 / 0000").x;
        float startX = (availW - totalW) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.38f, 0.12f, 1.0f));
        if (ImGui::Button("  < Назад  ", {btnW, 32}))
        {
            --homeIndex;
            if (homeIndex < 0) homeIndex = (int)homeStack.size() - 1;
            state.homeVisitCounter++;
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0, 8);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.60f, 1.0f));
        ImGui::Text("%d / %d", homeIndex + 1, (int)homeStack.size());
        ImGui::PopStyleColor();

        ImGui::SameLine(0, 8);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.38f, 0.12f, 1.0f));
        if (ImGui::Button("  Вперёд >  ", {btnW, 32}))
        {
            ++homeIndex;
            if (homeIndex >= (int)homeStack.size()) homeIndex = 0;
            state.homeVisitCounter++;
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0, 20);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.22f, 0.08f, 1.0f));
        if (ImGui::Button("Случайный", {120, 32}))
        {
            ctrl.ClearSearch();
            ctrl.ClearFilter();
            const auto& all2 = ctrl.GetDisplayMovies();
            RebuildHomeStack(all2);
            state.homeVisitCounter++;
        }
        ImGui::PopStyleColor(2);

        ImGui::PopStyleVar();
        
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
}