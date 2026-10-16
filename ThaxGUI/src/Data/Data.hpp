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

    inline std::optional<Config> LoadConfig()
    {
        std::ifstream file{"data/internal/config.txt"};
        if (!file)
        {
            return std::nullopt;
        }

        Config config {};

        uint32_t n = 0;
        file.read(reinterpret_cast<char*>(&n), sizeof(n));
        std::string s(n, '\0');
        file.read(s.data(), n);
        config.rom_folder = s;

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

        const std::string rom_folder_string = config.rom_folder.string();
        const auto n = static_cast<uint32_t>(rom_folder_string.size());
        file.write(reinterpret_cast<const char*>(&n), sizeof(n));
        file.write(rom_folder_string.data(), n);

        file.close();
    }
}
