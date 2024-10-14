#pragma once

#include "AbstractSky.h"
#include "..\mesh.h"

namespace ntn
{

class Skybox:public AbstractSky
{
public:
    explicit Skybox();

    ~Skybox() {};

    void render(Shader& shader) override ;
    std::vector<Texture> loadTextures(std::vector<std::string>& texturesFaces);

private:
    void initSkyBox();
    std::vector<Vertex> generateCubeVertices() const;
    std::vector<unsigned int> generateCubeIndices() const;

};
}

