#pragma once

#define NOMINMAX
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

namespace ntn
{

#define MAX_BONE_INFLUENCE 4

struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal=glm::vec3(0.0f,1.0f,0.0f);
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec2 a_Postion; // additional  2D vector attribute
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture 
{
    unsigned int id;
    std::string type;
    std::string path;

    Texture() : id(0), type(""), path("") {}
    Texture(unsigned int id, const std::string& type, const std::string& path)
        : id(id), type(type), path(path) {}
};

class Mesh 
{
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO, VBO, EBO;

    Mesh();

    Mesh(std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        std::vector<Texture>& textures);

    Mesh(std::vector<Vertex>& vertices, 
        std::vector<Texture>& textures);

    // Terrain tesselation
    Mesh(std::vector<Vertex>& vertices, 
        std::vector<Texture>& textures, 
        int val);

    ~Mesh();

    void render(Shader& shader);
    void renderSkyBox(Shader& shader);
    void RenderSkyDome(Shader& shader);
    void RenderTesselation(Shader& shader);
    void RenderTerrain(Shader& shader, int res, int nInstances);

private:

    void setupMesh();
    void setupMeshWithoutIndices();
    void setupTessMesh();
};
}