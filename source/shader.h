#pragma once

#define NOMINMAX
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace ntn
{
class Shader
{
public:
    unsigned int ID=0;

    // ------------------------------------------------------------------------
    Shader() = default;
    Shader(const char* vertexPath, const char* fragmentPath, 
           const char* geometryPath=nullptr, 
           const char* tessControlPath = nullptr, 
           const char* tessEvalPath = nullptr);

    // ------------------------------------------------------------------------
    void activate()
    {
        glUseProgram(ID);
    }
    // Uniform setter functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    void setSampler2D(const std::string& name, unsigned int texture, int id) const;

    // ------------------------------------------------------------------------
    void setMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& project) const;

private:
    // Utility function for compiling shaders and checking errors
    unsigned int compileShader(const char* shaderCode, GLenum shaderType);
    void linkProgram(unsigned int vertexShader, unsigned int fragmentShader, unsigned int geometryShader = 0, unsigned int tessControlShader = 0, unsigned int tessEvalShader = 0);

    std::string getShaderPath(const std::string& filename);

    std::string readShaderFile(const std::string& filePath);

    void checkCompileErrors(GLuint shader, std::string type);
};
}