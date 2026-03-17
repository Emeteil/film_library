#pragma once

/// @file ActorManager.h
/// @brief Менеджер данных актёров - CRUD-операции и доступ по ID.
///
/// Упрощённый аналог DataManager для таблицы актёров.
/// Индексация только по ID через дерево оптимального поиска.
/// Автоматически сохраняет данные в CSV при каждом изменении.

#include "core/algorithms/OptimalBST.h"
#include "core/models/Actor.h"

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    class ActorManager
    {
        public:
            ActorManager();
            ~ActorManager();

            ActorManager(const ActorManager&) = delete;
            ActorManager& operator=(const ActorManager&) = delete;

            /// @brief Загрузить данные из CSV-файла.
            /// @param filePath  Путь к CSV-файлу актёров.
            /// @return Список ошибок парсинга (пустой при полном успехе).
            std::vector<std::string> LoadFromCsv(const std::string& filePath);

            /// @brief Сохранить данные в CSV-файл.
            /// @return true при успехе.
            bool SaveToCsv() const;

            /// @brief Добавить нового актёра. ID назначается автоматически.
            /// @return ID добавленного актёра.
            int AddActor(Actor data);

            /// @brief Обновить данные существующего актёра по ID.
            /// @return true, если актёр с указанным ID найден и обновлён.
            bool UpdateActor(int id, const Actor& data);

            /// @brief Удалить актёра по ID.
            /// @return true, если актёр был найден и удалён.
            bool DeleteActor(int id);

            /// @brief Получить актёра по ID (только для чтения).
            /// @return Указатель на актёра или nullptr, если не найден.
            const Actor* GetActorById(int id) const;

            /// @brief Получить всех актёров (только для чтения).
            const std::vector<std::unique_ptr<Actor>>& GetAllActors() const;

            /// @brief Общее количество актёров.
            std::size_t GetActorCount() const;

        private:
            std::vector<std::unique_ptr<Actor>> actors;
            OptimalBST<Actor*, int> idIndex;
            int nextId = 1;
            std::string csvFilePath;

            /// @brief Перестроить индекс и авто-сохранить.
            void OnDataChanged();
    };
}