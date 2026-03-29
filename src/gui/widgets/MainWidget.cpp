#include "gui/widgets/MainWidget.h"
#include "gui/widgets/NotificationManager.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <im_anim.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../third_party/stb/stb_image.h"

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <algorithm>

struct TexEntry
{
    std::atomic<bool> loading{false};
    std::atomic<bool> ready{false};
    std::atomic<bool> failed{false};
    unsigned char*    data     = nullptr;
    int               w = 0, h = 0;
    GLuint            tex      = 0;
    std::mutex        mtx;
    int               retries  = 0;
    static const int  MAX_RETRIES = 2;

    void StartLoad(const std::string& url)
    {
        if (loading || ready || tex) return;
        if (failed && retries >= MAX_RETRIES) return;
        failed  = false;
        loading = true;
        ++retries;

        std::thread([this, url]()
        {
            FILE* f = fopen(url.c_str(), "rb");
            if (f)
            {
                fclose(f);
                unsigned char* d = stbi_load(url.c_str(), &w, &h, nullptr, 4);
                std::lock_guard<std::mutex> lk(mtx);
                if (d) { data = d; ready = true; }
                else   { failed = true; }
            }
            else
            {
                char tmp[64];
                snprintf(tmp, sizeof(tmp), "fl_img_%p.png", (void*)this);
#ifdef _WIN32
                std::string cmd = "curl -s -L --max-time 10 -o \"" + std::string(tmp) + "\" \"" + url + "\"";
#else
                std::string cmd = "curl -s -L --max-time 10 -o " + std::string(tmp) + " \"" + url + "\"";
#endif
                int r = std::system(cmd.c_str());
                (void)r;
                unsigned char* d = stbi_load(tmp, &w, &h, nullptr, 4);
                std::remove(tmp);
                std::lock_guard<std::mutex> lk(mtx);
                if (d) { data = d; ready = true; }
                else   { failed = true; }
            }
            loading = false;
        }).detach();
    }

    void UploadGL()
    {
        std::lock_guard<std::mutex> lk(mtx);
        if (!ready || !data || tex) return;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        data  = nullptr;
        ready = false;
    }
};

static std::unordered_map<std::string, TexEntry*> g_TexCache;

static GLuint GetOrLoadTexture(const std::string& url)
{
    if (url.empty()) return 0;

    auto it = g_TexCache.find(url);
    if (it == g_TexCache.end())
    {
        TexEntry* e = new TexEntry();
        g_TexCache[url] = e;
        e->StartLoad(url);
        return 0;
    }
    TexEntry* e = it->second;
    e->UploadGL();
    return e->tex;
}

static void DrawCoverPlaceholder(ImVec2 pos, ImVec2 size, const char* title)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p1 = {pos.x + size.x, pos.y + size.y};
    dl->AddRectFilledMultiColor(pos, p1,
        IM_COL32(25, 20, 10, 255), IM_COL32(18, 15, 8, 255),
        IM_COL32(12, 10, 5, 255),  IM_COL32(20, 16, 8, 255));
    dl->AddRect(pos, p1, IM_COL32(90, 70, 25, 120), 4.0f, 0, 1.5f);

    float cx = pos.x + size.x * 0.5f;
    float cy = pos.y + size.y * 0.42f;
    float r  = std::min(size.x, size.y) * 0.18f;
    dl->AddCircle({cx, cy}, r, IM_COL32(90, 70, 25, 160), 32, 1.5f);
    dl->AddCircle({cx, cy}, r * 0.45f, IM_COL32(90, 70, 25, 160), 32, 1.0f);
    for (int i = 0; i < 6; ++i)
    {
        float a  = (float)i / 6.0f * 6.2832f;
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

static void GoldSeparator()
{
    ImVec2 p  = ImGui::GetCursorScreenPos();
    float  w  = ImGui::GetContentRegionAvail().x;
    ImGui::GetWindowDrawList()->AddLine(
        p, {p.x + w, p.y},
        IM_COL32(120, 90, 30, 180), 1.0f);
    ImGui::Dummy({0, 4});
}

static void SectionTitle(const char* text)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
    ImGui::Text("%s", text);
    ImGui::PopStyleColor();
}

static void DrawStars(float rating10, float scale = 1.0f)
{
    float stars = rating10 / 2.0f;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 base = ImGui::GetCursorScreenPos();
    float  sz   = 13.0f * scale;
    float  gap  = 2.0f * scale;

    for (int i = 0; i < 5; ++i)
    {
        float x  = base.x + (sz + gap) * i;
        float cx = x + sz * 0.5f, cy = base.y + sz * 0.5f;
        float r_out = sz * 0.48f, r_in = sz * 0.20f;
        ImU32 col = (float(i) < stars)
            ? IM_COL32(220, 175, 55, 255)
            : IM_COL32(50, 45, 35, 255);

        ImVec2 pts[10];
        for (int k = 0; k < 10; ++k)
        {
            float a = -1.5708f + k * 0.6283f;
            float r = (k % 2 == 0) ? r_out : r_in;
            pts[k] = {cx + cosf(a) * r, cy + sinf(a) * r};
        }
        dl->AddConvexPolyFilled(pts, 10, col);
    }
    ImGui::Dummy({(sz + gap) * 5, sz});
}

static void DrawGenreTag(const char* genre)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p  = ImGui::GetCursorScreenPos();
    float  tw = ImGui::CalcTextSize(genre).x;
    float  pw = 10.0f, ph = 4.0f;
    float  tagW = tw + pw * 2;
    float  tagH = ImGui::GetTextLineHeight() + ph * 2;
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

static std::string FormatDuration(int secs)
{
    int h = secs / 3600, m = (secs % 3600) / 60;
    char buf[32];
    if (h > 0) snprintf(buf, sizeof(buf), "%dч %02dм", h, m);
    else        snprintf(buf, sizeof(buf), "%dм", m);
    return buf;
}

static std::string FormatDate(long int ts)
{
    std::time_t t = (std::time_t)ts;
    struct tm* tm = std::localtime(&t);
    if (!tm) return "н/д";
    char buf[32];
    std::strftime(buf, sizeof(buf), "%d.%m.%Y", tm);
    return buf;
}

static int  g_activeTab       = 0;
static bool g_tabInitialized  = false;

static int               g_homeIndex  = 0;
static bool              g_homeNeedRefresh = true;
static std::vector<int>  g_homeStack;

static char              g_filmSearch[256] = {};
static int               g_filmSearchMode  = 0;
static int               g_filmSelected    = -1;

static char              g_actorSearch[256] = {};
static int               g_actorSelected   = -1;

static int               g_filmSortKey     = 0;
static bool              g_filmSortAsc     = true;

static int               g_actorSortKey    = 0;
static bool              g_actorSortAsc    = true;

static void RebuildHomeStack(const std::vector<FilmLibrary::Movie*>& movies)
{
    g_homeStack.clear();
    for (auto* m : movies) g_homeStack.push_back(m->id);

    for (int i = (int)g_homeStack.size() - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        std::swap(g_homeStack[i], g_homeStack[j]);
    }
    g_homeIndex = 0;
}

static void RenderHomeTab(FilmLibrary::AppController& ctrl, float dt)
{
    const auto& movies = ctrl.GetDisplayMovies();
    (void)movies;

    if (g_homeNeedRefresh || g_homeStack.empty())
    {
        ctrl.ClearSearch();
        ctrl.ClearFilter();

        const auto& all = ctrl.GetDisplayMovies();
        RebuildHomeStack(all);
        g_homeNeedRefresh = false;
        if (g_homeStack.empty()) return;
    }

    if (g_homeStack.empty()) { ImGui::TextDisabled("База данных пуста"); return; }

    if (g_homeIndex < 0) g_homeIndex = 0;
    if (g_homeIndex >= (int)g_homeStack.size()) g_homeIndex = (int)g_homeStack.size() - 1;

    int targetId = g_homeStack[g_homeIndex];
    const FilmLibrary::Movie* movie = ctrl.GetMovieById(targetId);
    if (!movie) { g_homeNeedRefresh = true; return; }

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    float avail_w = ImGui::GetContentRegionAvail().x;
    float avail_h = ImGui::GetContentRegionAvail().y;

    float coverW = std::min(280.0f, avail_w * 0.30f);
    float coverH = coverW * 1.42f;

    ImGui::BeginGroup();

    GLuint tex = GetOrLoadTexture(movie->cover);
    ImVec2 coverPos = ImGui::GetCursorScreenPos();

    if (tex)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            {coverPos.x + 6, coverPos.y + 6},
            {coverPos.x + coverW + 6, coverPos.y + coverH + 6},
            IM_COL32(0, 0, 0, 120), 8.0f);

        ImGui::Image((void*)(intptr_t)tex, {coverW, coverH});
    }
    else
    {
        DrawCoverPlaceholder(coverPos, {coverW, coverH}, movie->title.c_str());
        ImGui::Dummy({coverW, coverH});
    }

    ImGui::Dummy({0, 6});
    float starOffX = (coverW - (13.0f + 2.0f) * 5) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + starOffX);
    DrawStars((float)movie->rating);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + starOffX);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.9f));
    ImGui::Text("%.1f / 10", movie->rating);
    ImGui::PopStyleColor();

    ImGui::EndGroup();

    ImGui::SameLine(0, 20);

    ImGui::BeginGroup();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.96f, 0.96f, 0.96f, 1.0f));
    ImGui::SetWindowFontScale(1.35f);
    ImGui::TextWrapped("%s", movie->title.c_str());
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 4});
    GoldSeparator();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.65f, 0.70f, 1.0f));
    ImGui::Text("%s  ·  %d  ·  %s",
        movie->studio.c_str(),
        movie->year,
        FormatDuration(movie->length).c_str());
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 6});

    if (!movie->genres.empty())
    {
        ImGui::BeginGroup();
        for (size_t gi = 0; gi < movie->genres.size(); ++gi)
        {
            DrawGenreTag(movie->genres[gi].c_str());
        }
        ImGui::EndGroup();
        ImGui::NewLine();
        ImGui::Dummy({0, 8});
    }

    GoldSeparator();

    SectionTitle("Описание");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.82f, 1.0f));
    float descH = std::min(120.0f, avail_h * 0.25f);
    ImGui::BeginChild("##desc", {0, descH}, false, ImGuiWindowFlags_NoScrollbar);
    ImGui::PushTextWrapPos(0);
    ImGui::TextWrapped("%s", movie->description.c_str());
    ImGui::PopTextWrapPos();
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 8});
    GoldSeparator();

    if (!movie->actorIds.empty())
    {
        SectionTitle("В ролях");
        ImGui::Dummy({0, 4});

        float actorAvatarSz = 56.0f;
        float actorNameW    = 90.0f;

        ImGui::BeginChild("##actors_row",
            {0, actorAvatarSz + ImGui::GetTextLineHeightWithSpacing() * 2 + 12},
            false, ImGuiWindowFlags_HorizontalScrollbar);

        for (int actorId : movie->actorIds)
        {
            const FilmLibrary::Actor* actor = ctrl.GetActorById(actorId);
            if (!actor) continue;

            ImGui::BeginGroup();

            GLuint atex = GetOrLoadTexture(actor->photo);
            ImVec2 apos = ImGui::GetCursorScreenPos();

            if (atex)
            {
                ImGui::Image((void*)(intptr_t)atex, {actorAvatarSz, actorAvatarSz});
            }
            else
            {
                ImDrawList* adl = ImGui::GetWindowDrawList();
                adl->AddCircleFilled(
                    {apos.x + actorAvatarSz * 0.5f, apos.y + actorAvatarSz * 0.5f},
                    actorAvatarSz * 0.5f, IM_COL32(35, 28, 10, 255));
                adl->AddCircle(
                    {apos.x + actorAvatarSz * 0.5f, apos.y + actorAvatarSz * 0.5f},
                    actorAvatarSz * 0.5f, IM_COL32(90, 70, 25, 180), 32, 1.5f);

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
                adl->AddText(
                    {apos.x + actorAvatarSz * 0.5f - iSz.x * 0.5f,
                     apos.y + actorAvatarSz * 0.5f - iSz.y * 0.5f},
                    IM_COL32(200, 160, 60, 255), initials);

                ImGui::Dummy({actorAvatarSz, actorAvatarSz});
            }

            ImGui::SetCursorScreenPos(apos);
            ImGui::PushID(actorId);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.72f, 0.28f, 0.18f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.72f, 0.28f, 0.35f));
            if (ImGui::Button("##actor_btn", {actorAvatarSz, actorAvatarSz}))
            {
                g_actorSelected = actorId;
                g_activeTab     = 2;
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();

            std::string shortName = actor->name;
            if (shortName.size() > 14) shortName = shortName.substr(0, 13) + "…";
            ImGui::SetCursorPosX(ImGui::GetCursorPosX());
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.78f, 0.82f, 1.0f));
            ImGui::SetNextItemWidth(actorNameW);
            ImGui::TextUnformatted(shortName.c_str());
            ImGui::PopStyleColor();

            ImGui::EndGroup();
            ImGui::SameLine(0, 12);
        }
        ImGui::EndChild();
    }

    ImGui::EndGroup();

    ImGui::Dummy({0, 12});
    GoldSeparator();
    ImGui::Dummy({0, 4});

    float btnW = 120.0f;
    float totalW = btnW * 2 + 16 + ImGui::CalcTextSize("0000 / 0000").x;
    float startX = (avail_w - totalW) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);

    bool canPrev = (g_homeIndex > 0);
    bool canNext = (g_homeIndex < (int)g_homeStack.size() - 1);

    if (!canPrev)
        ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.38f, 0.12f, 1.0f));
    if (ImGui::Button("  < Назад  ", {btnW, 32}))
    {
        --g_homeIndex;
    }
    ImGui::PopStyleColor(2);
    if (!canPrev) ImGui::EndDisabled();

    ImGui::SameLine(0, 8);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.60f, 1.0f));
    ImGui::Text("%d / %d", g_homeIndex + 1, (int)g_homeStack.size());
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 8);

    if (!canNext)
        ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.12f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.38f, 0.12f, 1.0f));
    if (ImGui::Button("  Вперёд >  ", {btnW, 32}))
    {
        ++g_homeIndex;
    }
    ImGui::PopStyleColor(2);
    if (!canNext) ImGui::EndDisabled();

    ImGui::SameLine(0, 20);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.22f, 0.08f, 1.0f));
    if (ImGui::Button("Случайный", {120, 32}))
    {
        ctrl.ClearSearch();
        ctrl.ClearFilter();
        const auto& all2 = ctrl.GetDisplayMovies();
        RebuildHomeStack(all2);
    }
    ImGui::PopStyleColor(2);

    ImGui::PopStyleVar();
}

static void RenderFilmSearchRow(const FilmLibrary::Movie* m, bool selected)
{
    ImDrawList* dl  = ImGui::GetWindowDrawList();
    ImVec2 p        = ImGui::GetCursorScreenPos();
    float  rowH     = 64.0f;
    float  w        = ImGui::GetContentRegionAvail().x;

    ImU32 bgCol = selected
        ? IM_COL32(55, 42, 12, 220)
        : IM_COL32(18, 17, 22, 200);
    ImU32 hoverCol = IM_COL32(35, 27, 8, 220);

    bool hovered = ImGui::IsMouseHoveringRect(p, {p.x + w, p.y + rowH});
    dl->AddRectFilled(p, {p.x + w, p.y + rowH}, hovered ? hoverCol : bgCol, 6.0f);

    if (selected)
        dl->AddRect(p, {p.x + w, p.y + rowH}, IM_COL32(140, 105, 35, 200), 6.0f, 0, 1.5f);

    float thumbW = 42.0f, thumbH = 60.0f;
    float thumbX = p.x + 8, thumbY = p.y + 2;
    GLuint tex = GetOrLoadTexture(m->cover);
    if (tex)
        dl->AddImage((void*)(intptr_t)tex, {thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH});
    else
    {
        dl->AddRectFilled({thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH},
            IM_COL32(30, 24, 10, 255), 3.0f);
        dl->AddRect({thumbX, thumbY}, {thumbX + thumbW, thumbY + thumbH},
            IM_COL32(80, 62, 20, 150), 3.0f, 0, 1.0f);
    }

    float textX = thumbX + thumbW + 12;
    dl->AddText({textX, p.y + 8},
        IM_COL32(230, 230, 235, 255), m->title.c_str());
    char meta[128];
    snprintf(meta, sizeof(meta), "%d  ·  %.1f  ·  %s",
        m->year, m->rating, m->studio.c_str());
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

static void RenderFilmDetail(const FilmLibrary::Movie* m, FilmLibrary::AppController& ctrl)
{
    if (!m) { ImGui::TextDisabled("Выберите фильм из списка"); return; }

    float coverW = ImGui::GetContentRegionAvail().x * 0.55f;
    if (coverW > 240) coverW = 240;
    float coverH = coverW * 1.42f;

    GLuint tex = GetOrLoadTexture(m->cover);
    float cx = (ImGui::GetContentRegionAvail().x - coverW) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cx);

    ImVec2 cp = ImGui::GetCursorScreenPos();
    if (tex)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            {cp.x + 5, cp.y + 5}, {cp.x + coverW + 5, cp.y + coverH + 5},
            IM_COL32(0, 0, 0, 100), 6.0f);
        ImGui::Image((void*)(intptr_t)tex, {coverW, coverH});
    }
    else
    {
        DrawCoverPlaceholder(cp, {coverW, coverH}, m->title.c_str());
        ImGui::Dummy({coverW, coverH});
    }

    ImGui::Dummy({0, 8});
    GoldSeparator();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
    ImGui::SetWindowFontScale(1.18f);
    ImGui::PushTextWrapPos(0);
    ImGui::TextWrapped("%s", m->title.c_str());
    ImGui::PopTextWrapPos();
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.0f));
    ImGui::Text("%s · %d · %s", m->studio.c_str(), m->year, FormatDuration(m->length).c_str());
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 4});
    DrawStars((float)m->rating, 1.1f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 0.9f));
    ImGui::Text("%.1f / 10", m->rating);
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 6});

    for (const auto& g : m->genres)
        DrawGenreTag(g.c_str());
    if (!m->genres.empty()) { ImGui::NewLine(); ImGui::Dummy({0, 4}); }

    ImGui::Dummy({0, 8});
    GoldSeparator();

    SectionTitle("Описание");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.84f, 1.0f));
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
    ImGui::TextWrapped("%s", m->description.c_str());
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();

    if (!m->actorIds.empty())
    {
        ImGui::Dummy({0, 8});
        GoldSeparator();
        SectionTitle("Актёры");
        ImGui::Dummy({0, 4});
        for (int aid : m->actorIds)
        {
            const FilmLibrary::Actor* a = ctrl.GetActorById(aid);
            if (!a) continue;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 1.0f));
            char fbtn[256];
            snprintf(fbtn, sizeof(fbtn), "  %s  ##ad%d", a->name.c_str(), aid);
            if (ImGui::SmallButton(fbtn))
            {
                g_actorSelected = aid;
                g_activeTab     = 2;
            }
            ImGui::PopStyleColor();
        }
        ImGui::NewLine();
    }
}

static bool RenderSortButton(const char* popupId,
                             const char** labels, int count,
                             int& currentKey, bool& ascending)
{
    bool changed = false;

    char btnLabel[64];
    if (currentKey == 0)
        snprintf(btnLabel, sizeof(btnLabel), "Сортировка##%s", popupId);
    else
        snprintf(btnLabel, sizeof(btnLabel), "%s  %s##%s",
            labels[currentKey], ascending ? "ASC" : "DESC", popupId);

    ImDrawList* dl   = ImGui::GetWindowDrawList();
    ImVec2      p    = ImGui::GetCursorScreenPos();
    float       btnW = 160.0f;
    float       btnH = ImGui::GetFrameHeight();
    bool        hov  = ImGui::IsMouseHoveringRect(p, {p.x + btnW, p.y + btnH});
    bool        act  = ImGui::IsMouseDown(0) && hov;

    ImU32 bgCol = act ? IM_COL32(80, 60, 18, 255)
                : hov ? IM_COL32(55, 42, 12, 255)
                      : IM_COL32(30, 23, 7,  255);
    ImU32 bdCol = (currentKey != 0)
                ? IM_COL32(160, 120, 40, 200)
                : IM_COL32(90,  68, 20, 160);

    dl->AddRectFilled(p, {p.x + btnW, p.y + btnH}, bgCol, 5.0f);
    dl->AddRect      (p, {p.x + btnW, p.y + btnH}, bdCol, 5.0f, 0, 1.2f);

    const char* rawLabel = btnLabel;
    const char* hashPos  = strstr(btnLabel, "##");
    std::string visText;
    if (hashPos) {
        visText.assign(btnLabel, hashPos - btnLabel);
    } else {
        visText = btnLabel;
    }
    
    ImVec2 tsz = ImGui::CalcTextSize(visText.c_str());
    ImU32  tcol = (currentKey != 0)
                ? IM_COL32(220, 175, 55, 255)
                : IM_COL32(160, 140, 90, 255);
    dl->AddText({p.x + (btnW - tsz.x) * 0.5f, p.y + (btnH - tsz.y) * 0.5f}, tcol, visText.c_str());

    ImGui::InvisibleButton(rawLabel, {btnW, btnH});
    if (ImGui::IsItemClicked())
        ImGui::OpenPopup(popupId);

    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::PushStyleColor(ImGuiCol_PopupBg,       ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border,         ImVec4(0.50f, 0.38f, 0.12f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,  ImVec4(0.40f, 0.30f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header,         ImVec4(0.30f, 0.22f, 0.07f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8, 6));

    if (ImGui::BeginPopup(popupId))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.72f, 0.28f, 1.0f));
        ImGui::TextUnformatted("Сортировать по:");
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 2});
        ImDrawList* pdl = ImGui::GetWindowDrawList();
        ImVec2 sp = ImGui::GetCursorScreenPos();
        pdl->AddLine(sp, {sp.x + ImGui::GetContentRegionAvail().x, sp.y},
            IM_COL32(100, 76, 24, 160), 1.0f);
        ImGui::Dummy({0, 4});

        bool noneSelected = (currentKey == 0);
        ImGui::PushStyleColor(ImGuiCol_Text,
            noneSelected ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f) : ImVec4(0.75f, 0.75f, 0.78f, 1.0f));
        if (ImGui::Selectable("  Без сортировки", noneSelected, 0, {160, 0}))
        {
            currentKey = 0;
            changed    = true;
        }
        ImGui::PopStyleColor();

        ImGui::Dummy({0, 2});
        sp = ImGui::GetCursorScreenPos();
        pdl->AddLine(sp, {sp.x + ImGui::GetContentRegionAvail().x, sp.y},
            IM_COL32(70, 54, 16, 120), 1.0f);
        ImGui::Dummy({0, 4});

        for (int i = 1; i < count; ++i)
        {
            bool sel = (currentKey == i);
            char item[80];
            if (sel)
                snprintf(item, sizeof(item), "  %s  %s", labels[i], ascending ? "ASC" : "DESC");
            else
                snprintf(item, sizeof(item), "  %s", labels[i]);

            ImGui::PushStyleColor(ImGuiCol_Text,
                sel ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f) : ImVec4(0.75f, 0.75f, 0.78f, 1.0f));
            if (ImGui::Selectable(item, sel, 0, {160, 0}))
            {
                if (currentKey == i) ascending = !ascending;
                else { currentKey = i; ascending = true; }
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

static void RenderFilmsTab(FilmLibrary::AppController& ctrl)
{
    ImGui::Dummy({0, 6});

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
    ImGui::BeginChild("##film_search_panel", {0, 52}, false);
    ImGui::Dummy({0, 8});

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.32f);
    bool searchChanged = ImGui::InputText("##fsq", g_filmSearch, sizeof(g_filmSearch));

    ImGui::SameLine(0, 8);
    const char* modes[] = {"Название", "Студия", "Год", "Описание", "Жанр"};
    ImGui::SetNextItemWidth(110);
    if (ImGui::Combo("##fsmode", &g_filmSearchMode, modes, 5))
        searchChanged = true;

    ImGui::SameLine(0, 8);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
    if (ImGui::Button("Найти", {70, 0}) || searchChanged)
    {
        std::string q(g_filmSearch);
        if (q.empty()) ctrl.ClearSearch();
        else
        {
            auto mode = static_cast<FilmLibrary::AppController::SearchMode>(g_filmSearchMode);
            ctrl.PerformSearch(mode, q);
        }
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0, 16);
    static const char* filmSortLabels[] = {"", "Название", "Год", "Рейтинг", "Длительность"};
    if (RenderSortButton("##film_sort_popup", filmSortLabels, 5, g_filmSortKey, g_filmSortAsc))
    {
        using SK = FilmLibrary::AppController::SortKey;
        SK keys[] = {SK::None, SK::Title, SK::Year, SK::Rating, SK::Length};
        if (g_filmSortKey == 0) ctrl.ClearSort();
        else                    ctrl.SetSort(keys[g_filmSortKey], g_filmSortAsc);
    }

    ImGui::SameLine(0, 6);
    if (ImGui::Button("X##fclr", {28, 0}))
    {
        g_filmSearch[0] = '\0';
        g_filmSortKey   = 0;
        ctrl.ClearSearch();
        ctrl.ClearSort();
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleColor();

    GoldSeparator();

    float totalH  = ImGui::GetContentRegionAvail().y;
    float listW   = ImGui::GetContentRegionAvail().x * 0.48f;
    float detailW = ImGui::GetContentRegionAvail().x - listW - 10;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 1.0f));
    ImGui::BeginChild("##film_list", {listW, totalH}, false,
        ImGuiWindowFlags_HorizontalScrollbar);

    const auto& movies = ctrl.GetDisplayMovies();
    if (movies.empty())
    {
        ImGui::Dummy({0, 20});
        ImGui::SetCursorPosX((listW - ImGui::CalcTextSize("Ничего не найдено").x) * 0.5f);
        ImGui::TextDisabled("Ничего не найдено");
    }

    const FilmLibrary::Movie* selectedMovie = nullptr;

    for (const auto* m : movies)
    {
        bool sel = (m->id == g_filmSelected);
        if (sel) selectedMovie = m;

        ImGui::PushID(m->id);
        ImVec2 btnPos = ImGui::GetCursorScreenPos();
        float  rowH   = 68.0f;

        RenderFilmSearchRow(m, sel);

        ImGui::SetCursorScreenPos(btnPos);
        if (ImGui::InvisibleButton("##row", {listW - 4, rowH}))
            g_filmSelected = m->id;

        ImGui::PopID();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 10);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.11f, 1.0f));
    ImGui::BeginChild("##film_detail", {detailW, totalH}, false,
        ImGuiWindowFlags_HorizontalScrollbar);

    if (!selectedMovie && g_filmSelected >= 0)
        selectedMovie = ctrl.GetMovieById(g_filmSelected);

    RenderFilmDetail(selectedMovie, ctrl);

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

static void RenderActorRow(const FilmLibrary::Actor* a, bool selected)
{
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float rowH = 68.0f;
    float w = ImGui::GetContentRegionAvail().x;

    ImU32 bg = selected ? IM_COL32(55, 42, 12, 220) : IM_COL32(18, 17, 22, 200);
    bool hov = ImGui::IsMouseHoveringRect(p, {p.x + w, p.y + rowH});
    dl->AddRectFilled(p, {p.x + w, p.y + rowH},
        hov ? IM_COL32(35, 27, 8, 220) : bg, 6.0f);
    if (selected)
        dl->AddRect(p, {p.x + w, p.y + rowH}, IM_COL32(140, 105, 35, 200), 6.0f, 0, 1.5f);

    float az = 52.0f;
    float ax = p.x + 8, ay = p.y + 8;
    GLuint atex = GetOrLoadTexture(a->photo);
    if (atex)
        dl->AddImage((void*)(intptr_t)atex, {ax, ay}, {ax + az, ay + az});
    else
    {
        dl->AddCircleFilled({ax + az*0.5f, ay + az*0.5f}, az*0.5f, IM_COL32(35,28,10,255));
        dl->AddCircle({ax + az*0.5f, ay + az*0.5f}, az*0.5f, IM_COL32(90,70,25,180), 32, 1.5f);
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
        dl->AddText({ax + az*0.5f - isz.x*0.5f, ay + az*0.5f - isz.y*0.5f},
            IM_COL32(200,160,60,255), init);
    }

    float tx = ax + az + 12;
    dl->AddText({tx, p.y + 10}, IM_COL32(225, 225, 230, 255), a->name.c_str());
    char meta[64];
    snprintf(meta, sizeof(meta), "Фильмов: %zu  ·  %s",
        a->filmIds.size(), FormatDate(a->birthdate).c_str());
    dl->AddText({tx, p.y + 32}, IM_COL32(110, 105, 115, 255), meta);

    ImGui::Dummy({w, rowH + 4});
}

static void RenderActorDetail(const FilmLibrary::Actor* a, FilmLibrary::AppController& ctrl)
{
    if (!a) { ImGui::TextDisabled("Выберите актёра из списка"); return; }

    float photoSz = std::min(160.0f, ImGui::GetContentRegionAvail().x * 0.6f);
    float px = (ImGui::GetContentRegionAvail().x - photoSz) * 0.5f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + px);

    GLuint ptex = GetOrLoadTexture(a->photo);
    ImVec2 pp = ImGui::GetCursorScreenPos();
    if (ptex)
    {
        ImGui::GetWindowDrawList()->AddCircle(
            {pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f},
            photoSz * 0.5f + 2, IM_COL32(140, 105, 35, 200), 64, 2.5f);
        ImGui::Image((void*)(intptr_t)ptex, {photoSz, photoSz});
    }
    else
    {
        ImGui::GetWindowDrawList()->AddCircleFilled(
            {pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f},
            photoSz * 0.5f, IM_COL32(30, 24, 10, 255));
        ImGui::GetWindowDrawList()->AddCircle(
            {pp.x + photoSz * 0.5f, pp.y + photoSz * 0.5f},
            photoSz * 0.5f, IM_COL32(120, 90, 30, 200), 64, 2.0f);
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
        ImGui::GetWindowDrawList()->AddText(
            {pp.x + photoSz*0.5f - isz.x*0.5f, pp.y + photoSz*0.5f - isz.y*0.5f},
            IM_COL32(200,160,60,255), init);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Dummy({photoSz, photoSz});
    }

    ImGui::Dummy({0, 8});
    GoldSeparator();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
    ImGui::SetWindowFontScale(1.18f);
    float nW = ImGui::CalcTextSize(a->name.c_str()).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - nW) * 0.5f);
    ImGui::TextUnformatted(a->name.c_str());
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.0f));
    std::string bd = "Дата рождения: " + FormatDate(a->birthdate);
    float bW = ImGui::CalcTextSize(bd.c_str()).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bW) * 0.5f);
    ImGui::TextUnformatted(bd.c_str());
    ImGui::PopStyleColor();

    ImGui::Dummy({0, 8});
    GoldSeparator();

    SectionTitle("Биография");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.80f, 0.80f, 0.84f, 1.0f));
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
    ImGui::TextWrapped("%s", a->description.c_str());
    ImGui::PopTextWrapPos();
    ImGui::PopStyleColor();

    if (!a->filmIds.empty())
    {
        ImGui::Dummy({0, 8});
        GoldSeparator();
        SectionTitle("Фильмография");
        ImGui::Dummy({0, 4});

        for (int fid : a->filmIds)
        {
            const FilmLibrary::Movie* fm = ctrl.GetMovieById(fid);
            if (!fm) continue;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.58f, 0.22f, 1.0f));
            char fbtn[256];
            snprintf(fbtn, sizeof(fbtn), "  %s (%d)  ##fm%d", fm->title.c_str(), fm->year, fid);
            if (ImGui::SmallButton(fbtn))
            {
                g_filmSelected = fid;
                g_activeTab    = 1;
            }
            ImGui::PopStyleColor();
        }
    }
}

static void RenderActorsTab(FilmLibrary::AppController& ctrl)
{
    ImGui::Dummy({0, 6});

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.08f, 0.05f, 1.0f));
    ImGui::BeginChild("##actor_search_panel", {0, 52}, false);
    ImGui::Dummy({0, 8});

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.38f);
    bool changed = ImGui::InputText("##asq", g_actorSearch, sizeof(g_actorSearch));

    ImGui::SameLine(0, 8);
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.35f, 0.25f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.45f, 0.15f, 1.0f));
    if (ImGui::Button("Найти", {70, 0}) || changed)
    {
        std::string q(g_actorSearch);
        if (q.empty()) ctrl.ClearActorSearch();
        else           ctrl.PerformActorSearch(q);
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0, 16);
    static const char* actorSortLabels[] = {"", "Имя", "Дата рождения", "Кол-во фильмов"};
    if (RenderSortButton("##actor_sort_popup", actorSortLabels, 4, g_actorSortKey, g_actorSortAsc))
    {
        using SK = FilmLibrary::AppController::SortKey;
        if      (g_actorSortKey == 0) ctrl.ClearActorSort();
        else if (g_actorSortKey == 1) ctrl.SetActorSort(SK::Name,   g_actorSortAsc);
        else if (g_actorSortKey == 2) ctrl.SetActorSort(SK::Rating, g_actorSortAsc);
        else if (g_actorSortKey == 3) ctrl.SetActorSort(SK::Length, g_actorSortAsc);
    }

    ImGui::SameLine(0, 6);
    if (ImGui::Button("X##aclr", {28, 0}))
    {
        g_actorSearch[0] = '\0';
        g_actorSortKey   = 0;
        ctrl.ClearActorSearch();
        ctrl.ClearActorSort();
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleColor();

    GoldSeparator();

    float totalH  = ImGui::GetContentRegionAvail().y;
    float listW   = ImGui::GetContentRegionAvail().x * 0.42f;
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

    const FilmLibrary::Actor* selectedActor = nullptr;

    for (const auto* a : actors)
    {
        if (!a) continue;
        bool sel = (a->id == g_actorSelected);
        if (sel) selectedActor = a;

        ImGui::PushID(a->id);
        ImVec2 bp = ImGui::GetCursorScreenPos();
        float  rh = 72.0f;

        RenderActorRow(a, sel);
        ImGui::SetCursorScreenPos(bp);
        if (ImGui::InvisibleButton("##arow", {listW - 4, rh}))
            g_actorSelected = a->id;

        ImGui::PopID();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 10);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.11f, 1.0f));
    ImGui::BeginChild("##actor_detail", {detailW, totalH}, false);

    if (!selectedActor && g_actorSelected >= 0)
        selectedActor = ctrl.GetActorById(g_actorSelected);

    RenderActorDetail(selectedActor, ctrl);

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

static void RenderTabBar(float dt)
{
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      wpos  = ImGui::GetWindowPos();
    float       ww    = ImGui::GetWindowWidth();
    float       barH  = 48.0f;

    dl->AddRectFilled(wpos, {wpos.x + ww, wpos.y + barH}, IM_COL32(10, 9, 6, 255));
    dl->AddLine({wpos.x, wpos.y + barH}, {wpos.x + ww, wpos.y + barH},
        IM_COL32(80, 60, 20, 180), 1.5f);

    const char* tabLabels[] = {"  Главная  ", "  Фильмы  ", "  Актёры  "};
    float tabW = ww / 3.0f;

    for (int i = 0; i < 3; ++i)
    {
        float tx = wpos.x + tabW * i;

        ImGuiID tid = ImGui::GetID(tabLabels[i]);
        ImGuiID ch  = ImHashStr("underline");
        float target = (g_activeTab == i) ? 1.0f : 0.0f;
        iam_ease_desc ez = {iam_ease_out_cubic, 0,0,0,0};
        float under = iam_tween_float(tid, ch, target, 0.25f, ez,
            iam_policy_crossfade, dt, 0.0f);

        if (under > 0.01f)
        {
            dl->AddRectFilled({tx, wpos.y}, {tx + tabW, wpos.y + barH},
                IM_COL32((int)(15*under), (int)(12*under), (int)(5*under), (int)(255*under)));
            dl->AddRectFilled(
                {tx + tabW * (1.0f - under) * 0.5f, wpos.y + barH - 3.0f},
                {tx + tabW * (1.0f - (1.0f - under) * 0.5f), wpos.y + barH},
                IM_COL32(200, 155, 50, (int)(255 * under)), 2.0f);
        }

        ImVec4 textCol = (g_activeTab == i)
            ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f)
            : ImVec4(0.55f, 0.55f, 0.60f, 1.0f);

        ImVec2 tsz = ImGui::CalcTextSize(tabLabels[i]);
        dl->AddText({tx + (tabW - tsz.x) * 0.5f, wpos.y + (barH - tsz.y) * 0.5f},
            ImGui::ColorConvertFloat4ToU32(textCol), tabLabels[i]);

        ImGui::SetCursorScreenPos({tx, wpos.y});
        ImGui::PushID(i);
        ImGui::InvisibleButton("##tab", {tabW, barH});
        if (ImGui::IsItemClicked()) g_activeTab = i;
        ImGui::PopID();
    }

    ImGui::SetCursorScreenPos({wpos.x, wpos.y + barH + 6});
    ImGui::Dummy({0, 0});
}

static void RenderAppHeader()
{
    ImDrawList* dl   = ImGui::GetWindowDrawList();
    ImVec2      wpos = ImGui::GetWindowPos();
    float       ww   = ImGui::GetWindowWidth();
    float       hh   = 38.0f;

    dl->AddRectFilledMultiColor(
        wpos, {wpos.x + ww, wpos.y + hh},
        IM_COL32(18, 14, 5, 255), IM_COL32(30, 22, 6, 255),
        IM_COL32(22, 16, 5, 255), IM_COL32(14, 10, 4, 255));

    const char* logo = "   FILMOTEKA   ";
    ImVec2 lsz = ImGui::CalcTextSize(logo);
    dl->AddText({wpos.x + (ww - lsz.x) * 0.5f, wpos.y + (hh - lsz.y) * 0.5f},
        IM_COL32(190, 148, 50, 255), logo);

    dl->AddLine({wpos.x, wpos.y + hh}, {wpos.x + ww, wpos.y + hh},
        IM_COL32(90, 68, 20, 200), 1.0f);

    ImGui::SetCursorScreenPos({wpos.x, wpos.y + hh});
    ImGui::Dummy({0, 0});
}

namespace FilmLibrary
{
    MainWidget::MainWidget()  = default;
    MainWidget::~MainWidget() = default;

    void MainWidget::Render(AppController& controller, GLFWwindow* window)
    {
        (void)window;
        iam_update_begin_frame();

        float dt = ImGui::GetIO().DeltaTime;

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowBgAlpha(1.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("##mainapp", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove       |
            ImGuiWindowFlags_NoResize     |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        RenderAppHeader();

        RenderTabBar(dt);

        float padX = 18.0f, padY = 10.0f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
        float contentW = ImGui::GetWindowWidth() - padX * 2;
        float contentH = ImGui::GetContentRegionAvail().y - padY;

        ImGui::BeginChild("##content", {contentW, contentH}, false);

        switch (g_activeTab)
        {
        case 0:
            RenderHomeTab(controller, dt);
            break;
        case 1:
            RenderFilmsTab(controller);
            break;
        case 2:
            RenderActorsTab(controller);
            break;
        }

        ImGui::EndChild();

        ImGui::End();
        ImGui::PopStyleVar(3);

        NotificationManager::Instance().Render();

        static int s_frame = 0;
        if (++s_frame % 600 == 0) iam_gc();
    }
}