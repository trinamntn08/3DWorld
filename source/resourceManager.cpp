#include "resourceManager.h"
#include <cstdlib> // For getenv
#include <cerrno>  // For errno_t
#include <cstring> // For _dupenv_s

namespace ntn
{

// Path separator for cross-platform compatibility
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif


    ResourceManager& ResourceManager::getInstance() 
    {
        static ResourceManager instance; 
        return instance;
    }


    ResourceManager::ResourceManager() 
    {
        shaderDir = getEnvVariable("SHADER_DIR");
        resourcesDir = getEnvVariable("RESOURCES_DIR");
    }

    std::string ResourceManager::getShaderPath(const std::string& filename) const 
    {
        std::string fullPath = shaderDir + PATH_SEPARATOR + filename;

        // Check if the file exists
        if (!std::filesystem::exists(fullPath)) 
        {
            throw std::runtime_error("Shader file not found: " + fullPath);
        }

        return fullPath;
    }

    std::string ResourceManager::getResourcePath(const std::string& filename) const 
    {
        std::string fullPath = resourcesDir + PATH_SEPARATOR + filename;

        // Check if the file exists
        if (!std::filesystem::exists(fullPath)) 
        {
            throw std::runtime_error("Resource file not found: " + fullPath);
        }

        return fullPath;
    }

    std::string ResourceManager::getEnvVariable(const std::string& varName) const 
    {
        char* value = nullptr;
        size_t len = 0;
        errno_t err = _dupenv_s(&value, &len, varName.c_str());

        if (value != nullptr) 
        {
            std::string result(value);
            free(value); 
            return result;
        }
        else 
        {
            return std::filesystem::current_path().string();
        }
    }
}