#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>

class FileManager {
    public:
        static FileManager& getInstance()
        {
            static FileManager instance;
            return instance;
        }
    bool writeTextFile(const std::string& filename, const std::string& content);
    std::string readTextFile(const std::string& filename);

    template <typename T>
    bool writeBinaryFile(const std::string& filename, const T& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
        return true;
    }

    template <typename T>
    bool writeBinaryArray(const std::string& filename, const std::vector<T>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
        return true;
    }

    template <typename T>
    bool readBinaryFile(const std::string& filename, T& data) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.read(reinterpret_cast<char*>(&data), sizeof(T));
        return true;
    }

    template <typename T>
    bool readBinaryArray(const std::string& filename, std::vector<T>& data) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(T));
        return true;
    }

    bool createDirectory(const std::string& path);
    bool deleteFile(const std::string& path);
    bool fileExists(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory);
    size_t getFileSize(const std::string& path);
    std::string getFileExtension(const std::string& path);
private:
    FileManager() = default;
};