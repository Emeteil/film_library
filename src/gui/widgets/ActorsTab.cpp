#include "gui/widgets/ActorsTab.h"
#include "gui/widgets/GuiUtils.h"
#include "gui/widgets/MainWidget.h"
#include "gui/Style.h"
#include "gui/ImageLoader.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <im_anim.h>
#include <algorithm>

namespace FilmLibrary
{
    void ActorsTab::RenderActorRow(const Actor* a, bool selected)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float rowH = 68.0f;
        float w = ImGui::GetContentRegionAvail().x;

        ImU32 bg = selected ? IM_COL32(55, 42, 12, 220) : IM_COL32(18, 17, 22, 200);
        bool hov = ImGui::IsMouseHoveringRect(p, {p.x + w, p.y + rowH});
        
        ImGuiID rowId = ImGui::GetID((void*)a);
        float hovAlpha = iam_tween_float(rowId, ImHashStr("hov"), hov ? 1.0f : 0.0f, 0.15f, {iam_ease_out_quad, 0,0,0,0}, iam_policy_crossfade, ImGui::GetIO().DeltaTime, 0.0f);

        ImU32 currentBg = bg;
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

        float az = 52.0f;
        float ax = p.x + 8;
        float ay = p.y + 8;
        uint32_t atex = ImageLoader::GetOrLoadTexture(a->photo);
        if (atex)
        {
            float aw = az;
            float ah = az;
            int tw = 0, th = 0;
            if (ImageLoader::GetTextureSize(a->photo, tw, th) && tw > 0 && th > 0)
            {
                aw = ah * ((float)tw / th);
            }
            dl->AddImage((void*)(intptr_t)atex, {ax, ay}, {ax + aw, ay + ah});
        }
        else
        {
            dl->AddCircleFilled({ax + az * 0.5f, ay + az * 0.5f}, az * 0.5f, IM_COL32(35, 28, 10, 255));
            dl->AddCircle({ax + az * 0.5f, ay + az * 0.5f}, az * 0.5f, IM_COL32(90, 70, 25, 180), 32, 1.5f);
            char init[4] = {'?', '\0', '\0', '\0'};
            if (!a->name.empty())
            {
                init[0] = a->name[0];
                init[1] = '\0';
                size_t sp = a->name.find(' ');
                if (sp != std::string::npos && sp + 1 < a->name.size())
                {
                    init[1] = a->name[sp + 1];
                    init[2] = '\0';
                }
            }
            ImVec2 isz = ImGui::CalcTextSize(init);
            dl->AddText({ax + az * 0.5f - isz.x * 0.5f, ay + az * 0.5f - isz.y * 0.5f}, IM_COL32(200, 160, 60, 255), init);
        }

        float tx = ax + az + 12;
        dl->AddText({tx, p.y + 10}, IM_COL32(225, 225, 230, 255), a->name.c_str());
        char meta[64];
        snprintf(meta, sizeof(meta), "Фильмов: %zu  ·  %s", a->filmIds.size(), GuiUtils::FormatDate(a->birthdate).c_str());
        dl->AddText({tx, p.y + 32}, IM_COL32(110, 105, 115, 255), meta);

        ImGui::Dummy({w, rowH + 4});
    }

    void ActorsTab::RenderActorDetail(const Actor* a, AppController& ctrl, MainWidgetState& state)
    {
        if (!a) 
        {
            ImGui::TextDisabled("Выберите актёра из списка");
            return;
        }

        float photoSz = std::min(160.0f, ImGui::GetContentRegionAvail().x * 0.6f);
        float px = (ImGui::GetContentRegionAvail().x - photoSz) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + px);

        uint32_t ptex = ImageLoader::GetOrLoadTexture(a->photo);
        ImVec2 pp = ImGui::GetCursorScreenPos();
        if (ptex)
        {
            ImGuiID fadeId = ImGui::GetID(("actor_detail_fade_" + std::to_string(ptex)).c_str());
            float alpha = iam_tween_float(fadeId, ImHashStr("alpha"), 1.0f, 0.4f, {iam_ease_out_sine, 0,0,0,0}, iam_policy_crossfade, ImGui::GetIO().DeltaTime, 0.0f);

            ImGui::GetWindowDrawList()->AddCircle({pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f}, photoSz * 0.5f + 2, IM_COL32(140, 105, 35, (int)(200 * alpha)), 64, 2.5f);
            
            float pw = photoSz;
            float ph = photoSz;
            int tw = 0, th = 0;
            if (ImageLoader::GetTextureSize(a->photo, tw, th) && tw > 0 && th > 0)
            {
                if (tw > th) ph = pw * ((float)th / tw);
                else pw = ph * ((float)tw / th);
            }
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (photoSz - pw) * 0.5f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::Image((void*)(intptr_t)ptex, {pw, ph});
            ImGui::PopStyleVar();
            
            ImGui::SetCursorPosX(px);
        }
        else
        {
            ImGui::GetWindowDrawList()->AddCircleFilled({pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f}, photoSz * 0.5f, IM_COL32(30, 24, 10, 255));
            ImGui::GetWindowDrawList()->AddCircle({pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f}, photoSz * 0.5f, IM_COL32(120, 90, 30, 200), 64, 2.0f);
            char init[4] = {a->name.empty() ? '?' : a->name[0], '\0', '\0', '\0'};
            if (!a->name.empty())
            {
                size_t sp = a->name.find(' ');
                if (sp != std::string::npos && sp + 1 < a->name.size())
                {
                    init[1] = a->name[sp + 1];
                    init[2] = '\0';
                }
            }
            ImGui::SetWindowFontScale(2.5f);
            ImVec2 isz = ImGui::CalcTextSize(init);
            ImGui::GetWindowDrawList()->AddText({pp.x + photoSz * 0.5f - isz.x * 0.5f, pp.y + photoSz * 0.5f - isz.y * 0.5f}, IM_COL32(200, 160, 60, 255), init);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Dummy({photoSz, photoSz});
        }

        ImGui::Dummy({0, 8});
        GuiUtils::GoldSeparator();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
        ImGui::SetWindowFontScale(1.18f);
        float nW = ImGui::CalcTextSize(a->name.c_str()).x;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - nW) * 0.5f);
        ImGui::TextUnformatted(a->name.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.0f));
        std::string bd = "Дата рождения: " + GuiUtils::FormatDate(a->birthdate);
        float bW = ImGui::CalcTextSize(bd.c_str()).x;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bW) * 0.5f);
        ImGui::TextUnformatted(bd.c_str());
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 8});
        GuiUtils::GoldSeparator();

        GuiUtils::SectionTitle("Биография");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.84f, 1.0f));
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
        ImGui::TextWrapped("%s", a->description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();

        if (!a->filmIds.empty())
        {
            ImGui::Dummy({0, 8});
            GuiUtils::GoldSeparator();
            GuiUtils::SectionTitle("Фильмография");
            ImGui::Dummy({0, 4});

            for (int fid : a->filmIds)
            {
                const Movie* fm = ctrl.GetMovieById(fid);
                if (!fm) continue;

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 1.0f));
                char fbtn[256];
                snprintf(fbtn, sizeof(fbtn), "  %s (%d)  ##fm%d", fm->title.c_str(), fm->year, fid);
                if (ImGui::SmallButton(fbtn))
                {
                    state.filmSelected = fid;
                    state.activeTab = 1;
                }
                ImGui::PopStyleColor();
            }
        }
    }

    void ActorsTab::Render(AppController& ctrl, MainWidgetState& state)
    {
        ImGui::Dummy({0, 6});

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
        ImGui::BeginChild("##actor_search_panel", {0, 52}, false);
        ImGui::Dummy({0, 8});

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.38f);
        bool changed = ImGui::InputText("##asq", actorSearch, sizeof(actorSearch));

        ImGui::SameLine(0, 8);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
        if (ImGui::Button("Найти", {70, 0}) || changed)
        {
            std::string q(actorSearch);
            if (q.empty()) 
            {
                ctrl.ClearActorSearch();
            }
            else
            {
                ctrl.PerformActorSearch(q);
            }
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0, 16);
        static const char* actorSortLabels[] = {"", "Имя", "Дата рождения", "Кол-во фильмов"};
        if (GuiUtils::RenderSortButton("##actor_sort_popup", actorSortLabels, 4, actorSortKey, actorSortAsc))
        {
            using SK = AppController::SortKey;
            if (actorSortKey == 0)
            {
                ctrl.ClearActorSort();
            }
            else if (actorSortKey == 1)
            {
                ctrl.SetActorSort(SK::Name,   actorSortAsc);
            }
            else if (actorSortKey == 2)
            {
                ctrl.SetActorSort(SK::Rating, actorSortAsc);
            }
            else if (actorSortKey == 3)
            {
                ctrl.SetActorSort(SK::Length, actorSortAsc);
            }
        }

        ImGui::SameLine(0, 6);
        if (ImGui::Button("X##aclr", {28, 0}))
        {
            actorSearch[0] = '\0';
            actorSortKey = 0;
            ctrl.ClearActorSearch();
            ctrl.ClearActorSort();
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        GuiUtils::GoldSeparator();

        float totalH = ImGui::GetContentRegionAvail().y;
        float listW = ImGui::GetContentRegionAvail().x * 0.42f;
        float detailW = ImGui::GetContentRegionAvail().x - listW - 10;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 1.0f));
        ImGui::BeginChild("##actor_list", {listW, totalH}, false);

        const auto& actors = ctrl.GetDisplayActors();
        if (actors.empty())
        {
            ImGui::Dummy({0, 20});
            ImGui::SetCursorPosX((listW - ImGui::CalcTextSize("Ничего не найдено").x) * 0.5f);
            ImGui::TextDisabled("Ничего не найдено");
        }

        const Actor* selectedActor = nullptr;

        for (const auto* a : actors)
        {
            if (!a) continue;
            bool sel = (a->id == state.actorSelected);
            if (sel) selectedActor = a;

            ImGui::PushID(a->id);
            ImVec2 bp = ImGui::GetCursorScreenPos();
            float rh = 72.0f;

            RenderActorRow(a, sel);
            ImGui::SetCursorScreenPos(bp);
            if (ImGui::InvisibleButton("##arow", {listW - 4, rh}))
            {
                state.actorSelected = a->id;
            }

            ImGui::PopID();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::SameLine(0, 10);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.11f, 1.0f));
        ImGui::BeginChild("##actor_detail", {detailW, totalH}, false);

        if (!selectedActor && state.actorSelected >= 0)
        {
            selectedActor = ctrl.GetActorById(state.actorSelected);
        }

        RenderActorDetail(selectedActor, ctrl, state);

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
}