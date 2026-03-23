/// @file MainWidget.cpp
/// @brief Реализация главного виджета (контейнера) для UI, возможно временно (возможно менять будем)

#include "gui/widgets/MainWidget.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <im_anim.h>
#include <random>
#include <chrono>
#include <ctime>
#include <string>
#include <cstdlib>
#include <thread>
#include <atomic>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../third_party/stb/stb_image.h"

#ifdef _WIN32
#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#endif

struct AsyncImageLoader {
    std::atomic<bool> downloading{false};
    std::atomic<bool> ready{false};
    std::atomic<bool> failed{false};
    unsigned char* data = nullptr;
    int w = 0, h = 0, channels = 0;
    GLuint texture = 0;

    void StartDownload(const char* url) {
        if (downloading || ready || texture || failed) return;
        downloading = true;

        std::thread([this, u = std::string(url)]() {
            const char* temp_file = "img_download_async.png";
            
        #ifdef _WIN32
            HRESULT hr = URLDownloadToFileA(NULL, u.c_str(), temp_file, 0, NULL);
            if (hr != S_OK) {
                this->failed = true;
                this->downloading = false;
                return;
            }
        #else
            std::string cmd = std::string("curl -s -L -o ") + temp_file + " \"" + u + "\"";
            std::system(cmd.c_str());
        #endif

            this->data = stbi_load(temp_file, &this->w, &this->h, &this->channels, 4);
            std::remove(temp_file);
            
            if (!this->data) {
                this->failed = true;
            } else {
                this->ready = true;
            }
            
            this->downloading = false;
        }).detach();
    }

    void UpdateGL() {
        if (ready && data && !texture) {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            data = nullptr;
            ready = false;
        }
    }
};

static AsyncImageLoader g_AsyncImg;

namespace FilmLibrary
{
    MainWidget::MainWidget() = default;
    MainWidget::~MainWidget() = default;

    void MainWidget::Render(AppController& controller, GLFWwindow* window)
    {
        iam_update_begin_frame();

        // тестовое окно с анимацией
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 580), ImGuiCond_FirstUseEver);
        ImGui::Begin("Animation Debug & Network Image", nullptr);
        
        float dt = ImGui::GetIO().DeltaTime;
        
        float wiggle_x = iam_wiggle(ImGui::GetID("wiggle_text"), 5.0f, 2.0f, dt);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + wiggle_x);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Animated Wiggle Text");
        ImGui::Spacing();
        
        float pulse = iam_oscillate(ImGui::GetID("pulse_btn"), 10.0f, 1.5f, iam_wave_sine, 0.0f, dt);
        if (ImGui::Button("Close App", ImVec2(150 + pulse, 30)))
            glfwSetWindowShouldClose(window, true);
        ImGui::Spacing();
        
        if (ImGui::Button("Trigger Shake")) {
            iam_trigger_shake(ImGui::GetID("shake_btn"));
        }
        float shake_x = iam_shake(ImGui::GetID("shake_btn"), 20.0f, 15.0f, 1.0f, dt);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + shake_x);
        ImGui::Button("Shaking Button");
        ImGui::Spacing();

        g_AsyncImg.StartDownload("https://i.pinimg.com/736x/21/51/67/2151677298c48a01365cbd31980a6797.jpg");
        g_AsyncImg.UpdateGL();

        if (g_AsyncImg.texture) {
            ImGui::Separator();
            ImGui::Text("Network Image:");
            ImGui::Image((void*)(intptr_t)g_AsyncImg.texture, ImVec2((float)200, (float)200));
        }

        ImGui::End();

        // поиск, фильтры
        ImGui::SetNextWindowPos(ImVec2(20, 230), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Search & Filters", nullptr);

        static char searchBuf[128] = "";
        ImGui::InputText("Query", searchBuf, sizeof(searchBuf));

        static int searchModeIdx = 0;
        const char* searchModes[] = { "Title", "Studio", "Year", "Description", "Genre" };
        ImGui::Combo("Mode", &searchModeIdx, searchModes, IM_ARRAYSIZE(searchModes));

        if (ImGui::Button("Search"))
        {
            AppController::SearchMode mode = static_cast<AppController::SearchMode>(searchModeIdx);
            controller.PerformSearch(mode, searchBuf);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Search"))
        {
            controller.ClearSearch();
            searchBuf[0] = '\0';
        }

        ImGui::Separator();

        static double ratingRange[2] = { 0.0, 10.0 };
        ImGui::DragScalarN("Rating Range", ImGuiDataType_Double, ratingRange, 2, 0.1f);
        if (ImGui::Button("Apply Rating Filter"))
        {
            controller.FilterByRatingRange(ratingRange[0], ratingRange[1]);
        }

        static int lengthRange[2] = { 0, 180 };
        ImGui::DragInt2("Length Range (min)", lengthRange, 1);
        if (ImGui::Button("Apply Length Filter"))
        {
            controller.FilterByLengthRange(lengthRange[0] * 60, lengthRange[1] * 60);
        }

        if (ImGui::Button("Clear All Filters"))
        {
            controller.ClearFilter();
        }

        ImGui::Separator();

        static int sortKeyIdx = 0;
        const char* sortKeys[] = { "Rating", "Year", "Title", "Length", "None" };
        ImGui::Combo("Sort Key", &sortKeyIdx, sortKeys, IM_ARRAYSIZE(sortKeys));
        static bool ascending = true;
        ImGui::Checkbox("Ascending", &ascending);
        if (ImGui::Button("Apply Sort"))
        {
            AppController::SortKey keys[] = {
                AppController::SortKey::Rating,
                AppController::SortKey::Year,
                AppController::SortKey::Title,
                AppController::SortKey::Length,
                AppController::SortKey::None
            };
            controller.SetSort(keys[sortKeyIdx], ascending);
        }

        ImGui::End();

        // поиск и сортировка актёров
        ImGui::SetNextWindowPos(ImVec2(20, 760), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Actor Search & Sort", nullptr);

        static char actorSearchBuf[128] = "";
        ImGui::InputText("Name Query", actorSearchBuf, sizeof(actorSearchBuf));
        if (ImGui::Button("Search Actors"))
        {
            controller.PerformActorSearch(actorSearchBuf);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear##Actor"))
        {
            controller.ClearActorSearch();
            actorSearchBuf[0] = '\0';
        }

        ImGui::Separator();

        static bool actorAscending = true;
        ImGui::Checkbox("Ascending##Actor", &actorAscending);
        if (ImGui::Button("Sort by Name"))
        {
            controller.SetActorSort(AppController::SortKey::Name, actorAscending);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Sort##Actor"))
        {
            controller.ClearActorSort();
        }

        ImGui::End();

        // фильмы
        ImGui::SetNextWindowPos(ImVec2(340, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Movies Table", nullptr);
        
        const auto& movies = controller.GetDisplayMovies();
        if (ImGui::BeginTable("movies_table", 11, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Title");
            ImGui::TableSetupColumn("Year");
            ImGui::TableSetupColumn("Rating");
            ImGui::TableSetupColumn("Length");
            ImGui::TableSetupColumn("Studio");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Cover");
            ImGui::TableSetupColumn("Stream Link");
            ImGui::TableSetupColumn("Genres");
            ImGui::TableSetupColumn("Actors");
            ImGui::TableHeadersRow();

            for (const auto* m : movies)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", m->id);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", m->title.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", m->year);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", m->rating);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d min", m->length / 60);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", m->studio.c_str());
                ImGui::TableSetColumnIndex(6); ImGui::Text("%s", m->description.c_str());
                ImGui::TableSetColumnIndex(7); ImGui::Text("%s", m->cover.c_str());
                ImGui::TableSetColumnIndex(8); ImGui::Text("%s", m->streamLink.c_str());
                
                std::string genresStr;
                for (size_t i = 0; i < m->genres.size(); ++i) {
                    genresStr += m->genres[i];
                    if (i < m->genres.size() - 1) genresStr += ", ";
                }
                ImGui::TableSetColumnIndex(9); ImGui::Text("%s", genresStr.c_str());
                ImGui::TableSetColumnIndex(10); ImGui::Text("%zu", m->actorIds.size());
            }
            ImGui::EndTable();
        }
        ImGui::End();

        // актёры
        ImGui::SetNextWindowPos(ImVec2(340, 430), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Actors Table", nullptr);

        const auto& actors = controller.GetDisplayActors();
        if (ImGui::BeginTable("actors_table", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Films Count");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Birthdate");
            ImGui::TableSetupColumn("Photo");
            ImGui::TableHeadersRow();

            for (const auto* a : actors)
            {
                if (!a) continue;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", a->id);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", a->name.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%zu", a->filmIds.size());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%s", a->description.c_str());
                
                std::time_t t = static_cast<std::time_t>(a->birthdate);
                struct tm* tm = std::localtime(&t);
                char dateBuf[32] = "N/A";
                if (tm) {
                    std::strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", tm);
                }
                ImGui::TableSetColumnIndex(4); ImGui::Text("%s", dateBuf);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%s", a->photo.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::End();

        static int frame_count = 0;
        if (++frame_count % 600 == 0) iam_gc();
    }
}