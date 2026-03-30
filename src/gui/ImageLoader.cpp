/// @file ImageLoader.cpp
/// @brief Реализация ImageLoader.

#include "gui/ImageLoader.h"
#include "core/utils/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../third_party/stb/stb_image.h"
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <filesystem>
#include <cstdlib>
#include <cstdio>
#include <functional>

namespace fs = std::filesystem;

namespace FilmLibrary
{
    struct TexEntry
    {
        std::atomic<bool> loading{false};
        std::atomic<bool> ready{false};
        std::atomic<bool> failed{false};
        unsigned char* data = nullptr;
        int w = 0;
        int h = 0;
        GLuint tex = 0;
        std::mutex mtx;
        int retries = 0;
        static const int MAX_RETRIES = 2;

        void StartLoad(const std::string& url)
        {
            if (loading || ready || tex) return;
            if (failed && retries >= MAX_RETRIES) return;
            failed = false;
            loading = true;
            ++retries;

            std::thread([this, url]()
            {
                FILE* f = fopen(url.c_str(), "rb");
                if (f)
                {
                    fclose(f);
                    unsigned char* d = stbi_load(url.c_str(), &w, &h, nullptr, 4);
                    std::lock_guard<std::mutex> lk(mtx);
                    if (d) { data = d; ready = true; }
                    else { failed = true; }
                    loading = false;
                    return;
                }

                std::error_code ec;
                fs::create_directories("cache", ec);

                size_t hash = std::hash<std::string>{}(url);
                std::string cachePath = "cache/fl_img_" + std::to_string(hash) + ".png";

                if (!fs::exists(cachePath, ec))
                {
                    std::string cmd = "curl -s -L --max-time 10 -o \"" + cachePath + "\" \"" + url + "\"";
                    int r = std::system(cmd.c_str());
                    if (r != 0) 
                    {
                        Logger::Instance().Error("Не удалось скачать изображение: " + url);
                    }
                }

                unsigned char* d = stbi_load(cachePath.c_str(), &w, &h, nullptr, 4);
                std::lock_guard<std::mutex> lk(mtx);
                if (d) { data = d; ready = true; }
                else { failed = true; }
                loading = false;
            }).detach();
        }

        void UploadGL()
        {
            std::lock_guard<std::mutex> lk(mtx);
            if (!ready || !data || tex) return;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            data = nullptr;
            ready = false;
        }
    };


    static std::unordered_map<std::string, TexEntry*> g_TexCache;
    static std::mutex g_TexCacheMutex;

    uint32_t ImageLoader::GetOrLoadTexture(const std::string& url)
    {
        if (url.empty()) return 0;

        std::lock_guard<std::mutex> lock(g_TexCacheMutex);
        auto it = g_TexCache.find(url);
        if (it == g_TexCache.end())
        {
            TexEntry* e = new TexEntry();
            g_TexCache[url] = e;
            e->StartLoad(url);
            return 0;
        }
        TexEntry* e = it->second;
        e->UploadGL();
        return (uint32_t)e->tex;
    }

    bool ImageLoader::GetTextureSize(const std::string& url, int& outWidth, int& outHeight)
    {
        std::lock_guard<std::mutex> lock(g_TexCacheMutex);
        auto it = g_TexCache.find(url);
        if (it != g_TexCache.end())
        {
            if (it->second->tex != 0 || it->second->ready)
            {
                outWidth = it->second->w;
                outHeight = it->second->h;
                return true;
            }
        }
        return false;
    }

    void ImageLoader::Shutdown()
    {
        std::lock_guard<std::mutex> lock(g_TexCacheMutex);
        for (auto& pair : g_TexCache)
        {
            TexEntry* e = pair.second;
            if (e->tex)
                glDeleteTextures(1, &e->tex);
            if (e->data)
                stbi_image_free(e->data);
            delete e;
        }
        g_TexCache.clear();
    }
}