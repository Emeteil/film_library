#pragma once

#include "gui/AppController.h"
#include <vector>

namespace FilmLibrary
{
    struct MainWidgetState;

    class HomeTab
    {
        public:
            void Render(AppController& ctrl, MainWidgetState& state, float dt);

        private:
            void RebuildHomeStack(const std::vector<Movie*>& movies);

            int homeIndex = 0;
            bool homeNeedRefresh = true;
            std::vector<int> homeStack;
    };
}