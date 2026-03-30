#pragma once

#include "gui/AppController.h"
#include <string>

namespace FilmLibrary
{
    struct MainWidgetState;

    class FilmsTab
    {
        public:
            void Render(AppController& ctrl, MainWidgetState& state);

        private:
            void RenderFilmSearchRow(const Movie* m, bool selected);
            void RenderFilmDetail(const Movie* m, AppController& ctrl, MainWidgetState& state);

            char filmSearch[256] = {};
            int filmSearchMode = 0;
            int filmSortKey = 0;
            bool filmSortAsc = true;

            bool showFilters = false;
            float ratingRange[2] = {0.0f, 10.0f};
            int lengthRange[2] = {0, 600};
    };
}