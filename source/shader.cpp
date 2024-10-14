#include "shader.h"
#include"resourceManager.h"
#include <string>
#include <filesystem>

namespace ntn
{

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

Shader::Shader(const char* vertexFileName, const char* fragmentFileName, const char* geometryFileName,
                const char* tessControlFileName, const char* tessEvalFileName)
{
    // 1. Retrieve the full paths for shader files
    std::string vertexFilePath, fragmentFilePath, geometryFilePath, tessControlFilePath, tessEvalFilePath;

    try 
    {
        vertexFilePath = getShaderPath(vertexFileName);
        fragmentFilePath = getShaderPath(fragmentFileName);
        if (geometryFileName != nullptr) 
        {
            geometryFilePath = getShaderPath(geometryFileName);
        }
        if (tessControlFileName != nullptr) 
        {
            tessControlFilePath = getShaderPath(tessControlFileName);
        }
        if (tessEvalFileName != nullptr) 
        {
            tessEvalFilePath = getShaderPath(tessEvalFileName);
        }
    }
    catch (const std::exception& e) 
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << e.what() << std::endl;
        return;
    }

    // 2. Read shader files
    std::string vertexCode, fragmentCode, geometryCode, tessControlCode, tessEvalCode;

    try 
    {
        vertexCode = readShaderFile(vertexFilePath);
        fragmentCode = readShaderFile(fragmentFilePath);
        if (!geometryFilePath.empty()) 
        {
            geometryCode = readShaderFile(geometryFilePath);
        }
        if (!tessControlFilePath.empty()) 
        {
            tessControlCode = readShaderFile(tessControlFilePath);
        }
        if (!tessEvalFilePath.empty()) 
        {
            tessEvalCode = readShaderFile(tessEvalFilePath);
        }
    }
    catch (const std::ifstream::failure& e) 
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return;
    }

    // 3. Compile shaders
    unsigned int vertex = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
    unsigned int geometry = 0, tessControl = 0, tessEval = 0;

    if (!geometryCode.empty()) 
    {
        geometry = compileShader(geometryCode.c_str(), GL_GEOMETRY_SHADER);
    }
    if (!tessControlCode.empty()) 
    {
        tessControl = compileShader(tessControlCode.c_str(), GL_TESS_CONTROL_SHADER);
    }
    if (!tessEvalCode.empty()) 
    {
        tessEval = compileShader(tessEvalCode.c_str(), GL_TESS_EVALUATION_SHADER);
    }

    // 4. Link shader program
    linkProgram(vertex, fragment, geometry, tessControl, tessEval);

}
// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setSampler2D(const std::string& name, unsigned int texture, int id) const
{
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);
    this->setInt(name, id);
}
// ------------------------------------------------------------------------
void Shader::setMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& project) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &project[0][0]);
}
unsigned int Shader::compileShader(const char* shaderCode, GLenum shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" :
                               shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" :
                               shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
                               shaderType == GL_TESS_CONTROL_SHADER ? "TESS_CONTROL" : "TESS_EVALUATION");
    return shader;
}
// ------------------------------------------------------------------------
void Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader, 
                         unsigned int geometryShader, 
                         unsigned int tessControlShader, unsigned int tessEvalShader)
{
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    if (geometryShader)
    {
        glAttachShader(ID, geometryShader);
    }
    if (tessControlShader)
    {
        glAttachShader(ID, tessControlShader);
    }
    if (tessEvalShader)
    {
        glAttachShader(ID, tessEvalShader);
    }
    glLinkProgram(ID);

    checkCompileErrors(ID, "PROGRAM");

    // Delete the shaders as they're linked into the program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader)
    {
        glDeleteShader(geometryShader);
    }
    if (tessControlShader)
    {
        glDeleteShader(tessControlShader);
    }
    if (tessEvalShader)
    {
        glDeleteShader(tessEvalShader);
    }
}
// ------------------------------------------------------------------------
std::string Shader::getShaderPath(const std::string& filename)
{
    // Construct the full path
    std::string fullPath = ResourceManager::getInstance().getShaderPath(filename);

    // Check if the file exists
    if (!std::filesystem::exists(fullPath))
    {
        throw std::runtime_error("Shader file not found: " + fullPath);
    }

    return fullPath;
}
// ------------------------------------------------------------------------
std::string Shader::readShaderFile(const std::string& filePath)
{
    std::ifstream shaderFile(filePath);
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
}