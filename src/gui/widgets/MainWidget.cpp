#include "gui/widgets/MainWidget.h"
#include "gui/widgets/HomeTab.h"
#include "gui/widgets/FilmsTab.h"
#include "gui/widgets/ActorsTab.h"
#include "gui/widgets/NotificationManager.h"
#include "gui/ImageLoader.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <im_anim.h>

namespace FilmLibrary
{
    MainWidget::MainWidget()
    {
        homeTab = std::make_unique<HomeTab>();
        filmsTab = std::make_unique<FilmsTab>();
        actorsTab = std::make_unique<ActorsTab>();
    }

    MainWidget::~MainWidget()
    {
        ImageLoader::Shutdown();
    }

    void MainWidget::RenderAppHeader()
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wpos = ImGui::GetWindowPos();
        float ww = ImGui::GetWindowWidth();
        float hh = 38.0f;

        dl->AddRectFilledMultiColor(wpos, {wpos.x + ww, wpos.y + hh}, IM_COL32(18, 14, 5, 255), IM_COL32(30, 22, 6, 255), IM_COL32(22, 16, 5, 255), IM_COL32(14, 10, 4, 255));

        const char* logo = "   FILMOTEKA   ";
        ImVec2 lsz = ImGui::CalcTextSize(logo);
        dl->AddText({wpos.x + (ww - lsz.x) * 0.5f, wpos.y + (hh - lsz.y) * 0.5f}, IM_COL32(190, 148, 50, 255), logo);

        dl->AddLine({wpos.x, wpos.y + hh}, {wpos.x + ww, wpos.y + hh}, IM_COL32(90, 68, 20, 200), 1.0f);

        ImGui::SetCursorScreenPos({wpos.x, wpos.y + hh});
        ImGui::Dummy({0, 0});
    }

    void MainWidget::RenderTabBar(float dt)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wpos = ImGui::GetWindowPos();
        float ww = ImGui::GetWindowWidth();
        float barH = 48.0f;

        dl->AddRectFilled(wpos, {wpos.x + ww, wpos.y + barH}, IM_COL32(10, 9, 6, 255));
        dl->AddLine({wpos.x, wpos.y + barH}, {wpos.x + ww, wpos.y + barH}, IM_COL32(80, 60, 20, 180), 1.5f);

        const char* tabLabels[] = {"  Главная  ", "  Фильмы  ", "  Актёры  "};
        float tabW = ww / 3.0f;

        for (int i = 0; i < 3; ++i)
        {
            float tx = wpos.x + tabW * i;

            ImGuiID tid = ImGui::GetID(tabLabels[i]);
            ImGuiID ch = ImHashStr("underline");
            float target = (state.activeTab == i) ? 1.0f : 0.0f;
            iam_ease_desc ez = {iam_ease_out_cubic, 0, 0, 0, 0};
            float under = iam_tween_float(tid, ch, target, 0.25f, ez, iam_policy_crossfade, dt, 0.0f);

            if (under > 0.01f)
            {
                dl->AddRectFilled({tx, wpos.y}, {tx + tabW, wpos.y + barH}, IM_COL32((int)(15 * under), (int)(12 * under), (int)(5 * under), (int)(255 * under)));
                dl->AddRectFilled({tx + tabW * (1.0f - under) * 0.5f, wpos.y + barH - 3.0f}, {tx + tabW * (1.0f - (1.0f - under) * 0.5f), wpos.y + barH}, IM_COL32(200, 155, 50, (int)(255 * under)), 2.0f);
            }

            ImVec4 textCol = (state.activeTab == i) ? ImVec4(0.90f, 0.72f, 0.28f, 1.0f) : ImVec4(0.55f, 0.55f, 0.60f, 1.0f);

            ImVec2 tsz = ImGui::CalcTextSize(tabLabels[i]);
            dl->AddText({tx + (tabW - tsz.x) * 0.5f, wpos.y + (barH - tsz.y) * 0.5f}, ImGui::ColorConvertFloat4ToU32(textCol), tabLabels[i]);

            ImGui::SetCursorScreenPos({tx, wpos.y});
            ImGui::PushID(i);
            ImGui::InvisibleButton("##tab", {tabW, barH});
            if (ImGui::IsItemClicked())
            {
                state.activeTab = i;
            }
            ImGui::PopID();
        }

        ImGui::SetCursorScreenPos({wpos.x, wpos.y + barH + 6});
        ImGui::Dummy({0, 0});
    }

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

        ImGui::Begin("##mainapp", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

        RenderAppHeader();
        RenderTabBar(dt);

        ImGui::SetCursorPos({0, ImGui::GetCursorPosY()});
        
        float contentW = ImGui::GetWindowWidth();
        float contentH = ImGui::GetContentRegionAvail().y;

        ImGui::BeginChild("##content", {contentW, contentH}, false);

        switch (state.activeTab)
        {
            case 0:
                homeTab->Render(controller, state, dt);
                break;
            case 1:
                filmsTab->Render(controller, state);
                break;
            case 2:
                actorsTab->Render(controller, state);
                break;
        }

        ImGui::EndChild();
        ImGui::End();
        ImGui::PopStyleVar(3);

        NotificationManager::Instance().Render();
    }
}