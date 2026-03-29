/// @file MovieTableWidget.cpp
/// @brief Table

#include "gui/widgets/MovieTableWidget.h"
#include <imgui.h>
#include <cstdio>

namespace FilmLibrary
{
    MovieTableWidget::MovieTableWidget()  = default;
    MovieTableWidget::~MovieTableWidget() = default;

    int  MovieTableWidget::GetSelectedMovieId() const { return selectedMovieId; }
    void MovieTableWidget::ClearSelection() { selectedMovieId = -1; }

    void MovieTableWidget::SetOnColumnClick(OnColumnClickCallback cb, void* ud)
    {
        onColumnClick        = cb;
        columnClickUserData  = ud;
    }

    void MovieTableWidget::Render(const std::vector<Movie*>& movies)
    {
        (void)movies;
    }
}
