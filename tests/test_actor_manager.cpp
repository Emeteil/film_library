#include "TestFramework.h"
#include "core/ActorManager.h"
#include "core/models/Actor.h"

#include <fstream>
#include <cstdio>
#include <random>
#include <string>
#include <algorithm>

using namespace FilmLibrary;

static Actor CreateTestActor(int id = 0, const std::string& name = "Test Actor",
                              const std::string& description = "Test description",
                              long birthdate = 631152000)
{
    Actor actor;
    actor.id = id;
    actor.name = name;
    actor.description = description;
    actor.birthdate = birthdate;
    actor.photo = "photo.jpg";
    actor.filmIds = {1, 2, 3};
    return actor;
}

static std::string CreateTempCsvFile(const std::string& content)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(10000, 99999);
    
    std::string tempFile = "temp_actor_test_" + std::to_string(dis(gen)) + ".csv";
    
    std::ofstream file(tempFile);
    if (!file.is_open()) {
        return "";
    }
    file << content;
    file.close();
    
    return tempFile;
}

static void DeleteTempFile(const std::string& filePath)
{
    if (!filePath.empty()) {
        std::remove(filePath.c_str());
    }
}

TEST(ActorManager_LoadFromCsv_Success)
{
    ActorManager am;
    
    std::string csvContent = "id,name,description,birthdate,photo,filmIds\n"
                             "1,Leonardo DiCaprio,American actor,631152000,leo.jpg,\"1,2,3\"\n"
                             "2,Tom Hanks,American actor and filmmaker,631152000,tom.jpg,\"4,5,6\"\n";
    
    std::string filePath = CreateTempCsvFile(csvContent);
    ASSERT_FALSE(filePath.empty());
    
    std::vector<std::string> errors = am.LoadFromCsv(filePath);
    
    ASSERT_TRUE(errors.empty());
    ASSERT_EQ(am.GetActorCount(), 2);
    
    const Actor* actor = am.GetActorById(1);
    ASSERT_TRUE(actor != nullptr);
    ASSERT_EQ(actor->name, "Leonardo DiCaprio");
    ASSERT_EQ(actor->birthdate, 631152000);
    
    DeleteTempFile(filePath);
}

TEST(ActorManager_LoadFromCsv_FileNotFound)
{
    ActorManager am;
    std::vector<std::string> errors = am.LoadFromCsv("nonexistent_file.csv");
    
    ASSERT_EQ(am.GetActorCount(), 0);
}

TEST(ActorManager_LoadFromCsv_WithErrors)
{
    ActorManager am;
    
    std::string csvContent = "id,name,description,birthdate,photo,filmIds\n"
                             "1,Leonardo DiCaprio,American actor,invalid,leo.jpg,\"1,2,3\"\n"
                             "2,Tom Hanks,American actor and filmmaker,631152000,tom.jpg,\"4,5,6\"\n";
    
    std::string filePath = CreateTempCsvFile(csvContent);
    ASSERT_FALSE(filePath.empty());
    
    std::vector<std::string> errors = am.LoadFromCsv(filePath);
    
    ASSERT_FALSE(errors.empty());
    ASSERT_EQ(am.GetActorCount(), 1);
    
    DeleteTempFile(filePath);
}

TEST(ActorManager_SaveToCsv_Success)
{
    ActorManager am;
    
    std::string tempFile = CreateTempCsvFile("");
    ASSERT_FALSE(tempFile.empty());
    
    am.LoadFromCsv(tempFile);
    
    Actor actor = CreateTestActor(0, "New Actor", "Description", 631152000);
    int id = am.AddActor(actor);
    ASSERT_NE(id, 0);
    
    bool saved = am.SaveToCsv();
    ASSERT_TRUE(saved);
    
    std::ifstream file(tempFile);
    ASSERT_TRUE(file.good());
    
    DeleteTempFile(tempFile);
}

TEST(ActorManager_SaveToCsv_EmptyPath)
{
    ActorManager am;
    
    bool saved = am.SaveToCsv();
    ASSERT_FALSE(saved);
}

TEST(ActorManager_AddActor)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "New Actor", "Test description", 631152000);
    int newId = am.AddActor(actor);
    
    ASSERT_NE(newId, 0);
    ASSERT_EQ(am.GetActorCount(), 1);
    
    const Actor* retrieved = am.GetActorById(newId);
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->name, "New Actor");
    ASSERT_EQ(retrieved->description, "Test description");
    ASSERT_EQ(retrieved->birthdate, 631152000);
}

TEST(ActorManager_AddActor_AutoIncrementId)
{
    ActorManager am;
    
    Actor actor1 = CreateTestActor(0, "Actor 1");
    int id1 = am.AddActor(actor1);
    
    Actor actor2 = CreateTestActor(0, "Actor 2");
    int id2 = am.AddActor(actor2);
    
    ASSERT_EQ(id2, id1 + 1);
}

TEST(ActorManager_UpdateActor_Success)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "Original Name", "Original description", 631152000);
    int id = am.AddActor(actor);
    
    Actor updatedData = CreateTestActor(id, "Updated Name", "Updated description", 946944000);
    bool updated = am.UpdateActor(id, updatedData);
    
    ASSERT_TRUE(updated);
    
    const Actor* retrieved = am.GetActorById(id);
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->name, "Updated Name");
    ASSERT_EQ(retrieved->description, "Updated description");
    ASSERT_EQ(retrieved->birthdate, 946944000);
}

TEST(ActorManager_UpdateActor_NotFound)
{
    ActorManager am;
    
    Actor updatedData = CreateTestActor(999, "Updated Name");
    bool updated = am.UpdateActor(999, updatedData);
    
    ASSERT_FALSE(updated);
    ASSERT_EQ(am.GetActorCount(), 0);
}

TEST(ActorManager_DeleteActor_Success)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "Actor to Delete");
    int id = am.AddActor(actor);
    ASSERT_EQ(am.GetActorCount(), 1);
    
    bool deleted = am.DeleteActor(id);
    
    ASSERT_TRUE(deleted);
    ASSERT_EQ(am.GetActorCount(), 0);
    
    const Actor* retrieved = am.GetActorById(id);
    ASSERT_TRUE(retrieved == nullptr);
}

TEST(ActorManager_DeleteActor_NotFound)
{
    ActorManager am;
    
    bool deleted = am.DeleteActor(999);
    
    ASSERT_FALSE(deleted);
    ASSERT_EQ(am.GetActorCount(), 0);
}

TEST(ActorManager_GetActorById_Found)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "Find Me");
    int id = am.AddActor(actor);
    
    const Actor* retrieved = am.GetActorById(id);
    
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->name, "Find Me");
    ASSERT_EQ(retrieved->id, id);
}

TEST(ActorManager_GetActorById_NotFound)
{
    ActorManager am;
    
    const Actor* retrieved = am.GetActorById(999);
    
    ASSERT_TRUE(retrieved == nullptr);
}

TEST(ActorManager_GetAllActors)
{
    ActorManager am;
    
    Actor actor1 = CreateTestActor(0, "Actor 1");
    Actor actor2 = CreateTestActor(0, "Actor 2");
    Actor actor3 = CreateTestActor(0, "Actor 3");
    
    am.AddActor(actor1);
    am.AddActor(actor2);
    am.AddActor(actor3);
    
    const auto& allActors = am.GetAllActors();
    
    ASSERT_EQ(allActors.size(), 3);
    ASSERT_TRUE(allActors[0] != nullptr);
    ASSERT_TRUE(allActors[1] != nullptr);
    ASSERT_TRUE(allActors[2] != nullptr);
}

TEST(ActorManager_GetActorCount)
{
    ActorManager am;
    
    ASSERT_EQ(am.GetActorCount(), 0);
    
    am.AddActor(CreateTestActor(0, "Actor 1"));
    ASSERT_EQ(am.GetActorCount(), 1);
    
    am.AddActor(CreateTestActor(0, "Actor 2"));
    ASSERT_EQ(am.GetActorCount(), 2);
    
    am.DeleteActor(1);
    ASSERT_EQ(am.GetActorCount(), 1);
}

TEST(ActorManager_OptimalBST_Search)
{
    ActorManager am;
    
    Actor actor1 = CreateTestActor(0, "Actor 1");
    Actor actor2 = CreateTestActor(0, "Actor 2");
    Actor actor3 = CreateTestActor(0, "Actor 3");
    
    int id1 = am.AddActor(actor1);
    int id2 = am.AddActor(actor2);
    int id3 = am.AddActor(actor3);
    
    const Actor* retrieved1 = am.GetActorById(id1);
    const Actor* retrieved2 = am.GetActorById(id2);
    const Actor* retrieved3 = am.GetActorById(id3);
    
    ASSERT_TRUE(retrieved1 != nullptr);
    ASSERT_TRUE(retrieved2 != nullptr);
    ASSERT_TRUE(retrieved3 != nullptr);
    ASSERT_EQ(retrieved1->id, id1);
    ASSERT_EQ(retrieved2->id, id2);
    ASSERT_EQ(retrieved3->id, id3);
}

TEST(ActorManager_IndexRebuildOnAdd)
{
    ActorManager am;
    
    Actor actor1 = CreateTestActor(0, "Actor 1");
    int id1 = am.AddActor(actor1);
    
    const Actor* retrieved1 = am.GetActorById(id1);
    ASSERT_TRUE(retrieved1 != nullptr);
    ASSERT_EQ(retrieved1->name, "Actor 1");
    
    Actor actor2 = CreateTestActor(0, "Actor 2");
    int id2 = am.AddActor(actor2);
    
    const Actor* retrieved2 = am.GetActorById(id2);
    ASSERT_TRUE(retrieved2 != nullptr);
    ASSERT_EQ(retrieved2->name, "Actor 2");
    
    const Actor* retrieved1Again = am.GetActorById(id1);
    ASSERT_TRUE(retrieved1Again != nullptr);
    ASSERT_EQ(retrieved1Again->name, "Actor 1");
}

TEST(ActorManager_IndexRebuildOnUpdate)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "Original Name");
    int id = am.AddActor(actor);
    
    const Actor* before = am.GetActorById(id);
    ASSERT_EQ(before->name, "Original Name");
    
    Actor updated = CreateTestActor(id, "Updated Name");
    am.UpdateActor(id, updated);
    
    const Actor* after = am.GetActorById(id);
    ASSERT_EQ(after->name, "Updated Name");
}

TEST(ActorManager_IndexRebuildOnDelete)
{
    ActorManager am;
    
    Actor actor1 = CreateTestActor(0, "Actor 1");
    Actor actor2 = CreateTestActor(0, "Actor 2");
    
    int id1 = am.AddActor(actor1);
    int id2 = am.AddActor(actor2);
    
    const Actor* retrieved1 = am.GetActorById(id1);
    ASSERT_TRUE(retrieved1 != nullptr);
    
    am.DeleteActor(id1);
    
    const Actor* deleted = am.GetActorById(id1);
    ASSERT_TRUE(deleted == nullptr);
    
    const Actor* retrieved2 = am.GetActorById(id2);
    ASSERT_TRUE(retrieved2 != nullptr);
    ASSERT_EQ(retrieved2->id, id2);
}

TEST(ActorManager_ChainOperations)
{
    ActorManager am;
    
    int id1 = am.AddActor(CreateTestActor(0, "Actor A", "Description A", 631152000));
    int id2 = am.AddActor(CreateTestActor(0, "Actor B", "Description B", 946944000));
    
    ASSERT_EQ(am.GetActorCount(), 2);
    
    Actor updated = CreateTestActor(id1, "Actor A Updated", "Description Updated", 978307200);
    bool updatedOk = am.UpdateActor(id1, updated);
    ASSERT_TRUE(updatedOk);
    
    const Actor* retrieved = am.GetActorById(id1);
    ASSERT_EQ(retrieved->name, "Actor A Updated");
    ASSERT_EQ(retrieved->birthdate, 978307200);
    
    bool deleted = am.DeleteActor(id2);
    ASSERT_TRUE(deleted);
    ASSERT_EQ(am.GetActorCount(), 1);
    
    const Actor* remaining = am.GetActorById(id1);
    ASSERT_TRUE(remaining != nullptr);
    ASSERT_EQ(remaining->name, "Actor A Updated");
}

TEST(ActorManager_MultipleActorsDifferentIds)
{
    ActorManager am;
    
    int id1 = am.AddActor(CreateTestActor(0, "First"));
    int id2 = am.AddActor(CreateTestActor(0, "Second"));
    int id3 = am.AddActor(CreateTestActor(0, "Third"));
    
    ASSERT_NE(id1, id2);
    ASSERT_NE(id2, id3);
    ASSERT_NE(id1, id3);
    
    const Actor* a1 = am.GetActorById(id1);
    const Actor* a2 = am.GetActorById(id2);
    const Actor* a3 = am.GetActorById(id3);
    
    ASSERT_TRUE(a1 != nullptr);
    ASSERT_TRUE(a2 != nullptr);
    ASSERT_TRUE(a3 != nullptr);
    
    ASSERT_EQ(a1->name, "First");
    ASSERT_EQ(a2->name, "Second");
    ASSERT_EQ(a3->name, "Third");
}

TEST(ActorManager_FilmIdsPreservation)
{
    ActorManager am;
    
    Actor actor = CreateTestActor(0, "Actor with Films");
    actor.filmIds = {10, 20, 30, 40};
    int id = am.AddActor(actor);
    
    const Actor* retrieved = am.GetActorById(id);
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->filmIds.size(), 4);
    
    auto it = retrieved->filmIds.begin();
    ASSERT_EQ(*it, 10);
    ++it;
    ASSERT_EQ(*it, 20);
    ++it;
    ASSERT_EQ(*it, 30);
    ++it;
    ASSERT_EQ(*it, 40);
    
    Actor updated = *retrieved;
    updated.filmIds = {50, 60};
    am.UpdateActor(id, updated);
    
    const Actor* updatedRetrieved = am.GetActorById(id);
    ASSERT_EQ(updatedRetrieved->filmIds.size(), 2);
    
    auto it2 = updatedRetrieved->filmIds.begin();
    ASSERT_EQ(*it2, 50);
    ++it2;
    ASSERT_EQ(*it2, 60);
}