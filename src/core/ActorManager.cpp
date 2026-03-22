/// @file ActorManager.cpp
/// @brief Реализация менеджера данных актёров.

#include "core/ActorManager.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

namespace FilmLibrary
{
    ActorManager::ActorManager() : idIndex([](Actor* const& a) -> int { return a->id; })
    {
    }

    ActorManager::~ActorManager() = default;

    std::vector<std::string> ActorManager::LoadFromCsv(const std::string& filePath)
    {
        csvFilePath = filePath;

        // TODO: Реализовать загрузку актёров из CSV.
        //
        // 1. Вызвать CsvParser для загрузки.
        // 2. Найти максимальный id и установить nextId = max + 1.
        // 3. Построить индекс по id.

        (void)filePath;
        return {};
    }

    bool ActorManager::SaveToCsv() const
    {
        // TODO: Делегировать CsvParser для сохранения.
        return false;
    }

    int ActorManager::AddActor(Actor data)
    {
        // TODO: Реализовать добавление.
        //
        // 1. Назначить data.id = nextId++.
        // 2. Создать unique_ptr и добавить в actors.
        // 3. Вызвать OnDataChanged().
        // 4. Вернуть id.

        (void)data;
        return -1;
    }

    bool ActorManager::UpdateActor(int id, const Actor& data)
    {
        // TODO: Реализовать обновление.
        (void)id;
        (void)data;
        return false;
    }

    bool ActorManager::DeleteActor(int id)
    {
        // TODO: Реализовать удаление.
        (void)id;
        return false;
    }

    const Actor* ActorManager::GetActorById(int id) const
    {
        // TODO: Использовать idIndex.Find(id) для быстрого поиска.
        (void)id;
        return nullptr;
    }

    const std::vector<std::unique_ptr<Actor>>& ActorManager::GetAllActors() const
    {
        return actors;
    }

    std::size_t ActorManager::GetActorCount() const
    {
        return actors.size();
    }

    void ActorManager::OnDataChanged()
    {
        // TODO: Реализовать перестройку индекса и авто-сохранение.
        //
        // 1. Собрать сырые указатели в std::vector<Actor*>.
        // 2. idIndex.BuildTree(pointers) - ключи извлекаются через KeyExtractor.
        // 3. SaveToCsv();
    }
}