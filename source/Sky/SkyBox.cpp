#include"SkyBox.h"
#include"../resourceManager.h"
#include"../stb_image.h"

namespace ntn
{

Skybox::Skybox() : AbstractSky() 
{ 
    initSkyBox(); 
}

void Skybox::initSkyBox()
{
    std::vector<Vertex> vertices = generateCubeVertices();
    std::vector<unsigned int> indices = generateCubeIndices();
    std::vector<std::string> texturesFaces = 
    {
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/right.png"),
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/left.png"),
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/top.png"),
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/bottom.png"),
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/back.png"),
        ResourceManager::getInstance().getResourcePath("skybox/skybox1/front.png"),
    };
    std::vector<Texture> texturesLoaded = loadTextures(texturesFaces);

    m_skyMesh = std::make_unique<Mesh>(vertices, indices, texturesLoaded);
}

void Skybox::render(Shader& shaderSkyBox)
{
    shaderSkyBox.activate();
    m_skyMesh->renderSkyBox(shaderSkyBox);
}

std::vector<Vertex> Skybox::generateCubeVertices() const
{
    /*
      4------ 7
     /|      /|
    / |     / |
    0------ 3 |
    | |     | |
    | 5---- |--6
    | /     | /
    |/      |/
    1------ 2   
    */

    glm::vec3 vertexPositions[8] =
    {
        glm::vec3(-1.0f,  1.0f,  1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f),
    };

    std::vector<Vertex> vertices;
    vertices.reserve(8);
    for (const glm::vec3& pos : vertexPositions)
    {
        Vertex vertex;
        vertex.Position = pos;
        vertices.emplace_back(vertex);
    }
    return vertices;
}

std::vector<unsigned int> Skybox::generateCubeIndices() const
{
    return 
    {
        // Front
        0, 1, 2,
        0, 2, 3,
        // Back
        4, 5, 6,
        4, 6, 7,
        // Left 
        0, 4, 3,
        3, 4, 7,
        // Right
        1, 5, 2,
        2, 5, 6,
        // Top 
        3, 2, 7,
        2, 6, 7,
        // Bottom
        0, 1, 4,
        1, 5, 4
    };
}

std::vector<Texture> Skybox::loadTextures(std::vector<std::string>& texturesFaces)
{
    std::vector<Texture> texturesLoaded;
    texturesLoaded.reserve(6);
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Load the skybox texture
    for (size_t i = 0; i < texturesFaces.size(); ++i)
    {
        int width, height, nbrChannels;
        unsigned char* data = stbi_load(texturesFaces[i].c_str(), &width, &height, &nbrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nbrChannels == 1)
                format = GL_RED;
            else if (nbrChannels == 3)
                format = GL_RGB;
            else if (nbrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
                            width, height, 0, format, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load skybox texture: " << texturesFaces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Create a Texture object for each face of the skybox
    for (size_t i = 0; i < texturesFaces.size(); ++i)
    {
        texturesLoaded.emplace_back(Texture(textureID, "skybox", texturesFaces[i]));
    }

    return texturesLoaded;
}
}