#include"skyDome.h"
#include"../resourceManager.h"
#include"..\stb_image.h"
#include"..\model.h"

namespace ntn 
{

void SkyDome::render(Shader& shader)
{
    shader.activate();
    m_sky->RenderSkyDome(shader);
}

void SkyDome::initSkyDome(unsigned int nbr_rows, unsigned int nbr_cols, float radius)
{
    std::vector<Vertex> vertices = generateVertices(nbr_rows, nbr_cols, radius);
    std::string skyDomeTexture = ResourceManager::getInstance().getResourcePath("skydome/skydome.jpg");
    std::vector<Texture> texturesLoaded = loadTextures(std::vector<std::string>({ skyDomeTexture }));

    m_sky = std::make_unique<Mesh>(vertices, texturesLoaded);
}
std::vector<Vertex> SkyDome::generateVertices(int numRows, int numCols, float radius)
{
    std::vector<Vertex> vertices;
    float pitchStep = 90.0f / (float)numRows;
    float headingStep = 360.0f / (float)numCols;

    glm::vec3 apex(0.0f, radius, 0.0f);

    float pitch = -90.0f;

    // Generate Top Strip
    for (float heading = 0.0f; heading < 360.0f; heading += headingStep) 
    {
        glm::vec3 pos = apex;
        vertices.push_back(createVertex(pos));

        glm::vec3 pos1 = calculateSphericalCoords(radius, pitch + pitchStep, heading + headingStep);
        vertices.push_back(createVertex(pos1));

        glm::vec3 pos2 = calculateSphericalCoords(radius, pitch + pitchStep, heading);
        vertices.push_back(createVertex(pos2));
    }
    // Generate Remaining Strips
    for (pitch = -90.0f + pitchStep; pitch < 0; pitch += pitchStep) 
    {
        for (float heading = 0.0f; heading < 360.0f; heading += headingStep) 
        {
            glm::vec3 pos0 = calculateSphericalCoords(radius, pitch, heading);         
            glm::vec3 pos1 = calculateSphericalCoords(radius, pitch, heading + headingStep);
            glm::vec3 pos2 = calculateSphericalCoords(radius, pitch + pitchStep, heading);
            glm::vec3 pos3 = calculateSphericalCoords(radius, pitch + pitchStep, heading + headingStep);
            // First Triangle
            vertices.push_back(createVertex(pos0));
            vertices.push_back(createVertex(pos1));
            vertices.push_back(createVertex(pos2));

            // Second Triangle
            vertices.push_back(createVertex(pos1));
            vertices.push_back(createVertex(pos3));
            vertices.push_back(createVertex(pos2));
        }
    }
    return vertices;
}

Vertex SkyDome::createVertex(const glm::vec3& position) 
{
    Vertex vertex;
    glm::vec3 normalizedPos = glm::normalize(position);
    float u = 0.5f + atan2(normalizedPos.z, normalizedPos.x) / (2.0f * glm::pi<float>());
    float v = 0.5f - asin(normalizedPos.y) / (2.0f * glm::pi<float>());

    vertex.Position = position;
    vertex.Normal = normalizedPos;
    vertex.TexCoords = glm::vec2(u, v);
    return vertex;
}
glm::vec3 SkyDome::calculateSphericalCoords(float radius, float pitch, float heading)
{
    float x = radius * cosf(glm::radians(pitch)) * sinf(glm::radians(heading));
    float y = -radius * sinf(glm::radians(pitch));
    float z = radius * cosf(glm::radians(pitch)) * cosf(glm::radians(heading));
    return glm::vec3(x, y, z);
}

std::vector<Texture> SkyDome::loadTextures(std::vector<std::string> textures_skydome)
{
    std::vector<Texture> texturesLoaded;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(textures_skydome[0].c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (glGetError())
        {
            std::cout << "Texture images loaded failed" << std::endl;
        }

        stbi_image_free(data);
    }
    else
    {
        std::cout << "STBI failed to load skydome texture: " << textures_skydome[0] << std::endl;
        glDeleteTextures(1, &textureID);
        stbi_image_free(data);
    }
    
    texturesLoaded.push_back(Texture(textureID, "skydome", textures_skydome[0]));
 
    return texturesLoaded;
}
}