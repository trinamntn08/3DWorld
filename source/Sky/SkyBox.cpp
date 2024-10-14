#include"SkyBox.h"
#include"../resourceManager.h"
#include"../stb_image.h"

namespace ntn
{

static std::string path_skyBox = "skybox/skybox1/";

void Skybox::InitSkyBox()
{
    // cube vertices for vertex buffer object
    std::vector<Vertex> vertices;
    /*7------ 6
     /|      /|
    / |     / |
    3------ 2 |
    | |     | |
    | 4---- |--5
    | /     | /
    |/      |/
    0------ 1   */
    glm::vec3 vertexPositions[8] =
    {
      glm::vec3(-1.0,  1.0,  1.0),
      glm::vec3(-1.0, -1.0,  1.0),
      glm::vec3(1.0, -1.0,  1.0),
      glm::vec3(1.0,  1.0,  1.0),
      glm::vec3(-1.0,  1.0, -1.0),
      glm::vec3(-1.0, -1.0, -1.0),
      glm::vec3(1.0, -1.0, -1.0),
      glm::vec3(1.0,  1.0, -1.0),
    };

    for (auto& v : vertexPositions)
    {
        Vertex vertex;
        vertex.Position = v;
        // Set other vertex attributes if needed
        vertices.push_back(vertex);
    }
    // Flip the vertex data
    for (auto& vertex : vertices) {
        vertex.Position = glm::vec3(vertex.Position.x, -vertex.Position.y, vertex.Position.z);
    }

    std::vector<unsigned int> indices =
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
    // set up the skybox textures
    const std::string right("right.png");
    const std::string left("left.png");
    const std::string top("top.png");
    const std::string bottom("bottom.png");
    const std::string front("front.png");
    const std::string back("back.png");

    // Access the singleton instance of ResourceManager
    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::vector<std::string> textures_faces;
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + right));
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + left));
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + top));
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + bottom));
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + back));
    textures_faces.push_back(resourceManager.getResourcePath(path_skyBox + front));
    std::vector<Texture> textures_loaded = loadTextures(textures_faces);

    m_sky = std::make_unique<Mesh>(vertices, indices, textures_loaded);
}

void Skybox::render(Shader& shader_skyBox)
{
    shader_skyBox.activate();
    m_sky->Render(shader_skyBox);
}

std::vector<Texture> Skybox::loadTextures(std::vector<std::string> textures_faces)
{
    std::vector<Texture> textures_loaded;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    // Load the skybox texture
    for (int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            if (glGetError())
            {
                std::cout << "Texture images loaded failed" << std::endl;
            }

            stbi_image_free(data);
        }
        else
        {
            std::cout << "STBI failed to load skyBox texture: " << textures_faces[i] << std::endl;
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
    for (int i = 0; i < 6; i++)
    {
        textures_loaded.push_back(Texture(textureID, "skyBox", textures_faces[i]));
    }

    return textures_loaded;
}
}