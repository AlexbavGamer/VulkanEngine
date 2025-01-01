#include "projectManagment.h"
#include "../core/VulkanCore.h"
#include "../managers/FileManager.h"
#include <cstring>
#include <fstream>
#include <iostream>

ProjectManager::ProjectManager(VulkanCore *core) : vulkanCore(core)
{
}

bool ProjectManager::createProject(const std::string &projectName, const std::string &projectPath)
{
    m_config.projectName = projectName;
    m_config.projectPath = projectPath;

    if (!createProjectStructure())
        return false;

    m_isProjectOpen = true;

    return true;
}

bool ProjectManager::openProject(const std::string &projectPath)
{
    m_config.projectPath = projectPath;

    std::cout << m_config.projectPath << std::endl;

    if (!validateProjectStructure())
        return false;

    if (FileManager::getInstance().readBinaryFile(projectPath + "/project.config", m_config))
    {
        m_isProjectOpen = true;
        return true;
    }

    return false;
}

bool ProjectManager::saveProject()
{
    try
    {
        FileManager::getInstance().writeBinaryFile(m_config.projectPath + "/project.config", m_config);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectManager::closeProject()
{
    if (!m_isProjectOpen)
        return true;

    if (!saveProject())
        return false;

    m_config.projectName.clear();
    m_config.projectPath.clear();
    m_isProjectOpen = false;
    
    return true;
}

bool ProjectManager::createProjectStructure()
{
    try
    {
        std::filesystem::create_directories(m_config.projectPath);
        std::filesystem::create_directories(m_config.projectPath + "/Assets");

        FileManager::getInstance().writeBinaryFile(m_config.projectPath + "/project.config", m_config);

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ProjectManager::validateProjectStructure()
{
    try
    {
        if (!std::filesystem::exists(m_config.projectPath) ||
            !std::filesystem::exists(m_config.projectPath + "/Assets") ||
            !std::filesystem::exists(m_config.projectPath + "/project.config"))
            return false;

        ProjectConfig tempConfig;
        if (!FileManager::getInstance().readBinaryFile(m_config.projectPath + "/project.config", tempConfig))
            return false;

        if (tempConfig.projectName.empty() || tempConfig.projectPath.empty())
            return false;

        return tempConfig.projectPath == m_config.projectPath;
    }
    catch (...)
    {
        return false;
    }
}