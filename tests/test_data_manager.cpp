#include "TestFramework.h"
#include "core/DataManager.h"
#include "core/models/Movie.h"

#include <fstream>
#include <cstdio>
#include <random>
#include <string>

using namespace FilmLibrary;

static Movie CreateTestMovie(int id = 0, const std::string& title = "Test Movie",
                              const std::string& studio = "Test Studio", int year = 2020,
                              double rating = 7.5, int length = 120)
{
    Movie movie;
    movie.id = id;
    movie.title = title;
    movie.studio = studio;
    movie.description = "This is a test movie description";
    movie.year = year;
    movie.length = length;
    movie.rating = rating;
    movie.cover = "cover.jpg";
    movie.streamLink = "http://example.com/stream";
    movie.genres = {"Action", "Drama"};
    movie.actorIds = {1, 2, 3};
    return movie;
}

static std::string CreateTempCsvFile(const std::string& content)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(10000, 99999);
    
    std::string tempFile = "temp_test_" + std::to_string(dis(gen)) + ".csv";
    
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

TEST(DataManager_LoadFromCsv_Success)
{
    DataManager dm;
    
    std::string csvContent = "id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds\n"
                             "1,Inception,Warner Bros,A mind-bending thriller,2010,148,8.8,cover1.jpg,http://example.com/inception,\"Action,Sci-Fi\",\"1,2\"\n"
                             "2,The Matrix,Warner Bros,A computer hacker learns about reality,1999,136,8.7,cover2.jpg,http://example.com/matrix,\"Action,Sci-Fi\",\"2,3\"\n";
    
    std::string filePath = CreateTempCsvFile(csvContent);
    ASSERT_FALSE(filePath.empty());
    
    std::vector<std::string> errors = dm.LoadFromCsv(filePath);
    
    ASSERT_TRUE(errors.empty());
    ASSERT_EQ(dm.GetMovieCount(), 2);
    
    const Movie* movie = dm.GetMovieById(1);
    ASSERT_TRUE(movie != nullptr);
    ASSERT_EQ(movie->title, "Inception");
    ASSERT_EQ(movie->year, 2010);
    
    DeleteTempFile(filePath);
}

TEST(DataManager_LoadFromCsv_FileNotFound)
{
    DataManager dm;
    std::vector<std::string> errors = dm.LoadFromCsv("nonexistent_file.csv");
    
    ASSERT_EQ(dm.GetMovieCount(), 0);
}

TEST(DataManager_LoadFromCsv_WithErrors)
{
    DataManager dm;
    
    std::string csvContent = "id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds\n"
                             "1,Inception,Warner Bros,A mind-bending thriller,invalid,148,8.8,cover1.jpg,http://example.com/inception,\"Action,Sci-Fi\",\"1,2\"\n"
                             "2,The Matrix,Warner Bros,A computer hacker learns about reality,1999,136,8.7,cover2.jpg,http://example.com/matrix,\"Action,Sci-Fi\",\"2,3\"\n";
    
    std::string filePath = CreateTempCsvFile(csvContent);
    ASSERT_FALSE(filePath.empty());
    
    std::vector<std::string> errors = dm.LoadFromCsv(filePath);
    
    ASSERT_FALSE(errors.empty());
    ASSERT_EQ(dm.GetMovieCount(), 1);
    
    DeleteTempFile(filePath);
}

TEST(DataManager_SaveToCsv_Success)
{
    DataManager dm;
    
    std::string tempFile = CreateTempCsvFile("");
    ASSERT_FALSE(tempFile.empty());
    
    dm.LoadFromCsv(tempFile);
    
    Movie movie = CreateTestMovie(0, "Test Movie", "Test Studio", 2021, 8.5, 120);
    int id = dm.AddMovie(movie);
    ASSERT_NE(id, 0);
    
    bool saved = dm.SaveToCsv();
    ASSERT_TRUE(saved);
    
    std::ifstream file(tempFile);
    ASSERT_TRUE(file.good());
    
    DeleteTempFile(tempFile);
}

TEST(DataManager_SaveToCsv_EmptyPath)
{
    DataManager dm;
    
    bool saved = dm.SaveToCsv();
    ASSERT_FALSE(saved);
}

TEST(DataManager_AddMovie)
{
    DataManager dm;
    
    Movie movie = CreateTestMovie(0, "New Movie", "New Studio", 2023, 9.0, 150);
    int newId = dm.AddMovie(movie);
    
    ASSERT_NE(newId, 0);
    ASSERT_EQ(dm.GetMovieCount(), 1);
    
    const Movie* retrieved = dm.GetMovieById(newId);
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->title, "New Movie");
    ASSERT_EQ(retrieved->studio, "New Studio");
    ASSERT_EQ(retrieved->year, 2023);
    ASSERT_EQ(retrieved->rating, 9.0);
    ASSERT_EQ(retrieved->length, 150);
}

TEST(DataManager_AddMovie_AutoIncrementId)
{
    DataManager dm;
    
    Movie movie1 = CreateTestMovie(0, "Movie 1");
    int id1 = dm.AddMovie(movie1);
    
    Movie movie2 = CreateTestMovie(0, "Movie 2");
    int id2 = dm.AddMovie(movie2);
    
    ASSERT_EQ(id2, id1 + 1);
}

TEST(DataManager_UpdateMovie_Success)
{
    DataManager dm;
    
    Movie movie = CreateTestMovie(0, "Original Title", "Original Studio", 2020, 7.0, 100);
    int id = dm.AddMovie(movie);
    
    Movie updatedData = CreateTestMovie(id, "Updated Title", "Updated Studio", 2021, 8.0, 120);
    bool updated = dm.UpdateMovie(id, updatedData);
    
    ASSERT_TRUE(updated);
    
    const Movie* retrieved = dm.GetMovieById(id);
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->title, "Updated Title");
    ASSERT_EQ(retrieved->studio, "Updated Studio");
    ASSERT_EQ(retrieved->year, 2021);
    ASSERT_EQ(retrieved->rating, 8.0);
    ASSERT_EQ(retrieved->length, 120);
}

TEST(DataManager_UpdateMovie_NotFound)
{
    DataManager dm;
    
    Movie updatedData = CreateTestMovie(999, "Updated Title");
    bool updated = dm.UpdateMovie(999, updatedData);
    
    ASSERT_FALSE(updated);
    ASSERT_EQ(dm.GetMovieCount(), 0);
}

TEST(DataManager_DeleteMovie_Success)
{
    DataManager dm;
    
    Movie movie = CreateTestMovie(0, "Movie to Delete");
    int id = dm.AddMovie(movie);
    ASSERT_EQ(dm.GetMovieCount(), 1);
    
    bool deleted = dm.DeleteMovie(id);
    
    ASSERT_TRUE(deleted);
    ASSERT_EQ(dm.GetMovieCount(), 0);
    
    const Movie* retrieved = dm.GetMovieById(id);
    ASSERT_TRUE(retrieved == nullptr);
}

TEST(DataManager_DeleteMovie_NotFound)
{
    DataManager dm;
    
    bool deleted = dm.DeleteMovie(999);
    
    ASSERT_FALSE(deleted);
    ASSERT_EQ(dm.GetMovieCount(), 0);
}

TEST(DataManager_GetMovieById_Found)
{
    DataManager dm;
    
    Movie movie = CreateTestMovie(0, "Find Me");
    int id = dm.AddMovie(movie);
    
    const Movie* retrieved = dm.GetMovieById(id);
    
    ASSERT_TRUE(retrieved != nullptr);
    ASSERT_EQ(retrieved->title, "Find Me");
    ASSERT_EQ(retrieved->id, id);
}

TEST(DataManager_GetMovieById_NotFound)
{
    DataManager dm;
    
    const Movie* retrieved = dm.GetMovieById(999);
    
    ASSERT_TRUE(retrieved == nullptr);
}

TEST(DataManager_GetAllMovies)
{
    DataManager dm;
    
    Movie movie1 = CreateTestMovie(0, "Movie 1");
    Movie movie2 = CreateTestMovie(0, "Movie 2");
    Movie movie3 = CreateTestMovie(0, "Movie 3");
    
    dm.AddMovie(movie1);
    dm.AddMovie(movie2);
    dm.AddMovie(movie3);
    
    const auto& allMovies = dm.GetAllMovies();
    
    ASSERT_EQ(allMovies.size(), 3);
    ASSERT_TRUE(allMovies[0] != nullptr);
    ASSERT_TRUE(allMovies[1] != nullptr);
    ASSERT_TRUE(allMovies[2] != nullptr);
}

TEST(DataManager_GetMovieCount)
{
    DataManager dm;
    
    ASSERT_EQ(dm.GetMovieCount(), 0);
    
    dm.AddMovie(CreateTestMovie(0, "Movie 1"));
    ASSERT_EQ(dm.GetMovieCount(), 1);
    
    dm.AddMovie(CreateTestMovie(0, "Movie 2"));
    ASSERT_EQ(dm.GetMovieCount(), 2);
    
    dm.DeleteMovie(1);
    ASSERT_EQ(dm.GetMovieCount(), 1);
}

TEST(DataManager_FindByTitlePrefix)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Inception", "WB", 2010));
    dm.AddMovie(CreateTestMovie(0, "Interstellar", "Paramount", 2014));
    dm.AddMovie(CreateTestMovie(0, "The Dark Knight", "WB", 2008));
    dm.AddMovie(CreateTestMovie(0, "Incredibles", "Pixar", 2004));
    
    std::vector<Movie*> results = dm.FindByTitlePrefix("Inc");
    
    ASSERT_EQ(results.size(), 2);
    for (Movie* movie : results) {
        ASSERT_EQ(movie->title.substr(0, 3), "Inc");
    }
}

TEST(DataManager_FindByTitlePrefix_NoMatch)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Inception"));
    
    std::vector<Movie*> results = dm.FindByTitlePrefix("XYZ");
    
    ASSERT_TRUE(results.empty());
}

TEST(DataManager_FindByStudioPrefix)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "Warner Bros"));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "Warner Pictures"));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "Paramount"));
    
    std::vector<Movie*> results = dm.FindByStudioPrefix("Warner");
    
    ASSERT_EQ(results.size(), 2);
    for (Movie* movie : results) {
        ASSERT_EQ(movie->studio.substr(0, 6), "Warner");
    }
}

TEST(DataManager_FindByYear)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2020));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2021));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2020));
    dm.AddMovie(CreateTestMovie(0, "Movie4", "", 2022));
    
    std::vector<Movie*> results = dm.FindByYear(2020);
    
    ASSERT_EQ(results.size(), 2);
    for (Movie* movie : results) {
        ASSERT_EQ(movie->year, 2020);
    }
}

TEST(DataManager_FilterByRatingRange)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2020, 5.0));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2020, 7.5));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2020, 8.5));
    dm.AddMovie(CreateTestMovie(0, "Movie4", "", 2020, 9.0));
    
    std::vector<Movie*> results = dm.FilterByRatingRange(7.0, 9.0);
    
    ASSERT_EQ(results.size(), 3);
    for (Movie* movie : results) {
        ASSERT_TRUE(movie->rating >= 7.0 && movie->rating <= 9.0);
    }
}

TEST(DataManager_FilterByLengthRange)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2020, 7.0, 90));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2020, 7.0, 120));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2020, 7.0, 150));
    dm.AddMovie(CreateTestMovie(0, "Movie4", "", 2020, 7.0, 180));
    
    std::vector<Movie*> results = dm.FilterByLengthRange(100, 160);
    
    ASSERT_EQ(results.size(), 2);
    for (Movie* movie : results) {
        ASSERT_TRUE(movie->length >= 100 && movie->length <= 160);
    }
}

TEST(DataManager_SearchByDescription)
{
    DataManager dm;
    
    Movie movie1 = CreateTestMovie(0, "Movie1");
    movie1.description = "This is a great action movie";
    dm.AddMovie(movie1);
    
    Movie movie2 = CreateTestMovie(0, "Movie2");
    movie2.description = "This is a romantic comedy";
    dm.AddMovie(movie2);
    
    Movie movie3 = CreateTestMovie(0, "Movie3");
    movie3.description = "action packed thriller";
    dm.AddMovie(movie3);
    
    std::vector<Movie*> results = dm.SearchByDescription("action");
    
    ASSERT_EQ(results.size(), 2);
}

TEST(DataManager_FilterByGenre)
{
    DataManager dm;
    
    Movie movie1 = CreateTestMovie(0, "Movie1");
    movie1.genres = {"Action", "Drama"};
    dm.AddMovie(movie1);
    
    Movie movie2 = CreateTestMovie(0, "Movie2");
    movie2.genres = {"Comedy", "Romance"};
    dm.AddMovie(movie2);
    
    Movie movie3 = CreateTestMovie(0, "Movie3");
    movie3.genres = {"Action", "Sci-Fi"};
    dm.AddMovie(movie3);
    
    std::vector<Movie*> results = dm.FilterByGenre("Action");
    
    ASSERT_EQ(results.size(), 2);
}

TEST(DataManager_GetSortedByRating)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2020, 5.0));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2020, 9.0));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2020, 7.0));
    
    const std::vector<Movie*>& ascending = dm.GetSortedByRating(true);
    ASSERT_EQ(ascending.size(), 3);
    ASSERT_EQ(ascending[0]->rating, 5.0);
    ASSERT_EQ(ascending[1]->rating, 7.0);
    ASSERT_EQ(ascending[2]->rating, 9.0);
    
    const std::vector<Movie*>& descending = dm.GetSortedByRating(false);
    ASSERT_EQ(descending[0]->rating, 9.0);
    ASSERT_EQ(descending[1]->rating, 7.0);
    ASSERT_EQ(descending[2]->rating, 5.0);
}

TEST(DataManager_GetSortedByYear)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2010));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2020));
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2000));
    
    const std::vector<Movie*>& ascending = dm.GetSortedByYear(true);
    ASSERT_EQ(ascending.size(), 3);
    ASSERT_EQ(ascending[0]->year, 2000);
    ASSERT_EQ(ascending[1]->year, 2010);
    ASSERT_EQ(ascending[2]->year, 2020);
}

TEST(DataManager_GetSortedByTitle)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Zebra"));
    dm.AddMovie(CreateTestMovie(0, "Alpha"));
    dm.AddMovie(CreateTestMovie(0, "Beta"));
    
    const std::vector<Movie*>& ascending = dm.GetSortedByTitle(true);
    ASSERT_EQ(ascending.size(), 3);
    ASSERT_EQ(ascending[0]->title, "Alpha");
    ASSERT_EQ(ascending[1]->title, "Beta");
    ASSERT_EQ(ascending[2]->title, "Zebra");
}

TEST(DataManager_SortCache_InvalidatedOnAdd)
{
    DataManager dm;
    
    dm.AddMovie(CreateTestMovie(0, "Movie1", "", 2000, 5.0));
    dm.AddMovie(CreateTestMovie(0, "Movie2", "", 2010, 9.0));
    
    const std::vector<Movie*>& firstSort = dm.GetSortedByRating(true);
    ASSERT_EQ(firstSort.size(), 2);
    
    dm.AddMovie(CreateTestMovie(0, "Movie3", "", 2005, 7.0));
    
    const std::vector<Movie*>& secondSort = dm.GetSortedByRating(true);
    
    ASSERT_EQ(secondSort.size(), 3);
    ASSERT_EQ(secondSort[0]->rating, 5.0);
    ASSERT_EQ(secondSort[1]->rating, 7.0);
    ASSERT_EQ(secondSort[2]->rating, 9.0);
}

TEST(DataManager_ChainOperations)
{
    DataManager dm;
    
    int id1 = dm.AddMovie(CreateTestMovie(0, "Movie1", "Studio A", 2020, 8.0, 120));
    int id2 = dm.AddMovie(CreateTestMovie(0, "Movie2", "Studio B", 2021, 9.0, 130));
    
    ASSERT_EQ(dm.GetMovieCount(), 2);
    
    Movie updated = CreateTestMovie(id1, "Updated Movie1", "Studio A", 2022, 8.5, 125);
    bool updatedOk = dm.UpdateMovie(id1, updated);
    ASSERT_TRUE(updatedOk);
    
    const Movie* retrieved = dm.GetMovieById(id1);
    ASSERT_EQ(retrieved->title, "Updated Movie1");
    ASSERT_EQ(retrieved->year, 2022);
    
    std::vector<Movie*> found = dm.FindByTitlePrefix("Updated");
    ASSERT_EQ(found.size(), 1);
    
    bool deleted = dm.DeleteMovie(id2);
    ASSERT_TRUE(deleted);
    ASSERT_EQ(dm.GetMovieCount(), 1);
}