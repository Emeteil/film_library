#pragma once

#include "gui/AppController.h"
#include "gui/widgets/ActorFormDialog.h"
#include "gui/widgets/ConfirmDialog.h"

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
            void HandleActorSubmit(const Actor& actor, AppController& ctrl, MainWidgetState& state);
            void HandleActorDelete(int actorId, AppController& ctrl, MainWidgetState& state);

            char actorSearch[256] = {};
            int actorSortKey = 0;
            bool actorSortAsc = true;

            ActorFormDialog actorFormDialog;
            ConfirmDialog confirmDialog;
    };
}