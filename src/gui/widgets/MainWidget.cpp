/// @file MainWidget.cpp
/// @brief Реализация главного виджета (контейнера) для UI, возможно временно (возможно менять будем)

#include "gui/widgets/MainWidget.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <im_anim.h>
#include <random>
#include <chrono>

namespace FilmLibrary
{
    MainWidget::MainWidget() = default;
    MainWidget::~MainWidget() = default;

    void MainWidget::Render(AppController& controller, GLFWwindow* window)
    {
        iam_update_begin_frame();

        // тестовое окно с анимацией
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Animation Debug", nullptr);
        
        float dt = ImGui::GetIO().DeltaTime;
        float wiggle_x = iam_wiggle(ImGui::GetID("wiggle_text"), 5.0f, 2.0f, dt);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + wiggle_x);
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Animated Debug Text");
        
        float pulse = iam_oscillate(ImGui::GetID("pulse_btn"), 10.0f, 1.5f, iam_wave_sine, 0.0f, dt);
        if (ImGui::Button("Close App", ImVec2(150 + pulse, 30)))
            glfwSetWindowShouldClose(window, true);
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
        if (ImGui::BeginTable("movies_table", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Title");
            ImGui::TableSetupColumn("Year");
            ImGui::TableSetupColumn("Rating");
            ImGui::TableSetupColumn("Length");
            ImGui::TableHeadersRow();

            for (const auto* m : movies)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", m->id);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", m->title.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", m->year);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", m->rating);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d min", m->length / 60);
            }
            ImGui::EndTable();
        }
        ImGui::End();

        // актёры
        ImGui::SetNextWindowPos(ImVec2(340, 430), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Actors Table", nullptr);

        const auto& actors = controller.GetDisplayActors();
        if (ImGui::BeginTable("actors_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Films Count");
            ImGui::TableHeadersRow();

            for (const auto* a : actors)
            {
                if (!a) continue;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%d", a->id);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", a->name.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%zu", a->filmIds.size());
            }
            ImGui::EndTable();
        }
        ImGui::End();

        // тестовая генерация
        ImGui::SetNextWindowPos(ImVec2(20, 640), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Random Generator", nullptr);

        if (ImGui::Button("Generate Random Movie + Actors", ImVec2(-FLT_MIN, 40)))
        {
            static const std::vector<std::string> titles = {"Interstellar", "Inception", "The Dark Knight", "Tenet", "Dunkirk", "Memento"};
            static const std::vector<std::string> studios = {"Warner Bros", "Universal", "Paramount", "Sony", "Disney"};
            static const std::vector<std::string> actorNames = {"Tom Hardy", "Cillian Murphy", "Emma Stone", "Brad Pitt", "Leo DiCaprio"};
            
            static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<> dTitle(0, static_cast<int>(titles.size() - 1));
            std::uniform_int_distribution<> dStudio(0, static_cast<int>(studios.size() - 1));
            std::uniform_int_distribution<> dActor(0, static_cast<int>(actorNames.size() - 1));
            std::uniform_int_distribution<> dYear(1990, 2024);
            std::uniform_real_distribution<> dRating(1.0, 10.0);
            std::uniform_int_distribution<> dLength(3600, 10800);
            std::uniform_int_distribution<> dNumActors(1, 3);

            Movie movie;
            movie.title = titles[static_cast<std::size_t>(dTitle(gen))] + " " + std::to_string(gen() % 100);
            movie.studio = studios[static_cast<std::size_t>(dStudio(gen))];
            movie.year = dYear(gen);
            movie.rating = dRating(gen);
            movie.length = dLength(gen);
            movie.description = "Generated description for " + movie.title;
            
            int numActors = dNumActors(gen);
            for (int i = 0; i < numActors; ++i)
            {
                Actor actor;
                actor.name = actorNames[static_cast<std::size_t>(dActor(gen))] + " " + std::to_string(gen() % 100);
                actor.description = "Generated actor info";
                actor.birthdate = 631152000; // 1990 approx
                
                int newActorId = controller.AddActor(std::move(actor));
                movie.actorIds.push_back(newActorId);
            }
            
            int newMovieId = controller.AddMovie(std::move(movie));
            
            // Связываем назад
            for (int aid : movie.actorIds)
            {
                const Actor* a = controller.GetActorById(aid);
                if (a)
                {
                    Actor updated = *a;
                    updated.filmIds.insert(newMovieId);
                    controller.UpdateActor(aid, updated);
                }
            }
        }
        ImGui::End();

        static int frame_count = 0;
        if (++frame_count % 600 == 0) iam_gc();
    }
}