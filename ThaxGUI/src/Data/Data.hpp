#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    struct ROMEntry
    {
        std::string name;
        std::filesystem::path path;
        ROMEntry* parent = nullptr;
        std::vector<std::unique_ptr<ROMEntry>> children;
        bool is_directory = false;
        bool closed_directory = false;
    };

    struct Config
    {
        std::filesystem::path rom_folder {""};
        int bootrom_load_option = 0; // 0 - Default values 1 - specify bootrom file
        std::filesystem::path bootrom_file {""};
    };

    inline void ScanDirectory(const std::filesystem::path& path, ROMEntry* parent)
    {
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            if (file.path().extension() != ".gb" && !file.is_directory())
                continue;

            auto root_entry = std::make_unique<ROMEntry>();
            root_entry->name = file.path().filename().string();
            root_entry->path = file.path();
            root_entry->parent = parent;
            root_entry->is_directory = file.is_directory();

            ROMEntry* root = root_entry.get();
            parent->children.push_back(std::move(root_entry));

            if (file.is_directory())
                ScanDirectory(file.path(), root);
        }
    }

    inline std::unique_ptr<ROMEntry> SearchROMs(const std::filesystem::path& path)
    {
        if (!std::filesystem::is_directory(path))
        {
            DMG_WARN("Trying to load ROMs from not a folder or this folder doesn't exist.");
            return nullptr;
        }

        auto root_entry = std::make_unique<ROMEntry>();
        root_entry->name = path.filename().string();
        root_entry->path = path;
        root_entry->parent = nullptr;
        root_entry->is_directory = true;

        ROMEntry* root = root_entry.get();

        ScanDirectory(path, root);

        return std::move(root_entry);
    }

    inline std::string LoadString(std::ifstream& f)
    {
        uint32_t n = 0;
        f.read(reinterpret_cast<char*>(&n), sizeof(n));
        std::string s(n, '\0');
        f.read(s.data(), n);
        return s;
    }

    inline void SaveString(std::ofstream& f, const std::string& s)
    {
        const auto n = static_cast<uint32_t>(s.size());
        f.write(reinterpret_cast<const char*>(&n), sizeof(n));
        f.write(s.data(), n);
    }

    inline std::optional<Config> LoadConfig()
    {
        std::ifstream file{"data/internal/config.txt"};
        if (!file)
        {
            return std::nullopt;
        }

        Config config {};

        config.rom_folder = LoadString(file);
        file >> config.bootrom_load_option;
        config.bootrom_file = LoadString(file);

        return config;
    }

    inline void SaveConfig(const Config& config)
    {
        std::filesystem::create_directories("data/internal");
        std::ofstream file{"data/internal/config.txt"};
        if (!file)
        {
            DMG_ERROR("Could not create config.txt file for writing.");
            return;
        }

        SaveString(file, config.rom_folder.string());
        file << config.bootrom_load_option;
        SaveString(file, config.bootrom_file.string());

        file.close();
    }
}
