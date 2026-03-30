#pragma once

/// @file ImageLoader.h
/// @brief Асинхронный загрузчик изображений с кешированием.

#include <string>
#include <cstdint>

namespace FilmLibrary
{
    class ImageLoader
    {
        public:
            /// @brief Получает OpenGL текстуру для данного URL/пути.
            /// Запускает асинхронную загрузку с кешированием на диск, если текстура еще не скачана.
            /// @param url URL изображения.
            /// @return 0, если загрузка не завершена или не удалась, иначе ID текстуры (GLuint).
            static uint32_t GetOrLoadTexture(const std::string& url);

            /// @brief Получает исходные размеры текстуры.
            /// @param url URL изображения.
            /// @param outWidth Переменная для записи ширины.
            /// @param outHeight Переменная для записи высоты.
            /// @return true, если размеры успешно получены.
            static bool GetTextureSize(const std::string& url, int& outWidth, int& outHeight);

            /// @brief Очищает все загруженные ресурсы.
            static void Shutdown();
    };
}