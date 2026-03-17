/// @file MovieFormDialog.cpp
/// @brief Реализация модального диалога добавления/редактирования фильма.

#include "gui/widgets/MovieFormDialog.h"

#include <imgui.h>
#include <cstring>

namespace FilmLibrary
{
    MovieFormDialog::MovieFormDialog() = default;
    MovieFormDialog::~MovieFormDialog() = default;

    void MovieFormDialog::OpenForAdd()
    {
        isOpen = true;
        isEditMode = false;
        editingMovieId = -1;

        std::memset(titleBuffer, 0, sizeof(titleBuffer));
        std::memset(studioBuffer, 0, sizeof(studioBuffer));
        std::memset(descriptionBuffer, 0, sizeof(descriptionBuffer));
        std::memset(coverBuffer, 0, sizeof(coverBuffer));
        std::memset(streamLinkBuffer, 0, sizeof(streamLinkBuffer));
        std::memset(genresBuffer, 0, sizeof(genresBuffer));
        year = 2024;
        lengthMinutes = 90;
        rating = 5.0f;
    }

    void MovieFormDialog::OpenForEdit(const Movie& movie)
    {
        isOpen = true;
        isEditMode = true;
        FillFromMovie(movie);
    }

    void MovieFormDialog::Render()
    {
        if (!isOpen)
            return;

        // TODO: Реализовать ImGui popup.
        //
        // 1. ImGui::OpenPopup(isEditMode ? "Редактировать фильм" : "Добавить фильм").
        // 2. ImGui::BeginPopupModal(...).
        // 3. ImGui::InputText для каждого поля.
        // 4. ImGui::InputInt для year, lengthMinutes.
        // 5. ImGui::SliderFloat для rating.
        // 6. Кнопка "OK" → Validate(), если ок - BuildMovieFromBuffers(), onSubmit(movie).
        // 7. Кнопка "Отмена" → isOpen = false.
        // 8. ImGui::EndPopup().
    }

    void MovieFormDialog::SetOnSubmit(OnSubmitCallback callback)
    {
        onSubmit = std::move(callback);
    }

    bool MovieFormDialog::IsOpen() const
    {
        return isOpen;
    }

    void MovieFormDialog::FillFromMovie(const Movie& movie)
    {
        // TODO: Скопировать поля Movie в буферы.
        editingMovieId = movie.id;
        (void)movie;
    }

    Movie MovieFormDialog::BuildMovieFromBuffers() const
    {
        // TODO: Собрать Movie из буферов.
        Movie movie;
        movie.id = editingMovieId;
        return movie;
    }

    std::string MovieFormDialog::Validate() const
    {
        // TODO: Проверить валидность ввода.
        //
        // - title не пустой
        // - year в разумном диапазоне
        // - rating 0.0 – 10.0
        // - length > 0

        return "";
    }
}
