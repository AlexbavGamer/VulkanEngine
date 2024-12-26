#include "projectManagment.h"
#include <fstream>

ProjectManager::ProjectManager(VulkanCore *core) : vulkanCore(core)
{

}

bool ProjectManager::createProject(const std::string &projectName, const std::string &projectPath)
{
    m_config.projectName = projectName;
    m_config.projectPath = projectPath;

    return createProjectStructure();
}

bool ProjectManager::openProject(const std::string &projectPath)
{
    m_config.projectPath = projectPath;

    if (!validateProjectStructure())
        return false;

    std::ifstream configFile(projectPath + "/project.config");
    std::getline(configFile, m_config.projectName);
    
    return true;
}

bool ProjectManager::saveProject()
{
    try
    {
        std::ofstream configFile(m_config.projectPath + "/project.config");
        configFile << m_config.projectName << "\n";
        configFile << m_config.projectPath << "\n";
        return true;
    }
    catch(...)
    {
        return false;
    }
}
bool ProjectManager::createProjectStructure()
{
    try
    {
        std::filesystem::create_directories(m_config.projectPath);
        std::filesystem::create_directories(m_config.projectPath + "/assets");

        std::ofstream configFile(m_config.projectPath + "/project.config");
        configFile << m_config.projectName << "\n";
        configFile << m_config.projectPath << "\n";
        return true;
    }
    catch(...)
    {
        return false;
    }
    
}

bool ProjectManager::validateProjectStructure()
{
    try
    {
        if (!std::filesystem::exists(m_config.projectPath))
            return false;

        if (!std::filesystem::exists(m_config.projectPath + "/assets"))
            return false;

        if (!std::filesystem::exists(m_config.projectPath + "/project.config"))
            return false;

        std::ifstream configFile(m_config.projectPath + "/project.config");
        if (!configFile.is_open())
            return false;

        std::string projectName, projectPath;
        std::getline(configFile, projectName);
        std::getline(configFile, projectPath);

        if (projectName.empty() || projectPath.empty())
            return false;

        if (projectPath != m_config.projectPath)
            return false;

        return true;
    }
    catch(...)
    {
        return false;
    }
}