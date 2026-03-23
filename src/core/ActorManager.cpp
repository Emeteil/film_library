/// @file ActorManager.cpp
/// @brief Реализация менеджера данных актёров.

#include "core/ActorManager.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"
#include "core/storage/ActorCsvMapper.h"

namespace FilmLibrary
{
    ActorManager::ActorManager() : idIndex([](Actor* const& a) -> int { return a->id; })
    {
    }

    ActorManager::~ActorManager() = default;

    std::vector<std::string> ActorManager::LoadFromCsv(const std::string& filePath)
    {
        csvFilePath = filePath;
        std::vector<std::string> errors;


        auto result = CsvParser::LoadFromFile<Actor, ActorCsvMapper>(filePath);
        errors = result.errors;
        if (!result.errors.empty())
        {
            Logger::Instance().Warning("Warnings:" + std::to_string(result.errors.size()));
            for (const auto& err : result.errors)
            {
                Logger::Instance().Error(err);
            }
        }
        actors = std::move(result.records);


        int maxId = 0;
        for (const auto& actor : actors)
        {
            if (actor && actor->id > maxId)
            {
                maxId = actor->id;
            }
        }
        nextId = maxId + 1;
        

        std::vector<Actor*> pointers;
        pointers.reserve(actors.size());
        for (const auto& actor : actors)
        {
            if (actor)
            {
                pointers.push_back(actor.get());
            }
        }
        idIndex.BuildTree(pointers);

        Logger::Instance().Info("Loaded " + std::to_string(actors.size()) + " actors from " + filePath);

        return errors;
    }

    bool ActorManager::SaveToCsv() const
    {
        if (csvFilePath.empty())
        {
            Logger::Instance().Error("Cannot save actors: CSV file path is empty");
            return false;
        }
        
        bool success = CsvParser::SaveToFile<Actor, ActorCsvMapper>(csvFilePath, actors);
        
        if (success)
        {
            Logger::Instance().Info("Saved " + std::to_string(actors.size()) + " actors to " + csvFilePath);
        }
        else
        {
            Logger::Instance().Error("Failed to save actors to " + csvFilePath);
        }
        
        return success;
    }
    

     int ActorManager::AddActor(Actor data)
    {
        data.id = nextId++;
        
        auto actor = std::make_unique<Actor>(std::move(data));
        int newId = actor->id;
        actors.push_back(std::move(actor));
        
        OnDataChanged();

        Logger::Instance().Info("Added actor with ID: " + std::to_string(newId));
        return newId;
    }

    bool ActorManager::UpdateActor(int id, const Actor& data)
    {
        auto it = std::find_if(actors.begin(), actors.end(),[id](const std::unique_ptr<Actor>& actor) { return actor && actor->id == id; });
        
        if (it == actors.end())
        {
            Logger::Instance().Warning("Cannot update actor: ID " + std::to_string(id) + " not found");
            return false;
        }
        Actor* actor = it->get();
        actor->name = data.name;
        actor->description = data.description;
        actor->birthdate = data.birthdate;
        actor->filmIds = data.filmIds;
        actor->photo = data.photo;

        OnDataChanged();
        
        Logger::Instance().Info("Updated actor with ID: " + std::to_string(id));
        return true;
    }


    bool ActorManager::DeleteActor(int id)
    {
        auto it = std::find_if(actors.begin(), actors.end(),[id](const std::unique_ptr<Actor>& actor) { return actor && actor->id == id; });
        
        if (it == actors.end())
        {
            Logger::Instance().Warning("Cannot delete actor: ID " + std::to_string(id) + " not found");
            return false;
        }
        
        actors.erase(it);
        
        OnDataChanged();
        
        Logger::Instance().Info("Deleted actor with ID: " + std::to_string(id));
        return true;
    }

    const Actor* ActorManager::GetActorById(int id) const
    {
         std::vector<Actor*> results = idIndex.Find(id);
        if (results.empty())
        {
            return nullptr;
        }
        return results[0];
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
        std::vector<Actor*> pointers;
        pointers.reserve(actors.size());
        for (const auto& actor : actors)
        {
            if (actor)
            {
                pointers.push_back(actor.get());
            }
        }

        idIndex.BuildTree(pointers);
        SaveToCsv();
    }
}