/// @file DataManager.cpp
/// @brief Реализация центрального менеджера данных фильмов.

#include "core/DataManager.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"
#include "core/storage/MovieCsvMapper.h"

#include <algorithm>

namespace FilmLibrary
{
    DataManager::DataManager() = default;
    DataManager::~DataManager() = default;

    std::vector<std::string> DataManager::LoadFromCsv(const std::string& filePath)
    {
         csvFilePath = filePath;
        std::vector<std::string> errors;

        auto result = CsvParser::LoadFromFile<Movie, MovieCsvMapper>(filePath);

        errors = result.errors;
        
        if (!result.errors.empty())
        {
            Logger::Instance().Warning("Loaded movies with errors: " + std::to_string(result.errors.size()));
            for (const auto& err : result.errors)
            {
                Logger::Instance().Error(err);
            }
        }

        movies = std::move(result.records);

        int maxId = 0;
        for (const auto& movie : movies)
        {
            if (movie && movie->id > maxId)
            {
                maxId = movie->id;
            }
        }
        nextId = maxId + 1;
        
        std::vector<Movie*> pointers;
        pointers.reserve(movies.size());
        for (const auto& movie : movies)
        {
            if (movie)
            {
                pointers.push_back(movie.get());
            }
        }

        indexManager.Rebuild(movies);
        sortCache.Invalidate();

        Logger::Instance().Info("Loaded " + std::to_string(movies.size()) + " movies from " + filePath);
        
        return errors;
    }

    bool DataManager::SaveToCsv() const
    {
        if (csvFilePath.empty())
        {
            Logger::Instance().Error("Cannot save movies: CSV file path is empty");
            return false;
        }
        
        bool success = CsvParser::SaveToFile<Movie, MovieCsvMapper>(csvFilePath, movies);
        
        if (success)
        {
            Logger::Instance().Info("Saved " + std::to_string(movies.size()) + " movies to " + csvFilePath);
        }
        else
        {
            Logger::Instance().Error("Failed to save movies to " + csvFilePath);
        }
        
        return success;
    }

    int DataManager::AddMovie(Movie movieData)
    {
        movieData.id = nextId++;
        
        auto movie = std::make_unique<Movie>(std::move(movieData));
        int newId = movie->id;
        movies.push_back(std::move(movie));
        
        OnDataChanged();
        
        Logger::Instance().Info("Added movie with ID: " + std::to_string(newId));
        return newId;
    }

    bool DataManager::UpdateMovie(int id, const Movie& updatedData)
    {
        Movie* movie = indexManager.GetMovieById(id);
        
        if (!movie)
        {
            Logger::Instance().Warning("Cannot update movie: ID " + std::to_string(id) + " not found");
            return false;
        }
        
        movie->title = updatedData.title;
        movie->studio = updatedData.studio;
        movie->description = updatedData.description;
        movie->year = updatedData.year;
        movie->length = updatedData.length;
        movie->rating = updatedData.rating;
        movie->cover = updatedData.cover;
        movie->streamLink = updatedData.streamLink;
        movie->genres = updatedData.genres;
        movie->actorIds = updatedData.actorIds;
        
        OnDataChanged();
        
        Logger::Instance().Info("Updated movie with ID: " + std::to_string(id));
        return true;
    }
    

    bool DataManager::DeleteMovie(int id)
    {
        Movie* moviePtr = indexManager.GetMovieById(id);
        
        if (!moviePtr)
        {
            Logger::Instance().Warning("Cannot delete movie: ID " + std::to_string(id) + " not found");
            return false;
        }
        
        for (auto it = movies.begin(); it != movies.end(); ++it)
        {
            if (it->get() == moviePtr)
            {
                movies.erase(it);
                break;
            }
        }
        
        OnDataChanged();
        
        Logger::Instance().Info("Deleted movie with ID: " + std::to_string(id));
        return true;
    }

    const Movie* DataManager::GetMovieById(int id) const
    {
        return indexManager.GetMovieById(id);
    }
    

    const std::vector<std::unique_ptr<Movie>>& DataManager::GetAllMovies() const
    {
        return movies;
    }

    std::size_t DataManager::GetMovieCount() const
    {
        return movies.size();
    }

    std::vector<Movie*> DataManager::FindByTitlePrefix(const std::string& prefix) const
    {
        return indexManager.FindByTitlePrefix(prefix);
    }

    std::vector<Movie*> DataManager::FindByStudioPrefix(const std::string& prefix) const
    {
        return indexManager.FindByStudioPrefix(prefix);
    }

    std::vector<Movie*> DataManager::FindByYear(int year) const
    {
        return indexManager.FindByYear(year);
    }

    std::vector<Movie*> DataManager::FilterByRatingRange(double low, double high) const
    {
        return indexManager.FilterByRatingRange(low, high);
    }

    std::vector<Movie*> DataManager::FilterByLengthRange(int low, int high) const
    {
        return indexManager.FilterByLengthRange(low, high);
    }

    std::vector<Movie*> DataManager::SearchByDescription(const std::string& pattern) const
    {
        return indexManager.SearchByDescription(movies, pattern);
    }

    std::vector<Movie*> DataManager::FilterByGenre(const std::string& genre) const
    {
        return indexManager.FilterByGenre(movies, genre);
    }

    const std::vector<Movie*>& DataManager::GetSortedByRating(bool ascending)
    {
        std::string key = "rating_" + std::to_string(ascending);
            
        auto comparator = [ascending](const Movie* a, const Movie* b) -> bool {
            return ascending ? a->rating < b->rating : a->rating > b->rating;
        };
            
        return sortCache.GetSorted(key, movies, comparator);
    }

    const std::vector<Movie*>& DataManager::GetSortedByYear(bool ascending)
    {
        std::string key = "year_" + std::to_string(ascending);
        
        auto comparator = [ascending](const Movie* a, const Movie* b) -> bool {
            return ascending ? a->year < b->year : a->year > b->year;
        };
        
        return sortCache.GetSorted(key, movies, comparator);
    }

    const std::vector<Movie*>& DataManager::GetSortedByTitle(bool ascending)
    {
        std::string key = "title_" + std::to_string(ascending);
        
        auto comparator = [ascending](const Movie* a, const Movie* b) -> bool {
            return ascending ? a->title < b->title : a->title > b->title;
        };
        
        return sortCache.GetSorted(key, movies, comparator);
    }

    void DataManager::OnDataChanged()
    {
        indexManager.Rebuild(movies);
        
        sortCache.Invalidate();
        
        SaveToCsv();
    }
}