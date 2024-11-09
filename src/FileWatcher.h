#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <chrono>

class FileWatcher {
public:
    FileWatcher() = default;

    void addPath(const std::string& path) {
        paths[path] = std::filesystem::last_write_time(path);
    }

    std::vector<std::string> getChangedFiles() {
        std::vector<std::string> changedFiles;
        
        for (auto& [path, lastWriteTime] : paths) {
            auto currentTime = std::filesystem::last_write_time(path);
            if (currentTime != lastWriteTime) {
                changedFiles.push_back(path);
                lastWriteTime = currentTime;
            }
        }
        
        return changedFiles;
    }

private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths;
};
