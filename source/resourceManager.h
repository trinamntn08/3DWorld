#pragma once

#include <string>
#include <filesystem>
#include <stdexcept>

namespace ntn
{

#include <string>
#include <filesystem>
#include <stdexcept>

    class ResourceManager 
    {
    public:

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        // Static method to get the single instance of ResourceManager
        static ResourceManager& getInstance();

        // Get shader file path
        std::string getShaderPath(const std::string& filename) const;

        // Get resource file path
        std::string getResourcePath(const std::string& filename) const;

    private:
        // Private constructor to prevent instantiation
        ResourceManager();

        std::string shaderDir;
        std::string resourcesDir;

        // Helper function to retrieve environment variable
        std::string getEnvVariable(const std::string& varName) const;
    };
}
