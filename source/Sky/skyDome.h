#pragma once

#include "abstractSky.h"
#include "..\mesh.h"

namespace ntn
{

class SkyDome : public AbstractSky
{
public:

    explicit SkyDome(int numRows =50, int numCols =100, float radius=1.0f) :AbstractSky()
    {
        initSkyDome(numRows, numCols, radius);
    }

    void render(Shader& shader) override;
    std::vector<Texture> loadTextures(std::vector<std::string> textures) override;

private:
    void initSkyDome(unsigned int numRows, unsigned int numCols, float radius);
    std::vector<Vertex> generateVertices(int numRows, int numCols, float radius);
    
    glm::vec3 calculateSphericalCoords(float radius, float pitch, float heading);
    Vertex createVertex(const glm::vec3& position);



};
}