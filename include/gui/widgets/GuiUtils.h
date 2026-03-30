#pragma once

/// @file GuiUtils.h

#include <imgui.h>
#include <string>

namespace FilmLibrary
{
    class GuiUtils
    {
        public:
            static void DrawCoverPlaceholder(ImVec2 pos, ImVec2 size, const char* title);
            static void GoldSeparator();
            static void SectionTitle(const char* text);
            static void DrawStars(float rating10, float scale = 1.0f);
            static void DrawGenreTag(const char* genre);
            static std::string FormatDuration(int secs);
            static std::string FormatDate(long int ts);
            static bool RenderSortButton(const char* popupId, const char** labels, int count, int& currentKey, bool& ascending);
    };
}