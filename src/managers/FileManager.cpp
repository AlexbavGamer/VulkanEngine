#include "FileManager.h"
#include <fstream>
#include <filesystem>
#include <sstream>

bool FileManager::writeTextFile(const std::string &filename, const std::string &content)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

std::string FileManager::readTextFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

bool FileManager::createDirectory(const std::string &path)
{
    return std::filesystem::create_directories(path);
}

bool FileManager::deleteFile(const std::string &path)
{
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

bool FileManager::fileExists(const std::string &path)
{
    return std::filesystem::exists(path);
}

std::vector<std::string> FileManager::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        files.push_back(entry.path().filename().string());
    }
    return files;
}

std::string FileManager::getFileExtension(const std::string &path)
{
    std::filesystem::path filePath(path);
    return filePath.extension().string();
}
