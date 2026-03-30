#pragma once

#include "gui/AppController.h"

namespace FilmLibrary
{
    struct MainWidgetState;

    class ActorsTab
    {
        public:
            void Render(AppController& ctrl, MainWidgetState& state);

        private:
            void RenderActorRow(const Actor* a, bool selected);
            void RenderActorDetail(const Actor* a, AppController& ctrl, MainWidgetState& state);

            char actorSearch[256] = {};
            int actorSortKey = 0;
            bool actorSortAsc = true;
    };
}