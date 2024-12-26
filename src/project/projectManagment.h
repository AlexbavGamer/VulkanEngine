#pragma once
#include <string>
#include <vector>
#include <filesystem>

struct ProjectConfig {
    std::string projectName;
    std::string projectPath;
};

class VulkanCore;

class ProjectManager {
    public:
        ProjectManager(VulkanCore* core);
        bool createProject(const std::string& projectName, const std::string& projectPath);
        bool openProject(const std::string& projectPath);
        bool saveProject();
        const ProjectConfig& getConfig() const { return m_config; }
    private:
        ProjectConfig m_config;
        VulkanCore* vulkanCore;
        bool createProjectStructure();
        bool validateProjectStructure();
};