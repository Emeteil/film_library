#pragma once

#include "gui/AppWindow.h"
#include "gui/AppController.h"
#include <memory>

struct GLFWwindow;

namespace FilmLibrary
{
    struct MainWidgetState
    {
        int activeTab = 0;
        int filmSelected = -1;
        int actorSelected = -1;
        
        int filmVisitCounter = 0;
        int actorVisitCounter = 0;
        int homeVisitCounter = 0;
    };

    class HomeTab;
    class FilmsTab;
    class ActorsTab;

    class MainWidget
    {
        public:
            MainWidget();
            ~MainWidget();

            void Render(AppController& controller, GLFWwindow* window);

        private:
            void RenderAppHeader();
            void RenderTabBar(float dt);

            MainWidgetState state;
            std::unique_ptr<HomeTab> homeTab;
            std::unique_ptr<FilmsTab> filmsTab;
            std::unique_ptr<ActorsTab> actorsTab;
    };
}