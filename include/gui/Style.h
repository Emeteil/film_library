#pragma once

/// @file Style.h
/// @brief Общие константы цветов и стиля для GUI "Фильмотека".

#include <imgui.h>

namespace FilmLibrary
{
namespace Style
{
    /// Золотой акцент (активный элемент, заголовки, звёзды)
    inline constexpr ImVec4 Gold          = {0.90f, 0.72f, 0.28f, 1.00f};
    /// Приглушённое золото (hover, теги)
    inline constexpr ImVec4 GoldMuted     = {0.75f, 0.58f, 0.22f, 1.00f};
    /// Тёмное золото (фон тегов, кнопки)
    inline constexpr ImVec4 GoldDark      = {0.35f, 0.25f, 0.08f, 1.00f};
    /// Основной текст
    inline constexpr ImVec4 TextPrimary   = {0.92f, 0.92f, 0.94f, 1.00f};
    /// Вторичный текст (мета-данные)
    inline constexpr ImVec4 TextSecondary = {0.60f, 0.60f, 0.65f, 1.00f};
    /// Отключённый текст
    inline constexpr ImVec4 TextDisabled  = {0.40f, 0.40f, 0.45f, 1.00f};
    /// Фон окна
    inline constexpr ImVec4 BgWindow      = {0.09f, 0.09f, 0.11f, 1.00f};
    /// Фон дочернего виджета
    inline constexpr ImVec4 BgChild       = {0.07f, 0.07f, 0.09f, 1.00f};
    /// Фон панели поиска / заголовка
    inline constexpr ImVec4 BgPanel       = {0.09f, 0.08f, 0.05f, 1.00f};
    /// Граница
    inline constexpr ImVec4 Border        = {0.18f, 0.18f, 0.22f, 1.00f};
    /// Граница золотая
    inline constexpr ImVec4 BorderGold    = {0.30f, 0.22f, 0.08f, 1.00f};

    // --- U32 цвета для DrawList ---

    inline constexpr ImU32 U32Gold        = IM_COL32(230, 184, 71, 255);
    inline constexpr ImU32 U32GoldMuted   = IM_COL32(191, 148, 56, 255);
    inline constexpr ImU32 U32GoldDark    = IM_COL32(90,  68, 20, 255);
    inline constexpr ImU32 U32TextPrimary = IM_COL32(235, 235, 240, 255);
    inline constexpr ImU32 U32TextMuted   = IM_COL32(153, 153, 163, 255);
    inline constexpr ImU32 U32BgDark      = IM_COL32(15,  14, 10, 255);
    inline constexpr ImU32 U32Border      = IM_COL32(46,  46, 56, 255);
    inline constexpr ImU32 U32BorderGold  = IM_COL32(90,  68, 20, 180);

    // --- Размеры ---

    /// Высота TabBar
    inline constexpr float TabBarH        = 48.0f;
    /// Высота App-заголовка
    inline constexpr float AppHeaderH     = 38.0f;
    /// Горизонтальный отступ контента
    inline constexpr float ContentPadX    = 18.0f;
    /// Вертикальный отступ контента
    inline constexpr float ContentPadY    = 10.0f;
    /// Стандартная ширина обложки на главной
    inline constexpr float CoverWidthHome = 280.0f;
    /// Размер аватара актёра на главной
    inline constexpr float ActorAvatarSz  = 56.0f;
    /// Высота строки поиска
    inline constexpr float SearchRowH     = 68.0f;
    /// Высота панели поиска
    inline constexpr float SearchPanelH   = 56.0f;

} // namespace Style
} // namespace FilmLibrary
