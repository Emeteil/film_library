/// @file GuiUtils.cpp

#include "gui/widgets/GuiUtils.h"
#include <imgui_internal.h>
#include <cmath>
#include <ctime>
#include <algorithm>

namespace FilmLibrary
{
    void GuiUtils::DrawCoverPlaceholder(ImVec2 pos, ImVec2 size, const char* title)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p1 = {pos.x + size.x, pos.y + size.y};
        dl->AddRectFilledMultiColor(pos, p1,
            IM_COL32(25, 20, 10, 255), IM_COL32(18, 15, 8, 255),
            IM_COL32(12, 10, 5, 255), IM_COL32(20, 16, 8, 255));
        dl->AddRect(pos, p1, IM_COL32(90, 70, 25, 120), 4.0f, 0, 1.5f);

        float cx = pos.x + size.x * 0.5f;
        float cy = pos.y + size.y * 0.42f;
        float r = std::min(size.x, size.y) * 0.18f;
        dl->AddCircle({cx, cy}, r, IM_COL32(90, 70, 25, 160), 32, 1.5f);
        dl->AddCircle({cx, cy}, r * 0.45f, IM_COL32(90, 70, 25, 160), 32, 1.0f);
        for (int i = 0; i < 6; ++i)
        {
            float a = (float)i / 6.0f * 6.2832f;
            float x1 = cx + cosf(a) * r * 0.6f;
            float y1 = cy + sinf(a) * r * 0.6f;
            float x2 = cx + cosf(a) * r * 0.85f;
            float y2 = cy + sinf(a) * r * 0.85f;
            dl->AddLine({x1, y1}, {x2, y2}, IM_COL32(90, 70, 25, 160), 1.0f);
        }

        float textY = pos.y + size.y * 0.72f;
        ImGui::SetCursorScreenPos({pos.x + 6, textY});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.80f));
        ImGui::PushTextWrapPos(pos.x + size.x - 6);
        ImGui::TextWrapped("%s", title ? title : "");
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
    }

    void GuiUtils::GoldSeparator()
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;
        ImGui::GetWindowDrawList()->AddLine(
            p, {p.x + w, p.y},
            IM_COL32(120, 90, 30, 180), 1.0f);
        ImGui::Dummy({0, 4});
    }

    void GuiUtils::SectionTitle(const char* text)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
        ImGui::Text("%s", text);
        ImGui::PopStyleColor();
    }

    void GuiUtils::DrawStars(float rating10, float scale)
    {
        float stars = rating10 / 2.0f;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 base = ImGui::GetCursorScreenPos();
        float sz = 13.0f * scale;
        float gap = 2.0f * scale;

        for (int i = 0; i < 5; ++i)
        {
            float x = base.x + (sz + gap) * i;
            float cx = x + sz * 0.5f;
            float cy = base.y + sz * 0.5f;
            float rOut = sz * 0.48f;
            float rIn = sz * 0.20f;
            ImU32 col = (float(i) < stars)
                ? IM_COL32(220, 175, 55, 255)
                : IM_COL32(50, 45, 35, 255);

            ImVec2 pts[10];
            for (int k = 0; k < 10; ++k)
            {
                float a = -1.5708f + k * 0.6283f;
                float r = (k % 2 == 0) ? rOut : rIn;
                pts[k] = {cx + cosf(a) * r, cy + sinf(a) * r};
            }
            dl->AddConvexPolyFilled(pts, 10, col);
        }
        ImGui::Dummy({(sz + gap) * 5, sz});
    }

    void GuiUtils::DrawGenreTag(const char* genre)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float tw = ImGui::CalcTextSize(genre).x;
        float pw = 10.0f;
        float ph = 4.0f;
        float tagW = tw + pw * 2;
        float tagH = ImGui::GetTextLineHeight() + ph * 2;
        ImVec2 p1 = {p.x + tagW, p.y + tagH};
        dl->AddRectFilled(p, p1, IM_COL32(55, 42, 12, 220), 4.0f);
        dl->AddRect(p, p1, IM_COL32(120, 90, 30, 180), 4.0f, 0, 1.0f);
        ImGui::SetCursorScreenPos({p.x + pw, p.y + ph});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
        ImGui::TextUnformatted(genre);
        ImGui::PopStyleColor();
        ImGui::SetCursorScreenPos({p.x, p.y});
        ImGui::Dummy({tagW + 6, tagH});
        ImGui::SameLine(0, 0);
    }

    std::string GuiUtils::FormatDuration(int secs)
    {
        int h = secs / 3600;
        int m = (secs % 3600) / 60;
        char buf[32];
        if (h > 0)
        {
            snprintf(buf, sizeof(buf), "%dч %02dм", h, m);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%dм", m);
        }
        return buf;
    }

    std::string GuiUtils::FormatDate(long int ts)
    {
        std::time_t t = (std::time_t)ts;
        struct tm* tmInfo = std::localtime(&t);
        if (!tmInfo) return "н/д";
        char buf[32];
        std::strftime(buf, sizeof(buf), "%d.%m.%Y", tmInfo);
        return buf;
    }

    bool GuiUtils::RenderSortButton(const char* popupId, const char** labels, int count, int& currentKey, bool& ascending)
    {
        bool changed = false;

        char btnLabel[64];
        if (currentKey == 0)
        {
            snprintf(btnLabel, sizeof(btnLabel), "Сортировка##%s", popupId);
        }
        else
        {
            snprintf(btnLabel, sizeof(btnLabel), "%s  %s##%s", labels[currentKey], ascending ? "ASC" : "DESC", popupId);
        }

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float btnW = 160.0f;
        float btnH = ImGui::GetFrameHeight();
        bool hov = ImGui::IsMouseHoveringRect(p, {p.x + btnW, p.y + btnH});
        bool act = ImGui::IsMouseDown(0) && hov;

        ImU32 bgCol = act ? IM_COL32(80, 60, 18, 255) : hov ? IM_COL32(55, 42, 12, 255) : IM_COL32(30, 23, 7, 255);
        ImU32 bdCol = (currentKey != 0) ? IM_COL32(160, 120, 40, 200) : IM_COL32(90, 68, 20, 160);

        dl->AddRectFilled(p, {p.x + btnW, p.y + btnH}, bgCol, 5.0f);
        dl->AddRect(p, {p.x + btnW, p.y + btnH}, bdCol, 5.0f, 0, 1.2f);

        const char* rawLabel = btnLabel;
        const char* hashPos = strstr(btnLabel, "##");
        std::string visText;
        if (hashPos)
        {
            visText.assign(btnLabel, hashPos - btnLabel);
        }
        else
        {
            visText = btnLabel;
        }

        ImVec2 tsz = ImGui::CalcTextSize(visText.c_str());
        ImU32 tcol = (currentKey != 0) ? IM_COL32(220, 175, 55, 255) : IM_COL32(160, 140, 90, 255);
        dl->AddText({p.x + (btnW - tsz.x) * 0.5f, p.y + (btnH - tsz.y) * 0.5f}, tcol, visText.c_str());

        ImGui::InvisibleButton(rawLabel, {btnW, btnH});
        if (ImGui::IsItemClicked())
        {
            ImGui::OpenPopup(popupId);
        }

        ImGui::SetNextWindowBgAlpha(0.97f);
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.50f, 0.38f, 0.12f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.40f, 0.30f, 0.10f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.30f, 0.22f, 0.07f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

        if (ImGui::BeginPopup(popupId))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
            ImGui::TextUnformatted("Сортировать по:");
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 2});
            ImDrawList* pdl = ImGui::GetWindowDrawList();
            ImVec2 sp = ImGui::GetCursorScreenPos();
            pdl->AddLine(sp, {sp.x + ImGui::GetContentRegionAvail().x, sp.y}, IM_COL32(100, 76, 24, 160), 1.0f);
            ImGui::Dummy({0, 4});

            bool noneSelected = (currentKey == 0);
            ImGui::PushStyleColor(ImGuiCol_Text, noneSelected ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f) : ImVec4(0.75f, 0.75f, 0.78f, 1.0f));
            if (ImGui::Selectable("  Без сортировки", noneSelected, 0, {160, 0}))
            {
                currentKey = 0;
                changed = true;
            }
            ImGui::PopStyleColor();

            ImGui::Dummy({0, 2});
            sp = ImGui::GetCursorScreenPos();
            pdl->AddLine(sp, {sp.x + ImGui::GetContentRegionAvail().x, sp.y}, IM_COL32(70, 54, 16, 120), 1.0f);
            ImGui::Dummy({0, 4});

            for (int i = 1; i < count; ++i)
            {
                bool sel = (currentKey == i);
                char item[80];
                if (sel)
                {
                    snprintf(item, sizeof(item), "  %s  %s", labels[i], ascending ? "ASC" : "DESC");
                }
                else
                {
                    snprintf(item, sizeof(item), "  %s", labels[i]);
                }

                ImGui::PushStyleColor(ImGuiCol_Text, sel ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f) : ImVec4(0.75f, 0.75f, 0.78f, 1.0f));
                if (ImGui::Selectable(item, sel, 0, {160, 0}))
                {
                    if (currentKey == i)
                    {
                        ascending = !ascending;
                    }
                    else
                    {
                        currentKey = i;
                        ascending = true;
                    }
                    changed = true;
                }
                ImGui::PopStyleColor();
            }
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(4);

        return changed;
    }
}